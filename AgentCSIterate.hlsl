#include "SharedVars.hlsli"
#include "AgentData.hlsli"

struct Segment
{
	float2 end1;
	float2 end2;
};

StructuredBuffer<AgentData> oldAgentData : register(t0);
StructuredBuffer<Segment> segWallsSinks : register(t1);

RWStructuredBuffer<AgentData> newAgentData : register(u0);

// Find the closest point on a line segment to a test point (clamped to endpoints)
float2 ClosestPoint(in float2 test, in float2 end1, in float2 end2)
{
	float t = saturate(dot(test - end1, end2 - end1) / dot(end2 - end1, end2 - end1));
	return end1 + t * (end2 - end1);
}

// Calculate two-body effect (continuously accumulated).  Return "true" if agents have collided
bool Calculate2Body(inout float2 accumVel, in uint id1, in uint id2)
{
	if (id1 == id2 || oldAgentData[id1].type < 0 || oldAgentData[id2].type < 0)
		return false;
	float2 pos1 = oldAgentData[id1].pos.xy;
	float2 pos2 = oldAgentData[id2].pos.xy;

	float dist = distance(pos1, pos2);

	// Check for collision
	bool bBounce = false;
	if (dist <= g_fAgentRadius * 2.0f)
	{
		float2 vel1 = oldAgentData[id1].velo.xy;
		float2 vel2 = oldAgentData[id2].velo.xy;
		if (dot(vel2 - vel1, pos2 - pos1) < 0)
		{
			bBounce = true;
			float2 vecAxis = normalize(pos2 - pos1);
			accumVel = vel1 + dot(vel2 - vel1, vecAxis) * vecAxis;
		}
	}

	if (!bBounce)
	{
		// Short-range repulsion
		if (dist < g_fRepulseDist)
		{
			accumVel += g_fRepulseStrength * (dist - g_fRepulseDist) * normalize(pos2 - pos1);
		}
	}

	return bBounce;
}

bool CalculateBodyWall(inout float2 accumVel, in uint idB, in uint idW)
{
	float2 posB = oldAgentData[idB].pos.xy;
	float2 posW = ClosestPoint(posB, segWallsSinks[idW].end1, segWallsSinks[idW].end2);

	float dist = distance(posB, posW);

	bool bBounce = false;
	if (dist <= g_fAgentRadius * 2.0f)
	{
		float2 vel = oldAgentData[idB].velo.xy;
		if (dot(posB - posW, vel) < 0)
		{
			bBounce = true;
			float2 vecAxis = normalize(posB - posW);
			accumVel = (vel - 2.0f * dot(vel, vecAxis) * vecAxis);
		}
	}

	// Short-range repulsion
	if (dist < g_fWallRepulseDist)
	{
		accumVel += g_fWallRepulseStrength * (dist - g_fWallRepulseDist) * normalize(posW - posB);
	}

	return bBounce;
}

#define blocksize 128

[numthreads(blocksize, 1, 1)]
void AgentCSIterate( uint3 DTid : SV_DispatchThreadID )
{
	uint iAgent = DTid.x;
	AgentData agent = oldAgentData[iAgent];
	float2 pos = agent.pos.xy;
	float2 vel = agent.velo.xy;
	bool alive = agent.type >= 0;

	if (iAgent < g_iMaxAliveAgents && alive)
	{
		// Velocity towards the appropriate sink
		uint iSink = agent.type + g_iNumWalls;
		float2 ptSink = ClosestPoint(pos, segWallsSinks[iSink].end1, segWallsSinks[iSink].end2);
		float2 velSink = normalize(ptSink - pos) * g_fIdealSpeed;

		float2 velOthers = float2(0.0f, 0.0f);

		// Interaction with other agents
		bool bCollide = false;
		float2 velBounce;
		for (uint iOther = 0; iOther < g_iMaxAliveAgents; iOther++)
		{
			bool bColl = Calculate2Body(velOthers, iAgent, iOther);
			bCollide = bCollide || bColl;
			if (bColl)
			{
				velBounce = velOthers;
				break;
			}
		}

		// Interaction with walls
		float2 velWalls = float2(0.0f, 0.0f);
		for (uint iWall = 0; iWall < g_iNumWalls; iWall++)
		{
			bool bColl = CalculateBodyWall(velWalls, iAgent, iWall);
			bCollide = bCollide || bColl;
			if (bColl)
			{
				velBounce = velWalls;
				break;
			}
		}

		if (bCollide)
		{
			vel = velBounce;
		}
		else
		{
			float2 velIdeal = velSink + velOthers + velWalls;
			velIdeal = normalize(velIdeal) * g_fIdealSpeed;

			// Limit acceleration
			float2 acc = (velIdeal - vel);
			float accMag = length(acc);
			if (accMag > g_fMaxAcceleration)
			{
				acc *= g_fMaxAcceleration / accMag;
			}

			// Calculate the new velocity
			vel += g_fElapsedTime * acc;
		}

		if (length(vel) > g_fIdealSpeed)
		{
			vel = g_fIdealSpeed * normalize(vel);
		}

		// Calculate new position
		pos += g_fElapsedTime * vel;

		agent.pos = float4(pos, 0.0f, 1.0f);
		agent.velo = float4(vel, 0.0f, 0.0f);

		// Check for death 
		if (distance(pos, ptSink) < g_fAgentRadius)
		{
			agent.type = -1;
			agent.score += g_fGlobalTime - agent.birth;
		}

		// Assess collision penalty
		if (bCollide)
		{
			agent.score += g_fCollisionPenalty;
			agent.lastCollision = g_fGlobalTime;
		}

		// Check for running off either end of the course
		if (pos.x > g_fCourseLength || pos.x < 0.0f)
		{
			agent.type = -1;
			agent.score += g_fGlobalTime - agent.birth + g_fCollisionPenalty;
		}
	}
	newAgentData[iAgent] = agent;
}

