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
	typedef std::vector<XMVECTOR> XMPOLYLINE;
	struct AGENT_SOURCE_SINK {
		XMVECTOR vColor;
		XMPOLYLINE lineSource;
		XMPOLYLINE lineSink;
	};
	CString GetName() { return m_strName; }
	float GetCourseLength() { return m_fCourseLength; }
	int GetAgentCount() { return MAX_AGENTS; }

	int GetSpawnCount() { return (int)m_vecAgentSS.size(); }
	XMVECTOR GetColor(int iIndex) { return m_vecAgentSS[iIndex].vColor; }
	XMVECTOR GetSpawnPoint(int iIndex);

	void PrepareAgentRender(ComPtr<ID3D11DeviceContext>& pD3DContext);
	ID3D11Buffer* GetCBWorldPhysicsPtr() { return m_pCBWorldPhysics.Get(); }
	ID3D11Buffer** GetCBWorldPhysicsAddress() { return m_pCBWorldPhysics.GetAddressOf(); }

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

	struct COLOR_VERTEX
	{
		XMFLOAT4 Color;
	};

	HRESULT InitializeHourglass();
	HRESULT InitializeAgentBuffers();

	bool m_bVisualize;
	CString m_strName;
	float m_fCourseLength;
	std::vector<XMPOLYLINE> m_vecWalls;
	std::vector<AGENT_SOURCE_SINK> m_vecAgentSS;

	WORLD_PHYSICS m_sWorldPhysics;

	ComPtr<ID3D11Device> m_pD3DDevice;

	// D3D stuff needed for simulation
	ComPtr<ID3D11Buffer> m_pCBWorldPhysics;
	ComPtr<ID3D11Buffer> m_pSBPosVel;
	ComPtr<ID3D11Buffer> m_pSBPosVelNext;
	ComPtr<ID3D11ShaderResourceView> m_pSRVPosVel;
	ComPtr<ID3D11ShaderResourceView> m_pSRVPosVelNext;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVPosVel;
	ComPtr<ID3D11UnorderedAccessView> m_pUAVPosVelNext;

	// D3D stuff only needed for visualization
	ComPtr<ID3D11Buffer> m_pVBAgentColors;

};

