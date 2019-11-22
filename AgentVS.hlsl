// Render agents vertex shader.  
// Input is agent number and color.
// Output is agent position (extracted from structured buffer maintained by the 
//    physics compute shader) in untransformed space and a unit vector pointing 
//    in the direction of the velocity.  Input color is passed on unchanged.

struct PosVelo
{
	float4 pos;
	float4 velo;
};

StructuredBuffer<PosVelo>   g_bufPosVelo;

// Input to vertex shader 
struct AgentVSIn
{
	float3	color	: COLOR;		// agent color
	uint	id		: SV_VERTEXID;	// auto-generated vertex id
};

// Output from vertex shader
struct AgentVSOut
{
	float2 position	: POSITION0;	// agent 3D position
	float2 direction : POSITION1;	// agent direction of travel
	float4 color	: COLOR;		// particle color
};

// Just look up our particle positions and pass them on to the 
// geometry shader - all world transformations will happen there.
AgentVSOut AgentVS(AgentVSIn input)
{
	AgentVSOut output;

	// Look up the particle position in the texture map
	output.position = g_bufPosVelo[input.id].pos.xy;
	float2 velo = g_bufPosVelo[input.id].velo.xy;
	output.direction = any(velo) ? normalize(velo) : float2(1.0, 0.0);
	output.color = float4(input.color.xyz, 1.0f);

	return output;
}
