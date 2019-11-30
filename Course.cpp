#include "pch.h"
#include "atlstr.h"
#include "Course.h"


// External json parser by Niels Lohmann from https://github.com/nlohmann/json
#include "nlohmann/json.hpp"

void LoadPolyline(nlohmann::json j, CCourse::XMPOLYLINE &vecPts)
{
	vecPts.clear();
	auto jpl = j.at("polyline");
	for (auto i = jpl.begin(); i != jpl.end(); i++)
	{
		XMFLOAT2 pt;
		pt.x = i->value("x", 0.0f);
		pt.y = i->value("y", 0.0f);
		vecPts.push_back(pt);
	}
}

BOOL CCourse::LoadFromFile(const CString& strFile)
{
	USES_CONVERSION;
	XMPOLYLINE vecPts;

	std::ifstream inFile(strFile);
	nlohmann::json j = nlohmann::json::parse(inFile);

	ClearAll();

	std::string strName = j.value("name", "Error");
	m_strName = A2T(strName.c_str());

	m_fCourseLength = j.value("length", 0.0f);

	auto jWalls = j.at("walls");
	for (auto i = jWalls.begin(); i != jWalls.end(); i++)
	{
		LoadPolyline(*i, vecPts);
		m_vecWalls.push_back(vecPts);
	}

	auto jAgents = j.at("agents");
	for (auto i = jAgents.begin(); i != jAgents.end(); i++)
	{
		AGENT_SOURCE_SINK agentSS;
		auto jClr = i->at("color");
		agentSS.vColor = XMFLOAT3(jClr.value("r", 0.0f), jClr.value("g", 0.0f), jClr.value("b", 0.0f));
		LoadPolyline(i->at("source"), agentSS.lineSource);
		LoadPolyline(i->at("sink"), agentSS.lineSink);

		XMVECTOR src1 = XMLoadFloat2(&agentSS.lineSource[0]);
		XMVECTOR src2 = XMLoadFloat2(&agentSS.lineSource[1]);
		XMVECTOR snk1 = XMLoadFloat2(&agentSS.lineSink[0]);
		XMVECTOR snk2 = XMLoadFloat2(&agentSS.lineSink[1]);
		XMStoreFloat(&agentSS.lenSource, XMVector2Length(src2 - src1));

		XMVECTOR dir = (snk1 + snk2) - (src1 + src2);
		dir = XMVector2Normalize(dir);
		XMStoreFloat2(&agentSS.velDir, dir);
		
		m_vecAgentSS.push_back(agentSS);
	}

	float fTotal = GetTotalSourceLength();
	float fAccum = 0.0f;
	for (size_t i = 0; i < m_vecAgentSS.size(); i++)
	{
		m_vecAgentSS[i].randLimit = (m_vecAgentSS[i].lenSource + fAccum) / fTotal;
		fAccum += m_vecAgentSS[i].lenSource;
	}

	return 0;
}

BOOL CCourse::LoadHourglass()
{
	m_strName = _T("Hourglass");
	m_fCourseLength = 100.0f;

	m_vecWalls.resize(2);

	m_vecWalls[0].resize(3);
	m_vecWalls[0][0] = XMFLOAT2(0.0f, -15.0f);
	m_vecWalls[0][1] = XMFLOAT2(50.0f, -5.0f);
	m_vecWalls[0][2] = XMFLOAT2(100.0f, -15.0f);

	m_vecWalls[1].resize(3);
	m_vecWalls[1][0] = XMFLOAT2(0.0f, 15.0f);
	m_vecWalls[1][1] = XMFLOAT2(50.0f, 5.0f);
	m_vecWalls[1][2] = XMFLOAT2(100.0f, 15.0f);

	m_vecAgentSS.resize(1);
	m_vecAgentSS[0].vColor = XMFLOAT3(0.5f, 0.5f, 1.0f);
	m_vecAgentSS[0].randLimit = 1.0f;

	m_vecAgentSS[0].lineSource.resize(2);
	m_vecAgentSS[0].lineSource[0] = XMFLOAT2(0.0f, -13.5f);
	m_vecAgentSS[0].lineSource[1] = XMFLOAT2(0.0f, 13.5f);
	m_vecAgentSS[0].lenSource = m_vecAgentSS[0].lineSource[1].y - m_vecAgentSS[0].lineSource[0].y;

	m_vecAgentSS[0].lineSink.resize(2);
	m_vecAgentSS[0].lineSink[0] = XMFLOAT2(100.0f, -13.5f);
	m_vecAgentSS[0].lineSink[1] = XMFLOAT2(100.0f, 13.5f);

	m_vecAgentSS[0].velDir = XMFLOAT2(1.0f, 0.0f);

	return TRUE;
}

float CCourse::GetTotalSourceLength() const
{
	float fTotal = 0.0f;
	for (size_t i = 0; i < m_vecAgentSS.size(); i++)
	{
		fTotal += m_vecAgentSS[i].lenSource;
	}
	return fTotal;
}

void CCourse::ClearAll()
{
	m_strName.Empty();
	m_fCourseLength = 0.0f;
	m_vecWalls.clear();
	m_vecAgentSS.clear();
}