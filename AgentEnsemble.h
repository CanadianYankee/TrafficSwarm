#pragma once

extern class CAgentCourse;

// This class defines the genome of the agents and keeps 
// track of spawning, scoring, and destroying them.  It 
// also handles the DirectX objects for doing the physics
// and drawing of the agents. 
class CAgentEnsemble
{
public:
	CAgentEnsemble(CAgentCourse* m_pAgentCourse);
	int GetCount() { return m_iCount; }
	void UpdateEnsemble(float dt, float T);
	void SetSpawnRate(float fRate) { m_fSpawnRate = fRate; }

protected:
	void SpawnAgent();

	CAgentCourse *m_pAgentCourse;
	int m_iCount;
	int m_iTypes;
	float m_fNextSpawn;
	float m_fSpawnRate; 
	float m_fInitialVelocity;
};

