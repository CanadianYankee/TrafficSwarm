cbuffer cbWorldPhysics : register(b0)
{
	float g_fAgentRadius;
	float wpfDummy0;
	float wpfDummy1;
	float wpfDummy2;
};

cbuffer cbFrameVariables : register(b1)
{
	float4x4 g_matView;
	float g_fGlobalTime;
	float g_fElapsedTime;
	float fvfDummy0;
	float fvfDummy1;
};
