#pragma once

#include "TrialRun.h"

// Structures for reading in old versions of CTrialRun::RUN_RESULTS and agent genomes and
// translating them into the newest version.

// Read version = 1, return version = 2
struct AGENT_GENOME_1
{
	AGENT_GENOME_1::AGENT_GENOME_1() { vecGene.resize((size_t)GENE_1::NUM_GENES); }
	enum class GENE_1 : UINT
	{
		RepulseDist,
		RepulseStrength,

		WallRepulseDist,
		WallRepulseStrength,

		MinAlignDist,
		MaxAlignDist,
		AlignAtMin,
		AlignAtMax,
		AlignAtRear,

		MinWallAlignDist,
		MaxWallAlignDist,
		WallAlignAtMin,
		WallAlignAtMax,
		WallAlignAtRear,

		NUM_GENES
	};
	std::vector<float> vecGene;
};

struct RUN_RESULTS_1
{
	AGENT_GENOME_1 genome;
	UINT nAgents;
	UINT nComplete;
	UINT nSpawnFails;
	UINT nLeftEscapes;
	UINT nRightEscapes;
	float fSimulatedTime;
	float fRealTime;
	float fFPS;
	float fAvgLifetime;
	float fAvgAACollisions;
	float fAvgAWCollisions;
};

std::istream& operator >> (std::istream& in, AGENT_GENOME_1& g);
std::istream& operator >> (std::istream& in, RUN_RESULTS_1& rr);

void ConvertRunResult(CTrialRun::RUN_RESULTS& out, const RUN_RESULTS_1& in);
