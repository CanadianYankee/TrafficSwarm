// Render walls vertex shader.  
// Input is list of vertex positions in untransformed coordinates with colors.
// Output is in transformed coordinates. 

#include "SharedVars.hlsli"

// Input to vertex shader 
struct WallVSIn
{
	float2	position : POSITION;	// wall vertex
	float3	color	 : COLOR;		// vertex color
};

// Output from vertex shader
struct WallVSOut
{
	float4 position	: SV_POSITION;	// view vertex coordinate
	float4 color	: COLOR;		// vertex color
};

WallVSOut WallVS(WallVSIn input)
{
	WallVSOut output;

	output.position = mul(float4(input.position, 0.0f, 1.0f), g_matView);
	output.color = float4(input.color, 1.0f);

	return output;
}