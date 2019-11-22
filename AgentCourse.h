#pragma once

using Microsoft::WRL::ComPtr;
using namespace DirectX;

inline float frand()
{
	return (float)rand() / (float)RAND_MAX;
}

constexpr int MAX_AGENTS = 4;

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

	typedef std::vector<XMFLOAT2> XMPOLYLINE;
	struct AGENT_SOURCE_SINK {
		AGENT_SOURCE_SINK() : vColor(0.0f, 0.0f, 0.0f) {}
		XMFLOAT3 vColor;
		XMPOLYLINE lineSource;
		XMPOLYLINE lineSink;
	};
	CString GetName() { return m_strName; }
	float GetCourseLength() { return m_fCourseLength; }
	int GetAgentCount() { return MAX_AGENTS; }

	int GetSpawnCount() { return (int)m_vecAgentSS.size(); }
	XMFLOAT3 GetColor(int iIndex) { return m_vecAgentSS[iIndex].vColor; }
	XMFLOAT2 GetSpawnPoint(int iIndex);

	void RenderWalls(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables);
	void RenderAgents(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables,
		const ComPtr<ID3D11ShaderResourceView>& pSRVParticleDraw, const ComPtr<ID3D11SamplerState>& pTextureSampler);

protected:
	struct WORLD_PHYSICS
	{
		WORLD_PHYSICS() : g_fParticleRadius(0.5f), wpfDummy0(0.0f), wpfDummy1(0.0f), wpfDummy2(0.0f) {}
		float g_fParticleRadius;
		float wpfDummy0;
		float wpfDummy1;
		float wpfDummy2;
	};

	struct AGENT_POSVEL
	{
		XMFLOAT4 Position;
		XMFLOAT4 Velocity;
	};

	struct AGENT_VERTEX
	{
		XMFLOAT3 Color;
	};

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

	bool m_bVisualize;
	CString m_strName;
	float m_fCourseLength;
	std::vector<XMPOLYLINE> m_vecWalls;
	std::vector<AGENT_SOURCE_SINK> m_vecAgentSS;
	UINT m_iWallSegments;
	UINT m_iWallVertices;
	UINT m_iWallIndices;

	WORLD_PHYSICS m_sWorldPhysics;

	ComPtr<ID3D11Device> m_pD3DDevice;

	// D3D stuff needed for simulation
	ComPtr<ID3D11Buffer> m_pCBWorldPhysics;
	ComPtr<ID3D11Buffer> m_pSBPosVel;
	ComPtr<ID3D11Buffer> m_pSBPosVelNext;
	ComPtr<ID3D11Buffer> m_pSBWalls;
	ComPtr<ID3D11ShaderResourceView> m_pSRVPosVel;
	ComPtr<ID3D11ShaderResourceView> m_pSRVPosVelNext;
	ComPtr<ID3D11ShaderResourceView> m_pSRVWalls;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVPosVel;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVPosVelNext;

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
