#pragma once

#include "Course.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class CRunStatistics; 
class CAgentGenome;

constexpr int MAX_AGENTS = 4096;
constexpr int MAX_DEAD_AGENTS = MAX_AGENTS >> 3;

// This class defines the course that the agents must 
// navigate, including walls and agent source-sinks. It 
// also handles the DirectX objects for doing the physics
// and drawing of the walls. 
class CAgentCourse
{
public:
	CAgentCourse(bool bVisualize, std::shared_ptr<CRunStatistics> pRunStats);

	HRESULT Initialize(ComPtr<ID3D11Device>& pD3DDevice, ComPtr<ID3D11DeviceContext>& pD3DContext, const CCourse &cCourse, const CAgentGenome &cGenome);
	BOOL UpdateAgents(const ComPtr<ID3D11Buffer>& pCBFrameVariables, float dt, float T);

	CString GetName() { return m_cCourse.m_strName; }
	float GetCourseLength() { return m_sWorldPhysics.g_fCourseLength; }
	UINT GetMaxAlive() { return m_nMaxLiveAgents; }
	UINT GetNumSpawned() { return m_nSpawned; }
	bool GetSpawnActive() { return m_bSpawnActive; }
	void SetSpawnActive(bool bSpawnActive) { m_bSpawnActive = bSpawnActive; }

	void RenderWalls(const ComPtr<ID3D11Buffer>& pCBFrameVariables);
	void RenderAgents(const ComPtr<ID3D11Buffer>& pCBFrameVariables, const ComPtr<ID3D11ShaderResourceView>& pSRVParticleDraw, 
		const ComPtr<ID3D11SamplerState>& pTextureSampler);

protected:
	struct WORLD_PHYSICS
	{
		WORLD_PHYSICS() : g_fCourseLength(100.0f), g_fParticleRadius(0.5f), g_fIdealSpeed(10.0f), g_fMaxAcceleration(50.0f), 
			g_iMaxAgents(MAX_AGENTS), g_iNumWalls(0), g_iNumSinks(0), wpiDummy0(0), 
			g_fRepulseDist(0.0f), g_fRepulseStrength(0.0f),	g_fWallRepulseDist(0.0f), g_fWallRepulseStrength(0.0f),
			g_fMinAlignDist(0.0f), g_fMaxAlignDist(0.0f), g_fAlignAtMin(0.0f), g_fAlignAtMax(0.0f), 
			g_fAlignAtRear(0.0f), g_fMinWallAlignDist(0.0f), g_fMaxWallAlignDist(0.0f), g_fWallAlignAtMin(0.0f), 
			g_fWallAlignAtMax(0.0f), g_fWallAlignAtRear(0.0f)
			, wpfDummy0(0.0f) , wpfDummy1(0.0f) //, wpfDummy2(0.0f)
		{}
		float g_fCourseLength;
		float g_fParticleRadius;
		float g_fIdealSpeed;
		float g_fMaxAcceleration;

		UINT g_iMaxAgents;
		UINT g_iNumWalls;
		UINT g_iNumSinks;
		UINT wpiDummy0;

		float g_fRepulseDist;
		float g_fRepulseStrength;
		float g_fWallRepulseDist;
		float g_fWallRepulseStrength;

		float g_fMinAlignDist;
		float g_fMaxAlignDist;
		float g_fAlignAtMin;
		float g_fAlignAtMax;

		float g_fAlignAtRear;
		float g_fMinWallAlignDist;
		float g_fMaxWallAlignDist;
		float g_fWallAlignAtMin;

		float g_fWallAlignAtMax;
		float g_fWallAlignAtRear;
		float wpfDummy0;
		float wpfDummy1;

//		float wpfDummy2;
	};

	struct RENDER_VARIABLES
	{
		XMFLOAT4 g_arrColors[4];
	};

	struct AGENT_DATA
	{
		XMFLOAT4 Position;
		XMFLOAT4 Velocity;
		float SpawnTime;
		float Lifetime;
		float lastCollision;
		UINT numAACollisions;
		UINT numAWCollisions;
		int offCourse;
		int Type;
	};

	struct SPAWN_AGENT
	{
		XMFLOAT2 Position;
		XMFLOAT2 Velocity;

		float SpawnTime;
		float Radius;
		int Type;
		UINT maxLiveAgents;

		UINT maxAgents;
		UINT iDummy0;
		UINT iDummy1;
		UINT iDummy2;
	};

	typedef int AGENT_VERTEX;

	struct WALL_SEGMENT
	{
		WALL_SEGMENT() : End1(0.0f, 0.0f), End2(0.0f, 0.0f) {}
		WALL_SEGMENT(XMFLOAT2 p1, XMFLOAT2 p2)	{
			// Align all segments in the same direction
			if (p2.x > p1.x || (p1.x == p2.y && p1.y > p2.y)) { End1 = p1; End2 = p2; }
			else { End1 = p2; End2 = p1; }
		}

		XMFLOAT2 End1;
		XMFLOAT2 End2;
	};

	struct WALL_VERTEX
	{
		XMFLOAT2 Position;
		XMFLOAT3 Color;
	};
	const XMFLOAT3 colorWall = XMFLOAT3(0.75f, 0.75f, 0.75f);

	HRESULT LoadShaders();
	HRESULT InitializeAgentBuffers();
	HRESULT InitializeWallBuffers();

	void MakeSegmentVertices(std::vector<WALL_VERTEX> &vecVerts, std::vector<UINT> &vecInds, const std::vector<WALL_SEGMENT> &vecSegs, XMFLOAT3 color);

	void SpawnAgent(ComPtr<ID3D11DeviceContext>& pD3DContext, float T);
	void ComputeAgents(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables);
	XMFLOAT2 GetSpawnPoint(size_t& iIndex);

	bool m_bVisualize;
	bool m_bSpawnActive;
	float m_fNextSpawn;
	float m_fSpawnRate;
	UINT m_iWallVertices;
	UINT m_iWallIndices;

	std::shared_ptr<CRunStatistics> m_pRunStats;
	CCourse m_cCourse;

	WORLD_PHYSICS m_sWorldPhysics;

	UINT m_nSpawned;
	UINT m_nMaxLiveAgents;
	UINT m_nFrames;

	ComPtr<ID3D11Device> m_pD3DDevice;
	ComPtr<ID3D11DeviceContext> m_pD3DContext;

	// D3D stuff needed for simulation
	ComPtr<ID3D11Buffer> m_pCBWorldPhysics;
	ComPtr<ID3D11Buffer> m_pCBRender;

	ComPtr<ID3D11Buffer> m_pSBWallsSinks;
	ComPtr<ID3D11ShaderResourceView> m_pSRVWallsSinks;

	ComPtr<ID3D11Buffer> m_pSBAgentData;
	ComPtr<ID3D11ShaderResourceView> m_pSRVAgentData;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVAgentData;

	ComPtr<ID3D11Buffer> m_pSBAgentDataNext;
	ComPtr<ID3D11ShaderResourceView> m_pSRVAgentDataNext;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVAgentDataNext;
	
	#define NUM_COMPUTE_OUT 8
	ComPtr<ID3D11Buffer> m_pSBComputeOutput;
	ComPtr<ID3D11Buffer> m_pSBCPUComputeOutput;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVComputeOutput;

	ComPtr<ID3D11Buffer> m_pSBFinalScores;
	ComPtr<ID3D11Buffer> m_pSBCPUScores;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVFinalScores;

	ComPtr<ID3D11Buffer> m_pCBSpawnAgent;
	ComPtr<ID3D11ComputeShader> m_pAgentCSSpawn;
	ComPtr<ID3D11ComputeShader> m_pAgentCSIterate;

	// D3D stuff only needed for visualization
	ComPtr<ID3D11Buffer> m_pVBAgentColors;
	ComPtr<ID3D11Buffer> m_pVBWalls;
	ComPtr<ID3D11Buffer> m_pIBWalls;
	ComPtr<ID3D11VertexShader> m_pAgentVS;
	ComPtr<ID3D11VertexShader> m_pWallVS;
	ComPtr<ID3D11InputLayout> m_pAgentIL;
	ComPtr<ID3D11InputLayout> m_pWallIL;
	ComPtr<ID3D11GeometryShader> m_pAgentGS;
	ComPtr<ID3D11PixelShader> m_pAgentPS;
	ComPtr<ID3D11PixelShader> m_pWallPS;
};
