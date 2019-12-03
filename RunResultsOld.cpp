#include "pch.h"
#include "RunResultsOld.h"

std::istream& operator >> (std::istream& in, AGENT_GENOME_1& g)
{
	for (size_t i = 0; i < (size_t)AGENT_GENOME_1::GENE_1::NUM_GENES; i++)
	{
		float f;
		in >> f;
		g.vecGene[i] = f;
	}
	return in;
}

std::istream& operator >> (std::istream& in, RUN_RESULTS_1& rr)
{
	in >> rr.genome;
	in >> rr.nAgents >> rr.nComplete >> rr.nSpawnFails >> rr.nLeftEscapes >> rr.nRightEscapes >>
		rr.fSimulatedTime >> rr.fRealTime >> rr.fFPS >> rr.fAvgLifetime >> rr.fAvgAACollisions >> rr.fAvgAWCollisions;

	return in;
}

// Convet version = 1 to version = 2
void ConvertGenome(CAgentGenome& out, const AGENT_GENOME_1& in)
{
	out.ZeroAll();
	out.SetGene(CAgentGenome::GENE::RepulseDist, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::RepulseDist]);
	out.SetGene(CAgentGenome::GENE::RepulseStrength, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::RepulseStrength]);
	out.SetGene(CAgentGenome::GENE::WallRepulseDist, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::WallRepulseDist]);
	out.SetGene(CAgentGenome::GENE::WallRepulseStrength, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::WallRepulseStrength]);
	out.SetGene(CAgentGenome::GENE::MinAlignDist, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::MinAlignDist]);
	out.SetGene(CAgentGenome::GENE::MaxAlignDist, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::MaxAlignDist]);
	out.SetGene(CAgentGenome::GENE::AlignAtMin, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::AlignAtMin]);
	out.SetGene(CAgentGenome::GENE::AlignAtMax, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::AlignAtMax]);
	out.SetGene(CAgentGenome::GENE::AlignAtRear, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::AlignAtRear]);
	out.SetGene(CAgentGenome::GENE::MinWallAlignDist, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::MinWallAlignDist]);
	out.SetGene(CAgentGenome::GENE::MaxWallAlignDist, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::MaxWallAlignDist]);
	out.SetGene(CAgentGenome::GENE::WallAlignAtMin, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::WallAlignAtMin]);
	out.SetGene(CAgentGenome::GENE::WallAlignAtMax, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::WallAlignAtMax]);
	out.SetGene(CAgentGenome::GENE::WallAlignAtRear, in.vecGene[(size_t)AGENT_GENOME_1::GENE_1::WallAlignAtRear]);
}

void ConvertRunResult(CTrialRun::RUN_RESULTS& out, const RUN_RESULTS_1& in)
{
	ConvertGenome(out.genome, in.genome);
	out.nAgents = in.nAgents;
	out.nComplete = in.nComplete;
	out.nSpawnFails = in.nSpawnFails;
	out.nLeftEscapes = in.nLeftEscapes;
	out.nRightEscapes = in.nRightEscapes;
	out.fSimulatedTime = in.fSimulatedTime;
	out.fRealTime = in.fRealTime;
	out.fFPS = in.fFPS;
	out.fAvgLifetime = in.fAvgLifetime;
	out.fAvgAACollisions = in.fAvgAACollisions;
	out.fAvgAWCollisions = in.fAvgAWCollisions;
}
