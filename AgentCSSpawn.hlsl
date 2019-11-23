#include "AgentData.hlsli"

cbuffer cbSpawnAgent : register(b0)
{
	float2 pos;
	float2 velo;

	float birth;
	float radius;
	int type;
	uint maxLiveAgents;

	uint maxAgents;
	uint iDummy0;
	uint iDummy1;
	uint iDummy2;
};


RWStructuredBuffer<AgentData> arrAgents : register(u0);
RWStructuredBuffer<uint> arrDeadIndices : register(u1);
RWStructuredBuffer<uint> arrSpawnOutput : register(u2);
RWStructuredBuffer<float> arrFinalScores : register(u3);

[numthreads(1, 1, 1)]
void AgentCSSpawn( uint3 DTid : SV_DispatchThreadID )
{
	AgentData newAgent;

	newAgent.pos = float4(pos.xy, 0.0f, 1.0f);
	newAgent.velo = float4(velo.xy, 0.0f, 0.0f);
	newAgent.birth = birth;
	newAgent.score = 0.0f;
	newAgent.type = type;

	// TODO: take care of dead agents
	uint nDeadCount = 0;
	uint nLiveAgents = maxLiveAgents;

	// Suppress spawn if new agent collides with a live one
	bool bSuppress = false;
	for (uint i = 0; i < nLiveAgents; i++)
	{
		if (arrAgents[i].type >= 0)
		{
			if (distance(arrAgents[i].pos.xy, newAgent.pos) <= radius * 2.0f)
			{
				arrFinalScores[nDeadCount] = -1.0f;
				nDeadCount++;
				bSuppress = true;
				break;
			}
		}
	}

	if (!bSuppress)
	{
		arrAgents[nLiveAgents] = newAgent;
		nLiveAgents++;
	}

	// For read by the CPU
	arrSpawnOutput[0] = nLiveAgents;
	arrSpawnOutput[1] = nDeadCount;
}