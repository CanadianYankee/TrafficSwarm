cbuffer cbWorldPhysics : register(b0)
{
	float g_fCourseLength;
	float g_fAgentRadius;
	float g_fIdealSpeed;
	float g_fMaxAcceleration;

	uint g_iMaxAgents;
	uint g_iNumWalls;
	uint g_iNumSinks;
	float g_fCollisionPenalty;

	float g_fRepulseDist;
	float g_fRepulseStrength;
	float g_fWallRepulseDist;
	float g_fWallRepulseStrength;

	//float wpfDummy0;
	//float wpfDummy1;
	//float wpfDummy2;
};

cbuffer cbFrameVariables : register(b1)
{
	float4x4 g_matView;
	float g_fGlobalTime;
	float g_fElapsedTime;
	uint g_iMaxAliveAgents;
	uint fviDummyo;
};
