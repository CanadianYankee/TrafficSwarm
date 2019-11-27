struct AgentData
{
	float4 pos;
	float4 velo;
	float birth;
	float lifetime;
	float lastCollision;
	uint nAACollisions;
	uint nAWCollisions;
	int offCourse;
	int type;
};

struct DeadAgent
{
	float lifetime;
	uint nAACollisions;
	uint nAWCollisions;
	int offCourse;
};
