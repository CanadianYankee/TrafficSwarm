cbuffer cbWorldPhysics : register(b0)
{
	float g_fAgentRadius;
	float g_fIdealSpeed;
	uint g_iMaxAgents;
	uint wpiDummy0;
};

cbuffer cbFrameVariables : register(b1)
{
	float4x4 g_matView;
	float g_fGlobalTime;
	float g_fElapsedTime;
	uint g_iMaxAliveAgents;
	uint fviDummyo;
};
