#pragma once

#include "AgentGenome.h"
#include "Course.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class CAgentCourse;
class CRunStatistics;
class CCourse;

constexpr float AACollisionPenalty = 50.0f;
constexpr float AWCollisionPenalty = 20.0f;
constexpr float IncompletePenalty = 40.0f;
constexpr float SpawnFailPenalty = 0.0f; 
constexpr float LeftEscapePenalty = 5.0f;
constexpr float RightEscapePenalty = 5.0f;

class CTrialRun
{
public:
	CTrialRun();

	// The RUN_RESULTS structure holds all the results statistics for a trial run,  
	// including the full genome (parameter set) of the agents who performed the trial.
	// The structure also calculates the score of the trial and can be saved and loaded 
	// to disk for future breeding or graphical runs. 
	struct RUN_RESULTS
	{
		CString strCourseName;
		CAgentGenome genome;
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
		float Score() const {
			return fAvgLifetime + IncompletePenalty * (nAgents - nComplete) + AACollisionPenalty * fAvgAACollisions +
				AWCollisionPenalty * fAvgAWCollisions + SpawnFailPenalty * nSpawnFails + LeftEscapePenalty * nLeftEscapes + 
				RightEscapePenalty * nRightEscapes;
		}
		CString ToListString(const CString &strSeparator) const;
		CString ToParagraphString(const CString& strSeparator) const;
	};

	HRESULT Intialize(UINT nAgents, const CCourse &pCourse, const CAgentGenome& cGenome);
	BOOL Run(RUN_RESULTS &results);

protected:
	HRESULT InitDirect3D();
	HRESULT PrepareShaderConstants();

	struct FRAME_VARIABLES
	{
		FRAME_VARIABLES() : g_fGlobalTime(0.0f), g_fElapsedTime(0.0f), g_iMaxAliveAgents(0), fviDummy0(0) {}

		XMFLOAT4X4 fv_ViewTransform;  // Not needed if we're not rendering, but shaders have this memory reserved.

		float g_fGlobalTime;
		float g_fElapsedTime;
		UINT g_iMaxAliveAgents;
		UINT fviDummy0;
	};

	FRAME_VARIABLES m_sFrameVariables;

	ComPtr<ID3D11Device> m_pD3DDevice;
	ComPtr<ID3D11DeviceContext> m_pD3DContext;
	ComPtr<ID3D11Buffer> m_pCBFrameVariables;

	std::shared_ptr<CRunStatistics> m_pRunStats;
	std::shared_ptr<CAgentCourse> m_pAgentCourse;
	CCourse m_cCourse;
	CAgentGenome m_cGenome;
};

