#include "pch.h"
#include "AgentCourse.h"

CAgentCourse::CAgentCourse() :
	m_fCourseLength(0.0f)
{
}

BOOL CAgentCourse::Initialize(const CString& strJsonFile)
{
	if (strJsonFile.IsEmpty())
		return InitializeHourglass();
}

BOOL CAgentCourse::InitializeHourglass()
{
	m_strName = _T("Hourglass");
	m_fCourseLength = 100.0f;

	XMFLOAT2 point;
	m_vecWalls.resize(2);

	m_vecWalls[0].resize(3);
	point.x = 0.0f; point.y = -15.0f;
	m_vecWalls[0][0] = XMLoadFloat2(&point);
	point.x = 50.0f; point.y = -5.0f;
	m_vecWalls[0][1] = XMLoadFloat2(&point);
	point.x = 100.0f; point.y = -15.0f;
	m_vecWalls[0][2] = XMLoadFloat2(&point);

	m_vecWalls[1].resize(3);
	point.x = 0.0f; point.y = 15.0f;
	m_vecWalls[1][0] = XMLoadFloat2(&point);
	point.x = 50.0f; point.y = 5.0f;
	m_vecWalls[1][1] = XMLoadFloat2(&point);
	point.x = 100.0f; point.y = 15.0f;
	m_vecWalls[1][2] = XMLoadFloat2(&point);

	m_vecAgentSS.resize(1);
	XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vecAgentSS[0].vColor = XMLoadFloat4(&color);

	m_vecAgentSS[0].lineSource.resize(2);
	point.x = 0.0f; point.y = -13.5f;
	m_vecAgentSS[0].lineSource[0] = XMLoadFloat2(&point);
	point.x = 0.0f; point.y = 13.5f;
	m_vecAgentSS[0].lineSource[1] = XMLoadFloat2(&point);

	m_vecAgentSS[0].lineSink.resize(2);
	point.x = 100.0f; point.y = -15.0f;
	m_vecAgentSS[0].lineSink[0] = XMLoadFloat2(&point);
	point.x = 100.0f; point.y = 15.0f;
	m_vecAgentSS[0].lineSink[1] = XMLoadFloat2(&point);

	return TRUE;
}

XMVECTOR CAgentCourse::GetSpawnPoint(int iIndex)
{
	float f1 = frand();
	float f2 = 1.0f - f1;
	return f1 * m_vecAgentSS[iIndex].lineSource[0] + f2 * m_vecAgentSS[iIndex].lineSource[1];
}
