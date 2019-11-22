// Render walls pixel shader.  
// Does nothing to the color. 

struct WallPSIn
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
};

float4 WallPS(WallPSIn pin) : SV_TARGET
{
	return pin.color;
}
