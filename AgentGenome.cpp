#include "pch.h"
#include "AgentGenome.h"

static CAgentGenome::GENE_SPECIFICATION GeneSpecs[(UINT)CAgentGenome::GENE::NUM_GENES] =
{
	{ _T("RepulseDist"), 0.0f, 10.0f},
	{ _T("RepulseStrength"), 0.0f, 20.0f},

	{ _T("MinAlignDist"), 0.0f, 20.0f},
	{ _T("MaxAlignDist"), 0.0f, 20.0f},
	{ _T("AlignAtMin"), 0.0f, 2.0f},
	{ _T("AlignAtMax"), 0.0f, 2.0f},
	{ _T("AlignAtRear"), 0.0f, 2.0f},

	{ _T("CautionVelDist"), 0.0f, 10.0f},
	{ _T("CautionVelStrength"), -0.5f, 1.0f},

	{ _T("WallRepulseDist"), 0.0f, 10.0f},
	{ _T("WallRepulseStrength"), 0.0f, 20.0f},

	{ _T("MinWallAlignDist"), 0.0f, 20.0f},
	{ _T("MaxWallAlignDist"), 0.0f, 20.0f},
	{ _T("WallAlignAtMin"), 0.0f, 2.0f},
	{ _T("WallAlignAtMax"), 0.0f, 2.0f},
	{ _T("WallAlignAtRear"), 0.0f, 2.0f}
};

CAgentGenome::CAgentGenome()
{
	m_vecGenes.resize((size_t)GENE::NUM_GENES);
}

void CAgentGenome::ZeroAll()
{
	for (size_t i = 0; i < m_vecGenes.size(); i++)
	{
		m_vecGenes[i] = 0.0f;
	}
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

	case GENE::MaxWallAlignDist:
		fRet = fmax(m_vecGenes[(UINT)GENE::MaxWallAlignDist], m_vecGenes[(UINT)GENE::MinWallAlignDist]);
		break;

	case GENE::MinWallAlignDist:
		fRet = fmin(m_vecGenes[(UINT)GENE::MaxWallAlignDist], m_vecGenes[(UINT)GENE::MinWallAlignDist]);
		break;

	default:
		ASSERT(geneId < GENE::NUM_GENES);
		fRet = m_vecGenes[(UINT)geneId];
	}

	return fRet;
}

void CAgentGenome::SetGene(GENE geneId, float fVal)
{
	m_vecGenes[(size_t)geneId] = fVal;
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
//	size_t iSwap = rand() % ((int)GENE::NUM_GENES - 1);
	bool bUse1 = frand() < 0.5;
	for (size_t i = 0; i < (size_t)GENE::NUM_GENES; i++)
	{
//		child.m_vecGenes[i] = i <= iSwap ? parent1.m_vecGenes[i] : parent2.m_vecGenes[i];
		child.m_vecGenes[i] = bUse1 ? parent1.m_vecGenes[i] : parent2.m_vecGenes[i];
		if (frand() < 0.2f) bUse1 = !bUse1;
	}

	while (frand() < fMutateProb)
		child.RandomizeOne();

	return child;
}

CString CAgentGenome::ToString(const CString &strSeparator)
{
	CString strOut;

	for (size_t i = 0; i < m_vecGenes.size(); i++)
	{
		CString strLine;
		strLine.Format(_T(" = %.3f "), Gene((GENE)(i)));
		strLine = GeneSpecs[i].szName + strLine;
		if (i < m_vecGenes.size() - 1)
			strLine += strSeparator;
		strOut += strLine;
	}

	return strOut;
}

