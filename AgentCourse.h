#pragma once

using namespace DirectX;

inline float frand()
{
	return (float)rand() / (float)RAND_MAX;
}

// This class defines the course that the agents must 
// navigate, including walls and agent source-sinks. It 
// also handles the DirectX objects for doing the physics
// and drawing of the walls. 
class CAgentCourse
{
public:
	CAgentCourse();

	BOOL Initialize(const CString &strJsonFile);
	typedef std::vector<XMVECTOR> XMPOLYLINE;
	struct AGENT_SOURCE_SINK {
		XMVECTOR vColor;
		XMPOLYLINE lineSource;
		XMPOLYLINE lineSink;
	};
	CString GetName() { return m_strName; }
	float GetCourseLength() { return m_fCourseLength; }
	int GetSpawnCount() { return m_vecAgentSS.size(); }
	XMVECTOR GetColor(int iIndex) { return m_vecAgentSS[iIndex].vColor; }
	XMVECTOR GetSpawnPoint(int iIndex);

protected:
	BOOL InitializeHourglass();

	CString m_strName;
	float m_fCourseLength;
	std::vector<XMPOLYLINE> m_vecWalls;
	std::vector<AGENT_SOURCE_SINK> m_vecAgentSS;
};

