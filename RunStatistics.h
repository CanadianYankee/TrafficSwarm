#pragma once

#include "TrialRun.h"

class CRunStatistics
{
public:
	// If total run count is zero, assuming an open-ended run. 
	CRunStatistics(UINT nTotalRunSize = 0);

	struct DEAD_AGENT
	{
		DEAD_AGENT() : lifetime(0.0f), nAACollisions(0), nAWCollisions(0), offCourse(0) {}
		float lifetime;
		UINT nAACollisions;
		UINT nAWCollisions;
		int offCourse;
	};

	UINT LogDeadAgent(float fTime, const DEAD_AGENT &deadAgent);
	UINT GetTotalRunSize() { return m_nTotalRunSize; }
	UINT GetNumComplete() { return (UINT)m_vecLifetimes.size();  }
	float GetLastTime() { return m_fLastTime; }
	void RecordRunResults(CTrialRun::RUN_RESULTS& results);

protected:
	UINT m_nTotalRunSize;	
	UINT m_nSpawnFails;
	UINT m_nLeftEscapes;
	UINT m_nRightEscapes;
	UINT m_nAACollisions;
	UINT m_nAWCollisions;
	float m_fLastTime;
	std::vector<float> m_vecLifetimes;
};
