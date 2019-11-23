#pragma once

using Microsoft::WRL::ComPtr;
using namespace DirectX;

inline float frand()
{
	return (float)rand() / (float)RAND_MAX;
}

constexpr int MAX_AGENTS = 2048;
constexpr int MAX_DEAD_AGENTS = MAX_AGENTS >> 3;

// This class defines the course that the agents must 
// navigate, including walls and agent source-sinks. It 
// also handles the DirectX objects for doing the physics
// and drawing of the walls. 
class CAgentCourse
{
public:
	CAgentCourse(bool bVisualize);

	HRESULT Initialize(ComPtr<ID3D11Device>& pD3DDevice, const CString &strJsonFile);
	HRESULT LoadShaders();
	BOOL UpdateAgents(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables, float dt, float T);

	CString GetName() { return m_strName; }
	float GetCourseLength() { return m_fCourseLength; }
	UINT GetMaxAlive() { return m_iMaxLiveAgents; }

	void RenderWalls(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables);
	void RenderAgents(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables,
		const ComPtr<ID3D11ShaderResourceView>& pSRVParticleDraw, const ComPtr<ID3D11SamplerState>& pTextureSampler);

protected:
	typedef std::vector<XMFLOAT2> XMPOLYLINE;
	struct AGENT_SOURCE_SINK {
		AGENT_SOURCE_SINK() : vColor(0.0f, 0.0f, 0.0f) {}
		XMFLOAT3 vColor;
		XMPOLYLINE lineSource;
		XMPOLYLINE lineSink;
		XMFLOAT2 velStart;
		float randLimit;
		float lenSource;
	};

	struct WORLD_PHYSICS
	{
		WORLD_PHYSICS() : g_fParticleRadius(0.5f), g_fIdealSpeed(10.0f), wpiDummy0(0),
			g_iMaxAgents(MAX_AGENTS)
		{}
		float g_fParticleRadius;
		float g_fIdealSpeed;
		UINT g_iMaxAgents;
		UINT wpiDummy0;
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
		float Score;
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
//	struct AGENT_VERTEX
//	{
//		int Dummy;
//	};

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

	HRESULT InitializeHourglass();
	HRESULT InitializeAgentBuffers();
	HRESULT InitializeWallBuffers();

	void MakeSegmentVertices(std::vector<WALL_VERTEX> &vecVerts, std::vector<UINT> &vecInds, const std::vector<WALL_SEGMENT> &vecSegs, XMFLOAT3 color);

	void SpawnAgent(ComPtr<ID3D11DeviceContext>& pD3DContext, float T);
	void ComputeAgents(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables);
	XMFLOAT2 GetSpawnPoint(size_t& iIndex);

	bool m_bVisualize;
	CString m_strName;
	float m_fCourseLength;
	float m_fNextSpawn;
	float m_fSpawnRate;
	std::vector<XMPOLYLINE> m_vecWalls;
	std::vector<AGENT_SOURCE_SINK> m_vecAgentSS;
	UINT m_iWallSegments;
	UINT m_iWallVertices;
	UINT m_iWallIndices;

	WORLD_PHYSICS m_sWorldPhysics;

	UINT m_iMaxLiveAgents;

	ComPtr<ID3D11Device> m_pD3DDevice;

	// D3D stuff needed for simulation
	ComPtr<ID3D11Buffer> m_pCBWorldPhysics;
	ComPtr<ID3D11Buffer> m_pCBRender;

	ComPtr<ID3D11Buffer> m_pSBAgentData;
	ComPtr<ID3D11ShaderResourceView> m_pSRVAgentData;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVAgentData;

	ComPtr<ID3D11Buffer> m_pSBAgentDataNext;
	ComPtr<ID3D11ShaderResourceView> m_pSRVAgentDataNext;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVAgentDataNext;

	ComPtr<ID3D11Buffer> m_pSBDeadList;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVDeadList;

	ComPtr<ID3D11Buffer> m_pSBSpawnOutput;
	ComPtr<ID3D11Buffer> m_pSBCPUSpawnOutput;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVSpawnOutput;

	ComPtr<ID3D11Buffer> m_pSBFinalScores;
	ComPtr<ID3D11Buffer> m_pSBCPUScores;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVFinalScores;

	ComPtr<ID3D11Buffer> m_pSBWalls;
	ComPtr<ID3D11ShaderResourceView> m_pSRVWalls;

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
