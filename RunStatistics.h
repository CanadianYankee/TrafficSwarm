#pragma once

class CRunStatistics
{
public:
	// If total run count is zero, assuming an open-ended run. 
	CRunStatistics(UINT nTotalRunSize = 0);

	UINT LogSpawnFail(float fTime, UINT count = 1);
	UINT LogFinalScore(float fTime, float fScore);
	UINT LogFinalScore(float fTime, const std::vector<float>& vecScores);

	UINT GetNumSpawnFails() { return m_nSpawnFails; }
	UINT GetNumComplete() { return (UINT)m_vecFinalScores.size();  }
	float GetLastTime() { return m_fLastTime; }
	float GetAverageScore(float fSpawnFailPenalty = 0.0f);

protected:
	UINT m_nTotalRunSize;
	UINT m_nSpawnFails;
	float m_fLastTime;
	std::vector<float> m_vecFinalScores;
};
