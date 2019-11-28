#include "pch.h"
#include "AgentGenome.h"

static CAgentGenome::GENE_SPECIFICATION GeneSpecs[(UINT)CAgentGenome::GENE::NUM_GENES] =
{
	{ _T("RepulseDist"), 0.0f, 10.0f},
	{ _T("RepulseStrength"), 0.0f, 20.0f},

	{ _T("WallRepulseDist"), 0.0f, 10.0f},
	{ _T("WallRepulseStrength"), 0.0f, 20.0f},

	{ _T("MinAlignDist"), 0.0f, 50.0f},
	{ _T("MaxAlignDist"), 0.0f, 50.0f},
	{ _T("AlignAtMin"), 0.0f, 2.0f},
	{ _T("AlignAtMax"), 0.0f, 2.0f},
	{ _T("AlignAtRear"), 0.0f, 2.0f},

	{ _T("WallAlignDist"), 0.0f, 20.0f},
	{ _T("WallAlign"), 0.0f, 2.0f},
	{ _T("WallAlignAtRear"), 0.0f, 2.0f}
};

CAgentGenome::CAgentGenome()
{
	m_vecGenes.resize((size_t)GENE::NUM_GENES);
}

float CAgentGenome::Gene(GENE geneId) const
{
	float fRet;

	// A wee bit of logic to sort genes that have to satisfy particular inequalities (min < max)
	switch (geneId) {
	case GENE::MaxAlignDist:
		fRet = fmax(m_vecGenes[(UINT)GENE::MaxAlignDist], m_vecGenes[(UINT)GENE::MinAlignDist]);
		break;

	case GENE::MinAlignDist:
		fRet = fmin(m_vecGenes[(UINT)GENE::MaxAlignDist], m_vecGenes[(UINT)GENE::MinAlignDist]);
		break;

	default:
		ASSERT(geneId < GENE::NUM_GENES);
		fRet = m_vecGenes[(UINT)geneId];
	}

	return fRet;
}

void CAgentGenome::RandomizeOne(GENE geneId)
{
	ASSERT(geneId < GENE::NUM_GENES);

	UINT i = (UINT)geneId;
	m_vecGenes[i] = frand() * (GeneSpecs[i].fMax - GeneSpecs[i].fMin) + GeneSpecs[i].fMin;
}

CAgentGenome::GENE CAgentGenome::RandomizeOne()
{
	GENE geneId = (GENE)(rand() % (int)GENE::NUM_GENES);
	RandomizeOne(geneId);

	return geneId;
}

void CAgentGenome::RandomizeAll()
{
	for (UINT i = 0; i < (UINT)GENE::NUM_GENES; i++)
	{
		RandomizeOne((GENE)i);
	}
}

CAgentGenome CAgentGenome::CrossBreed(const CAgentGenome& parent1, const CAgentGenome& parent2, float fMutateProb)
{
	CAgentGenome child;
	for (size_t i = 0; i < (size_t)GENE::NUM_GENES; i++)
	{
		child.m_vecGenes[i] = rand() % 0x100 ? parent1.m_vecGenes[i] : parent2.m_vecGenes[i];
	}

	if (frand() < fMutateProb)
		child.RandomizeOne();

	return child;
}


void CAgentGenome::MakeDefault()
{
	m_vecGenes[(UINT)GENE::RepulseDist] = 2.0f;
	m_vecGenes[(UINT)GENE::RepulseStrength] = 10.0f;

	m_vecGenes[(UINT)GENE::WallRepulseDist] = 1.5f;
	m_vecGenes[(UINT)GENE::WallRepulseStrength] = 10.0f;

	m_vecGenes[(UINT)GENE::MinAlignDist] = 1.0f;
	m_vecGenes[(UINT)GENE::MaxAlignDist] = 6.0f;
	m_vecGenes[(UINT)GENE::AlignAtMin] = 1.0f;
	m_vecGenes[(UINT)GENE::AlignAtMax] = 0.0f;
	m_vecGenes[(UINT)GENE::AlignAtRear] = 0.5f;

	m_vecGenes[(UINT)GENE::WallAlignDist] = 6.0f;
	m_vecGenes[(UINT)GENE::WallAlign] = 1.5f;
	m_vecGenes[(UINT)GENE::WallAlignAtRear] = 0.5f;
}

