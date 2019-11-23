#include "SharedVars.hlsli"
#include "AgentData.hlsli"

StructuredBuffer<AgentData> oldAgentData : register(t0);

RWStructuredBuffer<AgentData> newAgentData : register(u0);

#define blocksize 32

[numthreads(blocksize, 1, 1)]
void AgentCSIterate( uint3 DTid : SV_DispatchThreadID )
{
	float3 pos = oldAgentData[DTid.x].pos.xyz;
	float3 vel = oldAgentData[DTid.x].velo.xyz;
	bool alive = oldAgentData[DTid.x].type >= 0; 

	// Calculate the new position
	pos += g_fElapsedTime * vel;

	if (alive && DTid.x < g_iMaxAliveAgents)
	{
		newAgentData[DTid.x].pos = float4(pos, 1.0f);
		newAgentData[DTid.x].velo = float4(vel, 0.0f);
	}
}

