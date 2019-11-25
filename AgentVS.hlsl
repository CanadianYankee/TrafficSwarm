// Render agents vertex shader.  
// Input is agent number and color.
// Output is agent position (extracted from structured buffer maintained by the 
//    physics compute shader) in untransformed space and a unit vector pointing 
//    in the direction of the velocity.  Input color is passed on unchanged.

#include "SharedVars.hlsli"
#include "AgentData.hlsli"

cbuffer cbRenderVariables : register(b2)
{
	float4 g_arrColors[4];
};

StructuredBuffer<AgentData>   g_bufAgentData;

// Input to vertex shader 
struct AgentVSIn
{
	int		dummy	: DUMMY;		// not used
	uint	id		: SV_VERTEXID;	// auto-generated vertex id
};

// Output from vertex shader
struct AgentVSOut
{
	float2 position	: POSITION0;	// agent 3D position
	float2 direction : POSITION1;	// agent direction of travel
	float4 color	: COLOR;		// particle color
	bool alive      : ALIVE;		// if false, then geometry shader should swallow this
};

// Just look up our particle positions and pass them on to the 
// geometry shader - all world transformations will happen there.
// The geometry shader will also suppress dead agents if alive <- false.
AgentVSOut AgentVS(AgentVSIn input)
{
	AgentVSOut output;
	AgentData agent = g_bufAgentData[input.id];

	if (agent.type < 0)
	{
		output.alive = false;
	}
	else
	{
		output.position = agent.pos.xy;
		float2 velo = agent.velo.xy;
		output.direction = any(velo) ? normalize(velo) : float2(1.0, 0.0);
		output.color = (g_fGlobalTime - agent.lastCollision < 0.2f) ?
			lerp(g_arrColors[agent.type], float4(1.0f, 0.5f, 0.75f, 1.0f), (g_fGlobalTime - agent.lastCollision) / 0.2f)
			: g_arrColors[agent.type];
		output.alive = true;
	}

	return output;
}
