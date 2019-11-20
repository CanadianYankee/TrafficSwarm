#include "pch.h"
#include "AgentEnsemble.h"
#include "AgentCourse.h"

CAgentEnsemble::CAgentEnsemble(CAgentCourse* pAgentCourse) :
	m_pAgentCourse(pAgentCourse),
	m_iCount(0),
	m_fNextSpawn(0.0f),
	m_fSpawnRate(0.2f),
	m_fInitialVelocity(5.0f)
{
	ASSERT(m_pAgentCourse);
	m_iTypes = m_pAgentCourse->GetSpawnCount();
}

void CAgentEnsemble::UpdateEnsemble(float dt, float T)
{
	// If total time exceeds the next scheduled spawn time, then 
	// spawn a new agent.
	if (T >= m_fNextSpawn)
	{
		SpawnAgent();

		m_fNextSpawn = T - logf(1.0f - frand()) / m_fSpawnRate;
	}
}

void CAgentEnsemble::SpawnAgent()
{
	int iType = m_iTypes == 1 ? 1 : rand() % m_iTypes;
	XMVECTOR vecPos = m_pAgentCourse->GetSpawnPoint(iType);
	XMVECTOR vecVel = XMLoadFloat(&m_fInitialVelocity);
	XMVECTOR color = m_pAgentCourse->GetColor(iType);
}


