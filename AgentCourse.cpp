#include "pch.h"
#include "AgentCourse.h"
#include "DXUtils.h"

CAgentCourse::CAgentCourse(bool bVisualize) :
	m_fCourseLength(0.0f),
	m_bVisualize(bVisualize)
{
}

HRESULT CAgentCourse::Initialize(ComPtr<ID3D11Device>& pD3DDevice, const CString& strJsonFile)
{
	m_pD3DDevice = pD3DDevice;
	HRESULT hr = S_OK;

	if (strJsonFile.IsEmpty())
	{
		hr = InitializeHourglass();
	}
	else
	{
		hr = E_NOTIMPL;
	}
	if (FAILED(hr)) return hr;
	
	// World physics is just set once and remains unchanged for the life of the saver
	D3D11_SUBRESOURCE_DATA cbData;
	cbData.pSysMem = &m_sWorldPhysics;
	cbData.SysMemPitch = 0;
	cbData.SysMemSlicePitch = 0;
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.ByteWidth = sizeof(WORLD_PHYSICS);
	hr = m_pD3DDevice->CreateBuffer(&Desc, &cbData, &m_pCBWorldPhysics);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pCBWorldPhysics, "WorldPhysics CB");

	hr = InitializeAgentBuffers();

	return hr;
}

HRESULT CAgentCourse::InitializeAgentBuffers()
{
	HRESULT hr = S_OK;
	D3D11_SUBRESOURCE_DATA vinitData = { 0 };

	// Initial position/velocity data
	std::vector<AGENT_POSVEL> vecPosVel;
	vecPosVel.resize(MAX_AGENTS);
	vecPosVel[0].Position = XMFLOAT4(25.0f, 25.0f, 0.0f, 0.0f);
	vecPosVel[1].Position = XMFLOAT4(25.0f, -25.0f, 0.0f, 0.0f);
	vecPosVel[2].Position = XMFLOAT4(75.0f, 25.0f, 0.0f, 0.0f);
	vecPosVel[3].Position = XMFLOAT4(75.0f, -25.0f, 0.0f, 0.0f);
	vecPosVel[0].Velocity = XMFLOAT4(-1.0f, 1.0f, 0.0f, 0.0f);
	vecPosVel[1].Velocity = XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.0f);
	vecPosVel[2].Velocity = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);
	vecPosVel[3].Velocity = XMFLOAT4(1.0f, -1.0f, 0.0f, 0.0f);

	// Create the position/velocity structured buffers for simulation
	CD3D11_BUFFER_DESC vbdPV(MAX_AGENTS * sizeof(AGENT_POSVEL),
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(AGENT_POSVEL));
	vinitData.pSysMem = &vecPosVel.front();
	hr = m_pD3DDevice->CreateBuffer(&vbdPV, &vinitData, &m_pSBPosVel);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSBPosVel, "Position/Velocity");
	hr = m_pD3DDevice->CreateBuffer(&vbdPV, &vinitData, &m_pSBPosVelNext);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSBPosVelNext, "Next Position/Velocity");

	// Create the position/velocity resource views (read-only in shaders)
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvPV(D3D11_SRV_DIMENSION_BUFFER, DXGI_FORMAT_UNKNOWN);
	srvPV.Buffer.FirstElement = 0;
	srvPV.Buffer.NumElements = MAX_AGENTS;
	hr = m_pD3DDevice->CreateShaderResourceView(m_pSBPosVel.Get(), &srvPV, &m_pSRVPosVel);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSRVPosVel, "Position/Velocity SRV");
	hr = m_pD3DDevice->CreateShaderResourceView(m_pSBPosVelNext.Get(), &srvPV, &m_pSRVPosVelNext);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSRVPosVelNext, "Next Position/Velocity SRV");

	// Create the position/velocity access view (write-access in shaders)
	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavPV(m_pSBPosVel.Get(), DXGI_FORMAT_UNKNOWN, 0, MAX_AGENTS);
	hr = m_pD3DDevice->CreateUnorderedAccessView(m_pSBPosVel.Get(), &uavPV, &m_pUAVPosVel);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pUAVPosVel, "Position/Velocity UAV");
	hr = m_pD3DDevice->CreateUnorderedAccessView(m_pSBPosVelNext.Get(), &uavPV, &m_pUAVPosVelNext);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pUAVPosVelNext, "Position/Velocity Next UAV");

	// If we're visualizing, then also need color data
	if (m_bVisualize)
	{
		std::vector<COLOR_VERTEX> vecColors;
		vecColors.resize(MAX_AGENTS);
		vecColors[0].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vecColors[1].Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		vecColors[2].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
		vecColors[3].Color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

		// Create color vertex buffer
		D3D11_SUBRESOURCE_DATA vinitData = { 0 };
		vinitData.pSysMem = &vecColors.front();
		vinitData.SysMemPitch = 0;
		vinitData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vbdColors(sizeof(COLOR_VERTEX) * MAX_AGENTS, D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE_IMMUTABLE);
		hr = m_pD3DDevice->CreateBuffer(&vbdColors, &vinitData, &m_pVBAgentColors);
		if (FAILED(hr)) return hr;
		D3DDEBUGNAME(m_pVBAgentColors, "Agent colors");
	}

	return hr;
}

void CAgentCourse::PrepareAgentRender(ComPtr<ID3D11DeviceContext>& pD3DContext)
{
	ASSERT(m_bVisualize);

	UINT stride = sizeof(COLOR_VERTEX);
	UINT offset = 0;
	pD3DContext->IASetVertexBuffers(0, 1, m_pVBAgentColors.GetAddressOf(), &stride, &offset);
	pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	pD3DContext->VSSetShaderResources(0, 1, m_pSRVPosVel.GetAddressOf());
}

HRESULT CAgentCourse::InitializeHourglass()
{
	m_strName = _T("Hourglass");
	m_fCourseLength = 100.0f;

	XMFLOAT2 point;
	m_vecWalls.resize(2);

	m_vecWalls[0].resize(3);
	point.x = 0.0f; point.y = -15.0f;
	m_vecWalls[0][0] = XMLoadFloat2(&point);
	point.x = 50.0f; point.y = -5.0f;
	m_vecWalls[0][1] = XMLoadFloat2(&point);
	point.x = 100.0f; point.y = -15.0f;
	m_vecWalls[0][2] = XMLoadFloat2(&point);

	m_vecWalls[1].resize(3);
	point.x = 0.0f; point.y = 15.0f;
	m_vecWalls[1][0] = XMLoadFloat2(&point);
	point.x = 50.0f; point.y = 5.0f;
	m_vecWalls[1][1] = XMLoadFloat2(&point);
	point.x = 100.0f; point.y = 15.0f;
	m_vecWalls[1][2] = XMLoadFloat2(&point);

	m_vecAgentSS.resize(1);
	XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vecAgentSS[0].vColor = XMLoadFloat4(&color);

	m_vecAgentSS[0].lineSource.resize(2);
	point.x = 0.0f; point.y = -13.5f;
	m_vecAgentSS[0].lineSource[0] = XMLoadFloat2(&point);
	point.x = 0.0f; point.y = 13.5f;
	m_vecAgentSS[0].lineSource[1] = XMLoadFloat2(&point);

	m_vecAgentSS[0].lineSink.resize(2);
	point.x = 100.0f; point.y = -15.0f;
	m_vecAgentSS[0].lineSink[0] = XMLoadFloat2(&point);
	point.x = 100.0f; point.y = 15.0f;
	m_vecAgentSS[0].lineSink[1] = XMLoadFloat2(&point);

	return S_OK;
}

XMVECTOR CAgentCourse::GetSpawnPoint(int iIndex)
{
	float f1 = frand();
	float f2 = 1.0f - f1;
	return f1 * m_vecAgentSS[iIndex].lineSource[0] + f2 * m_vecAgentSS[iIndex].lineSource[1];
}
