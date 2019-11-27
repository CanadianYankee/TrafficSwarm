#include "pch.h"
#include "Course.h"

BOOL CCourse::LoadFromFile(const CString& strFile)
{
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

float CCourse::GetTotalSourceLength()
{
	float fTotal = 0.0f;
	for (size_t i = 0; i < m_vecAgentSS.size(); i++)
	{
		fTotal += m_vecAgentSS[i].lenSource;
	}
	return fTotal;
}
