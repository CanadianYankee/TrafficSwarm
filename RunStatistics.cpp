#include "pch.h"
#include "RunStatistics.h"

CRunStatistics::CRunStatistics(UINT iTotalRunCount) : 
	m_nTotalRunSize(iTotalRunCount), 
	m_nSpawnFails(0),
	m_fLastTime(0.0f)
{
	if (m_nTotalRunSize)
	{
		m_vecFinalScores.reserve(m_nTotalRunSize);
	}
}

UINT CRunStatistics::LogSpawnFail(float fTime, UINT count)
{
#ifdef DEBUG
	OutputDebugString(_T("Spawn failure\n"));
#endif 
	m_fLastTime = fTime;
	m_nSpawnFails += count;
	return m_nSpawnFails;
}

UINT CRunStatistics::LogFinalScore(float fTime, float fScore)
{
	if (!m_nTotalRunSize || GetNumComplete() < m_nTotalRunSize)
	{
#ifdef DEBUG
		CString str;
		str.Format(_T("Dead score = %f\n"), fScore);
		OutputDebugString(str);
#endif 
		m_fLastTime = fTime;
		m_vecFinalScores.push_back(fScore);
	}
	else
	{
		ASSERT(FALSE);
	}

	return (UINT)m_vecFinalScores.size();
}

UINT CRunStatistics::LogFinalScore(float fTime, const std::vector<float>& vecScores)
{
	UINT num = 0;
	for (size_t i = 0; i < vecScores.size(); i++)
	{
		num = LogFinalScore(fTime, vecScores[i]);
	}

	return num;
}

float CRunStatistics::GetAverageScore(float fSpawnFailPenalty)
{
	float fTotal = 0;
	for (size_t i = 0; i < m_vecFinalScores.size(); i++)
	{
		fTotal += m_vecFinalScores[i];
	}

	return fTotal / (float)m_vecFinalScores.size() + fSpawnFailPenalty * (float)m_nSpawnFails;
}
