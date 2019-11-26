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
RWStructuredBuffer<uint> arrComputeOutput : register(u1);
RWStructuredBuffer<float> arrFinalScores : register(u2);

[numthreads(1, 1, 1)]
void AgentCSSpawn( uint3 DTid : SV_DispatchThreadID )
{
	AgentData newAgent;

	newAgent.pos = float4(pos.xy, 0.0f, 1.0f);
	newAgent.velo = float4(velo.xy, 0.0f, 0.0f);
	newAgent.birth = birth;
	newAgent.score = 0.0f;
	newAgent.lastCollision = -100.0f;
	newAgent.type = type;

	// Count up our dead and save the scores
	// Suppress spawn if new agent collides with a live one
	// Negative type suppresses spawn and just does dead processing.
	uint nDeadCount = 0;
	bool bSuppress = false;
	uint nLiveAgents = maxLiveAgents;
	if (newAgent.type < 0)
	{
		arrFinalScores[nDeadCount] = -1.0f;
		nDeadCount++;
		bSuppress = true;
	}
	for (int i = maxLiveAgents - 1; i >= 0; i--)
	{
		if (arrAgents[i].type < 0)
		{
			arrFinalScores[nDeadCount] = arrAgents[i].score;
			if (i < nLiveAgents - 1)
			{
				AgentData agent = arrAgents[nLiveAgents - 1];
				arrAgents[i] = agent;
			}
			nLiveAgents--;
			nDeadCount++;
		}
		else
		{
			if (!bSuppress && distance(arrAgents[i].pos.xy, newAgent.pos) <= radius * 2.0f)
			{
				arrFinalScores[nDeadCount] = -1.0f;
				nDeadCount++;
				bSuppress = true;
			}
		}
	}

	if (!bSuppress)
	{
		arrAgents[nLiveAgents] = newAgent;
		nLiveAgents++;
	}

	// For read by the CPU
	arrComputeOutput[0] = nLiveAgents;
	arrComputeOutput[1] = nDeadCount;
	arrComputeOutput[2] = !bSuppress;
}