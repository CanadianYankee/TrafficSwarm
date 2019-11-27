#pragma once

#include "AgentGenome.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class CAgentCourse;
class CRunStatistics;
class CCourse;

class CTrialRun
{
public:
	CTrialRun();
	~CTrialRun() { CleanUp(); }

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
	};

	HRESULT Intialize(UINT nAgents, CCourse *pCourse, const CAgentGenome& cGenome);
	BOOL Run(RUN_RESULTS &results);
	void CleanUp();

protected:
	HRESULT InitDirect3D();
	HRESULT PrepareShaderConstants();

	const float MAX_RUN_TIME = 2000.0f;

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

	CRunStatistics* m_pRunStats;
	CCourse* m_pCourse;
	CAgentCourse* m_pAgentCourse;
	CAgentGenome m_cGenome;
};

