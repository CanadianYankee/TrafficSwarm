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
bool Calculate2Body(inout float2 accumVel, inout float accCaution, in uint id1, in uint id2)
{
	if (id1 == id2 || oldAgentData[id1].type < 0 || oldAgentData[id2].type < 0)
		return false;
	float2 pos1 = oldAgentData[id1].pos.xy;
	float2 pos2 = oldAgentData[id2].pos.xy;
	float2 vel1 = oldAgentData[id1].velo.xy;
	float2 vel2 = oldAgentData[id2].velo.xy;

	float2 vecAxis = normalize(pos2 - pos1);
	float dist = distance(pos1, pos2);

	// Check for collision - if there is one, do hard-sphere interactions
	bool bBounce = false;
	if (dist <= g_fAgentRadius * 2.0f)
	{
		if (dot(vel2 - vel1, pos2 - pos1) < 0)
		{
			bBounce = true;
			accumVel = vel1 + dot(vel2 - vel1, vecAxis) * vecAxis;
		}
	}

	if (!bBounce)
	{
		// Short-range repulsion
		if (dist <= g_fRepulseDist)
		{
			accumVel += g_fRepulseStrength * (dist - g_fRepulseDist) * vecAxis;
		}

		// Velocity alignment
		if (dist >= g_fMinAlignDist && dist <= g_fMaxAlignDist && g_fMaxAlignDist > 0.0f && g_fMaxAlignDist > g_fMinAlignDist)
		{
			float attenuate = lerp(g_fAlignAtMin, g_fAlignAtMax, (dist - g_fMinAlignDist) / (g_fMaxAlignDist - g_fMinAlignDist));
			attenuate *= lerp(1.0f, g_fAlignAtRear, 0.5f * (dot(normalize(vel1), vecAxis) + 1.0f));
			accumVel += (vel2 - vel1) * attenuate;
		}
	}

	// Caution factor for delta velocity with nearby agents
	if (dist < g_fCautionVelDist)
	{
		accCaution += g_fCautionVelStrength * (1.0f - dist / g_fCautionVelDist) * length(vel1 - vel2) / g_fCautionVelDist;
	}

	return bBounce;
}

bool CalculateBodyWall(inout float2 accumVel, in uint idB, in uint idW)
{
	float2 posB = oldAgentData[idB].pos.xy;
	float2 posW = ClosestPoint(posB, segWallsSinks[idW].end1, segWallsSinks[idW].end2);
	float2 vel = oldAgentData[idB].velo.xy;

	float dist = distance(posB, posW);
	float2 vecAxis = normalize(posB - posW);

	// Check for collision - if there is one, do hard-sphere bounce off wall calculation
	bool bBounce = false;
	if (dist <= g_fAgentRadius * 2.0f)
	{
		if (dot(posB - posW, vel) < 0)
		{
			bBounce = true;
			accumVel = (vel - 2.0f * dot(vel, vecAxis) * vecAxis);
		}
	}

	// Short-range repulsion
	if (dist < g_fWallRepulseDist && !bBounce)
	{
		accumVel += g_fWallRepulseStrength * (dist - g_fWallRepulseDist) * normalize(posW - posB);
	}

	// "Velocity alignment" with mirror image 
	if (dist >= g_fMinWallAlignDist && dist <= g_fMaxWallAlignDist && g_fMaxWallAlignDist > 0.0f && g_fMaxWallAlignDist > g_fMinWallAlignDist)
	{
		float attenuate = lerp(g_fWallAlignAtMin, g_fWallAlignAtMax, (dist - g_fMinWallAlignDist) / (g_fMaxWallAlignDist - g_fMinWallAlignDist));
		attenuate *= lerp(1.0f, g_fWallAlignAtRear, 0.5f * (dot(normalize(vel), vecAxis) + 1.0f));
		accumVel += (vel - 2.0f * dot(vel, vecAxis) * vecAxis) * attenuate;
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
		float fCaution = 0.0f;

		// Interaction with other agents
		bool bCollideAA = false;
		float2 velBounce;
		for (uint iOther = 0; iOther < g_iMaxAliveAgents; iOther++)
		{
			bool bColl = Calculate2Body(velOthers, fCaution, iAgent, iOther);
			bCollideAA = bCollideAA || bColl;
			if (bColl)
			{
				velBounce = velOthers;
			}
		}

		// Interaction with walls
		bool bCollideAW = false;
		float2 velWalls = float2(0.0f, 0.0f);
		for (uint iWall = 0; iWall < g_iNumWalls; iWall++)
		{
			bool bColl = CalculateBodyWall(velWalls, iAgent, iWall);
			bCollideAW = bCollideAW || bColl;
			if (bColl)
			{
				velBounce = velWalls;
				break;
			}
		}

		// Adjust ideal speed for caution corrections
		if (fCaution < -0.5f) fCaution = -0.5f;
		float fIdealSpeed = g_fIdealSpeed / (1.0f + fCaution);
		if (fIdealSpeed > g_fIdealSpeed)
		{
			fIdealSpeed = sqrt(fIdealSpeed * g_fIdealSpeed);
		}

		// Assess collision penalty
		if (bCollideAW)
		{
			agent.nAWCollisions++;
			agent.lastCollision = g_fGlobalTime;
			vel = velBounce;
		}
		else if (bCollideAA)
		{
			agent.nAACollisions++;
			agent.lastCollision = g_fGlobalTime;
			vel = velBounce;
		}
		else
		{
			float2 velIdeal = velSink + velOthers + velWalls;
			velIdeal = normalize(velIdeal) * fIdealSpeed;

			// Accelerate towards ideal velocity
			float2 acc = (velIdeal - vel);

			// Calculate the new velocity
			vel += g_fElapsedTime * acc;
		}

		// Suppress overly large velocities
		if (length(vel) > fIdealSpeed)
		{
			vel /= sqrt(length(vel)/fIdealSpeed);
		}

		// Calculate new position
		pos += g_fElapsedTime * vel;

		// Store position/velocity
		agent.pos = float4(pos, 0.0f, 1.0f);
		agent.velo = float4(vel, 0.0f, 0.0f);

		// Check for reaching the sink
		if (distance(pos, ptSink) < g_fAgentRadius)
		{
			agent.type = -1;
			agent.lifetime = g_fGlobalTime - agent.birth;
		}

		// Check for running off either end of the course
		if (pos.x > g_fCourseLength || pos.x < 0.0f)
		{
			agent.type = -1;
			agent.lifetime = g_fGlobalTime - agent.birth;
			agent.offCourse = pos.x < 0.0f ? -1 : 1;
		}
	}
	newAgentData[iAgent] = agent;
}

