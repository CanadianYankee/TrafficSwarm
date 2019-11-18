// Render agent pixel shader.  
// Input texture coordinates, particle color.
// Output is texture color/alpha shaded with particle color

texture2D g_txAgentDraw;			// Image for each billboarded agent

SamplerState g_sampleTexture
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct AgentPSIn
{
	float2 texCoord	: TEXCOORD0;	// particle texture coordinates
	float4 color	: COLOR;		// particle color
};

float4 AgentPS(AgentPSIn input) : SV_TARGET
{
	float4 color = g_txAgentDraw.Sample(g_sampleTexture, input.texCoord) * input.color;

	return color;
}
