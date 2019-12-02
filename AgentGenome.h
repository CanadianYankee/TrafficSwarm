#pragma once

inline float frand()
{
	return (float)rand() / (float)RAND_MAX;
}

class CAgentGenome
{
public:
	CAgentGenome();

	enum class GENE : UINT
	{
		RepulseDist,
		RepulseStrength,

		MinAlignDist,
		MaxAlignDist,
		AlignAtMin,
		AlignAtMax,
		AlignAtRear,

		CautionVelDist,
		CautionVelStrength,

		WallRepulseDist,
		WallRepulseStrength,

		MinWallAlignDist,
		MaxWallAlignDist,
		WallAlignAtMin,
		WallAlignAtMax,
		WallAlignAtRear,

		NUM_GENES
	};

	struct GENE_SPECIFICATION
	{
		const TCHAR* szName;
		float fMin;
		float fMax;
	};

	void ZeroAll();
	float Gene(GENE geneId) const;
	void SetGene(GENE geneId, float fVal);
	void RandomizeAll();
	GENE RandomizeOne();
	void RandomizeOne(GENE geneId);
	static CAgentGenome CrossBreed(const CAgentGenome& parent1, const CAgentGenome& parent2, float fMutateProb = 0.2f);
	CString ToString(const CString& strSeparator);
	bool operator==(const CAgentGenome& other) const { return m_vecGenes == other.m_vecGenes; }
	bool operator!=(const CAgentGenome& other) const { return m_vecGenes != other.m_vecGenes; }

protected:
	std::vector<float> m_vecGenes;
};
