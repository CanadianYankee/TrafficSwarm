#include "pch.h"
#include "RunStatistics.h"

CRunStatistics::CRunStatistics(UINT iTotalRunCount) : 
	m_nTotalRunSize(iTotalRunCount), 
	m_nSpawnFails(0),
	m_nLeftEscapes(0),
	m_nRightEscapes(0),
	m_nAACollisions(0),
	m_nAWCollisions(0),
	m_fLastTime(0.0f)
{
	if (m_nTotalRunSize)
	{
		m_vecLifetimes.reserve(m_nTotalRunSize);
	}
}

UINT CRunStatistics::LogDeadAgent(float fTime, UINT nFrames, const DEAD_AGENT &deadAgent)
{
#ifdef DEBUG
	CString str;
#endif 
	if (deadAgent.lifetime < 0.0f)
	{
#ifdef DEBUG
		str.Format(_T("T = %.2f: Spawn failure\n"), fTime);
		OutputDebugString(str);
#endif
		m_fLastTime = fTime;
		m_nSpawnFails++;
		return m_nSpawnFails;
	}

	if (!m_nTotalRunSize || GetNumComplete() < m_nTotalRunSize)
	{
#ifdef DEBUG
		CString strOffcourse;
#endif
		switch (deadAgent.offCourse)
		{
		case -1:
			m_nLeftEscapes++;
#ifdef DEBUG
			strOffcourse = _T("Left escape : ");
#endif
			break;
		case 1:
#ifdef DEBUG
			strOffcourse = _T("Right escape : ");
#endif
			m_nRightEscapes++;
			break;
		default:
			break; 
		}
#ifdef DEBUG
		str.Format(_T("T = %.2f: %s Lifespan = %.2f  AA = %d AW = %d\n"), fTime, strOffcourse.GetBuffer(), 
			deadAgent.lifetime, deadAgent.nAACollisions, deadAgent.nAWCollisions);
		OutputDebugString(str);
#endif 
		m_fLastTime = fTime;
		m_nFrames = nFrames;
		m_nAACollisions += deadAgent.nAACollisions;
		m_nAWCollisions += deadAgent.nAWCollisions;
		m_vecLifetimes.push_back(deadAgent.lifetime);
	}
	else
	{
		ASSERT(FALSE);
	}

	return (UINT)m_vecLifetimes.size();
}

void CRunStatistics::RecordRunResults(CTrialRun::RUN_RESULTS &results)
{
	float fRunSize = (float)m_nTotalRunSize;
	float fTotal = 0.0f;
	for (size_t i = 0; i < m_vecLifetimes.size(); i++)
	{
		fTotal += m_vecLifetimes[i];
	}

	results.nAgents = m_nTotalRunSize;
	results.nComplete = (UINT)m_vecLifetimes.size();
	results.nLeftEscapes = m_nLeftEscapes;
	results.nRightEscapes = m_nRightEscapes;
	results.nSpawnFails = m_nSpawnFails;
	results.fAvgAACollisions = (float)m_nAACollisions / fRunSize;
	results.fAvgAWCollisions = (float)m_nAWCollisions / fRunSize;
	results.fAvgLifetime = fTotal / fRunSize;
	results.fSimulatedTime = m_fLastTime;
	results.fFPS = (float)m_nFrames / m_fLastTime;
}