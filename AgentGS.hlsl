// Render agent geometry shader.  
// Input is list of point positions in untransformed coordinates.
// Output is a billboard quad facing the camera - input color is passed through unchanged.

#include "SharedVars.hlsli"

cbuffer cbConstants
{
	static float2 g_quadPositions[4] =
	{
		float2(-1, 1),
		float2(1, 1),
		float2(-1, -1),
		float2(1, -1),
	};

	static float2 g_quadTexcoords[4] =
	{
		float2(0,0),
		float2(1,0),
		float2(0,1),
		float2(1,1)
	};
};

struct AgentGSIn
{
	float2 position	: POSITION0;	// agent position
	float2 direction : POSITION1;	// agent diretion of travel
	float4 color	: COLOR;		// agent color
};

struct AgentGSOut
{
	float2 texCoord	: TEXCOORD0;	// agent texture coordinates
	float4 color	: COLOR;		// particle color
	float4 position	: SV_POSITION;	// screen position
};

// Generate a quad billboard for each agent
[maxvertexcount(4)]
void AgentGS(point AgentGSIn input[1], inout TriangleStream< AgentGSOut > SpriteStream)
{
	AgentGSOut output;

	float4 center = float4(input[0].position, 0.0, 1.0);
	float4x4 facing = float4x4(
		input[0].direction.y, -input[0].direction.x, 0.0f, 0.0f,
		input[0].direction.x, input[0].direction.y, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	// Make two triangles to billboard the particle texture sprite
	for (int i = 0; i < 4; i++)
	{
		float4 offset = float4(g_quadPositions[i] * g_fAgentRadius, 0.0, 1.0);

		output.position = mul(mul(offset, facing) + center, g_matView);
		output.position /= output.position.w;
		output.color = input[0].color;
		output.texCoord = g_quadTexcoords[i];
		SpriteStream.Append(output);
	}
	SpriteStream.RestartStrip();
}
