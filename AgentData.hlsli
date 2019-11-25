struct AgentData
{
	float4 pos;
	float4 velo;
	float birth;
	float score;
	float lastCollision;
	int type;
};

struct DeadAgent
{
	uint index;
	float score;
};
