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

		WallRepulseDist,
		WallRepulseStrength,

		MinAlignDist,
		MaxAlignDist,
		AlignAtMin,
		AlignAtMax,
		AlignAtRear,

		WallAlignDist,
		WallAlign,
		WallAlignAtRear,

		NUM_GENES
	};

	struct GENE_SPECIFICATION
	{
		const TCHAR* szName;
		float fMin;
		float fMax;
	};

	void MakeDefault();
	float Gene(GENE geneId) const;
	void RandomizeAll();
	GENE RandomizeOne();
	void RandomizeOne(GENE geneId);

protected:
	std::vector<float> m_vecGenes;
};

