#include "AgentData.hlsli"

RWStructuredBuffer<AgentData> arrAgents : register(u0);

cbuffer cbSpawnAgent
{
	float2 pos;
	float2 velo;
	float birth;
	int type;
	uint maxLiveAgents;
	uint maxAgents;
};

[numthreads(1, 1, 1)]
void AgentCSSpawn( uint3 DTid : SV_DispatchThreadID )
{
	AgentData newAgent;

	newAgent.pos = float4(pos.xy, 0.0f, 1.0f);
	newAgent.velo = float4(velo.xy, 0.0f, 0.0f);
	newAgent.birth = birth;
	newAgent.score = 0.0f;
	newAgent.type = type;

	if (maxLiveAgents < maxAgents)
	{
		arrAgents[maxLiveAgents] = newAgent;
	}
}