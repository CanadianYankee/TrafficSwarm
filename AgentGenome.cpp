#include "pch.h"
#include "AgentGenome.h"

void CAgentGenome::MakeDefault()
{
	fRepulseDist = 2.0f;
	fRepulseStrength = 10.0f;

	fWallRepulseDist = 1.5f;
	fWallRepulseStrength = 10.0f;

	fMinAlignDist = 1.0f;
	fMaxAlignDist = 6.0f;
	fAlignAtMin = 1.0f;
	fAlignAtMax = 0.0f;
	fAlignAtRear = 0.5f;

	fWallAlignDist = 6.0f;
	fWallAlign = 1.5f;
	fWallAlignAtRear = 0.5f;
}

