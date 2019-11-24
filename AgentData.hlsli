struct AgentData
{
	float4 pos;
	float4 velo;
	float birth;
	float score;
	int type;
};

struct DeadAgent
{
	uint index;
	float score;
};
