#include "SharedVars.hlsli"
#include "AgentData.hlsli"

StructuredBuffer<AgentData> oldAgentData : register(t0);

RWStructuredBuffer<AgentData> newAgentData : register(u0);

#define blocksize 128

[numthreads(blocksize, 1, 1)]
void AgentCSIterate( uint3 DTid : SV_DispatchThreadID )
{
	uint iAgent = DTid.x;
	AgentData agent = oldAgentData[iAgent];
	float3 pos = agent.pos.xyz;
	float3 vel = agent.velo.xyz;
	bool alive = agent.type >= 0;

	if (iAgent < g_iMaxAliveAgents && alive)
	{
		// Calculate the new position
		pos += g_fElapsedTime * vel;

		agent.pos = float4(pos, 1.0f);
		agent.velo = float4(vel, 0.0f);

		// Check for death - TODO: actually check against sink
		if (pos.x > 100.0f)
		{
			agent.type = -1;
			agent.score += g_fGlobalTime - agent.birth;
		}
	}
	newAgentData[iAgent] = agent;
}

