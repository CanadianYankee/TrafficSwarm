#pragma once
class CAgentGenome
{
public:
	float fRepulseDist;
	float fRepulseStrength;

	float fWallRepulseDist;
	float fWallRepulseStrength;

	float fMinAlignDist;
	float fMaxAlignDist;
	float fAlignAtMin;
	float fAlignAtMax;
	float fAlignAtRear;

	float fWallAlignDist;
	float fWallAlign;
	float fWallAlignAtRear;

	void MakeDefault();
};

