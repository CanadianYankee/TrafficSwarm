#include "pch.h"
#include "AgentCourse.h"
#include "DXUtils.h"

CAgentCourse::CAgentCourse(bool bVisualize) :
	m_fCourseLength(0.0f),
	m_bVisualize(bVisualize),
	m_fNextSpawn(0.0f),
	m_fSpawnRate(2.0f),
	m_iWallSegments(0),
	m_iWallVertices(0),
	m_iWallIndices(0),
	m_iMaxLiveAgents(0)
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
	if (SUCCEEDED(hr))
	{
		hr = InitializeWallBuffers();
	}

	return hr;
}

HRESULT CAgentCourse::LoadShaders()
{
	ASSERT(m_pD3DDevice);

	HRESULT hr = S_OK;
	ComPtr<ID3D11DeviceChild> pShader;

	// Stuff needed for visualization
	if (m_bVisualize)
	{
		// Agent vertex shader
		const D3D11_INPUT_ELEMENT_DESC vertexAgentDesc[] =
		{
			{ "DUMMY",    0, DXGI_FORMAT_R32_SINT  , 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		CDXUtils::VS_INPUTLAYOUTSETUP ILS;
		ILS.pInputDesc = vertexAgentDesc;
		ILS.NumElements = ARRAYSIZE(vertexAgentDesc);
		ILS.pInputLayout = NULL;
		hr = CDXUtils::LoadShader(m_pD3DDevice, CDXUtils::VertexShader, _T("AgentVS.cso"), nullptr, &pShader, &ILS);
		if (SUCCEEDED(hr))
		{
			hr = pShader.As<ID3D11VertexShader>(&m_pAgentVS);
			m_pAgentIL = ILS.pInputLayout;
			ILS.pInputLayout->Release();
			D3DDEBUGNAME(m_pAgentVS, "Agent VS");
			D3DDEBUGNAME(m_pAgentIL, "Agent IL");
		}
		if (FAILED(hr)) return hr;

		// Wall vertex shader
		const D3D11_INPUT_ELEMENT_DESC vertexWallDesc[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(WALL_VERTEX, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(WALL_VERTEX, Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		ILS.pInputDesc = vertexWallDesc;
		ILS.NumElements = ARRAYSIZE(vertexWallDesc);
		ILS.pInputLayout = NULL;
		hr = CDXUtils::LoadShader(m_pD3DDevice, CDXUtils::VertexShader, _T("WallVS.cso"), nullptr, &pShader, &ILS);
		if (SUCCEEDED(hr))
		{
			hr = pShader.As<ID3D11VertexShader>(&m_pWallVS);
			m_pWallIL = ILS.pInputLayout;
			ILS.pInputLayout->Release();
			D3DDEBUGNAME(m_pWallVS, "Wall VS");
			D3DDEBUGNAME(m_pWallIL, "Wall IL");
		}
		if (FAILED(hr)) return hr;

		hr = CDXUtils::LoadShader(m_pD3DDevice, CDXUtils::GeometryShader, _T("AgentGS.cso"), nullptr, &pShader);
		if (SUCCEEDED(hr))
		{
			hr = pShader.As<ID3D11GeometryShader>(&m_pAgentGS);
			D3DDEBUGNAME(m_pAgentGS, "Agent GS");
		}
		if (FAILED(hr)) return hr;

		hr = CDXUtils::LoadShader(m_pD3DDevice, CDXUtils::PixelShader, _T("AgentPS.cso"), nullptr, &pShader);
		if (SUCCEEDED(hr))
		{
			hr = pShader.As<ID3D11PixelShader>(&m_pAgentPS);
			D3DDEBUGNAME(m_pAgentPS, "Agent PS");
		}
		if (FAILED(hr)) return hr;

		hr = CDXUtils::LoadShader(m_pD3DDevice, CDXUtils::PixelShader, _T("WallPS.cso"), nullptr, &pShader);
		if (SUCCEEDED(hr))
		{
			hr = pShader.As<ID3D11PixelShader>(&m_pWallPS);
			D3DDEBUGNAME(m_pAgentPS, "Wall PS");
		}
		if (FAILED(hr)) return hr;
	}
		
	hr = CDXUtils::LoadShader(m_pD3DDevice, CDXUtils::ComputeShader, _T("AgentCSSpawn.cso"), nullptr, &pShader);
	if (SUCCEEDED(hr))
	{
		hr = pShader.As<ID3D11ComputeShader>(&m_pAgentCSSpawn);
		D3DDEBUGNAME(m_pAgentCSSpawn, "Agent Spawn CS");
	}
	if (FAILED(hr)) return hr;

	hr = CDXUtils::LoadShader(m_pD3DDevice, CDXUtils::ComputeShader, _T("AgentCSIterate.cso"), nullptr, &pShader);
	if (SUCCEEDED(hr))
	{
		hr = pShader.As<ID3D11ComputeShader>(&m_pAgentCSIterate);
		D3DDEBUGNAME(m_pAgentCSIterate, "Agent Iterate CS");
	}
	if (FAILED(hr)) return hr;

	return hr;
}

HRESULT CAgentCourse::InitializeAgentBuffers()
{
	HRESULT hr = S_OK;
	D3D11_SUBRESOURCE_DATA vinitData = { 0 };

	m_iMaxLiveAgents = 0;
	std::vector<AGENT_DATA> vecAgent(MAX_AGENTS);

	// Create the agent data structured buffers for simulation; no data yet
	CD3D11_BUFFER_DESC vbdPV(MAX_AGENTS * sizeof(AGENT_DATA),
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(AGENT_DATA));
	vinitData.pSysMem = &vecAgent.front();
	hr = m_pD3DDevice->CreateBuffer(&vbdPV, &vinitData, &m_pSBAgentData);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSBAgentData, "Agent Data");
	hr = m_pD3DDevice->CreateBuffer(&vbdPV, nullptr, &m_pSBAgentDataNext);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSBAgentDataNext, "Next Agent Data");

	// Create the agent data resource views (read-only in shaders)
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvPV(D3D11_SRV_DIMENSION_BUFFER, DXGI_FORMAT_UNKNOWN);
	srvPV.Buffer.FirstElement = 0;
	srvPV.Buffer.NumElements = MAX_AGENTS;
	hr = m_pD3DDevice->CreateShaderResourceView(m_pSBAgentData.Get(), &srvPV, &m_pSRVAgentData);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSRVAgentData, "Position/Velocity SRV");
	hr = m_pD3DDevice->CreateShaderResourceView(m_pSBAgentDataNext.Get(), &srvPV, &m_pSRVAgentDataNext);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSRVAgentDataNext, "Next Position/Velocity SRV");

	// Create the agent data access view (write-access in shaders)
	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavPV(m_pSBAgentData.Get(), DXGI_FORMAT_UNKNOWN, 0, MAX_AGENTS);
	hr = m_pD3DDevice->CreateUnorderedAccessView(m_pSBAgentData.Get(), &uavPV, &m_pUAVAgentData);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pUAVAgentData, "Position/Velocity UAV");
	hr = m_pD3DDevice->CreateUnorderedAccessView(m_pSBAgentDataNext.Get(), &uavPV, &m_pUAVAgentDataNext);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pUAVAgentDataNext, "Position/Velocity Next UAV");

	// Create the dead agent index list and an unordered access view
	std::vector<DEAD_AGENT> vecDead(MAX_DEAD_AGENTS);
	DEAD_AGENT agentNull = { 0, 0.0f };
	std::fill(vecDead.begin(), vecDead.end(), agentNull);
	CD3D11_BUFFER_DESC vbdDead(MAX_DEAD_AGENTS * sizeof(DEAD_AGENT),
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(DEAD_AGENT));
	vinitData.pSysMem = &vecDead.front();
	hr = m_pD3DDevice->CreateBuffer(&vbdDead, &vinitData, &m_pSBDeadList);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSBDeadList, "Dead Agent List");

	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDead(m_pSBDeadList.Get(), DXGI_FORMAT_UNKNOWN, 0, 
		MAX_DEAD_AGENTS, D3D11_BUFFER_UAV_FLAG_COUNTER);
	hr = m_pD3DDevice->CreateUnorderedAccessView(m_pSBDeadList.Get(), &uavDead, &m_pUAVDeadList);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pUAVDeadList, "Dead Agent UAV");

	// Spawn variables will be set for each new agent
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;
	Desc.ByteWidth = sizeof(SPAWN_AGENT);
	hr = m_pD3DDevice->CreateBuffer(&Desc, NULL, &m_pCBSpawnAgent);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pCBSpawnAgent, "Spawn Agent CB");

	// If we're visualizing, then also need rendering data
	if (m_bVisualize)
	{
		RENDER_VARIABLES rVar;
		for (auto i = 0; i < m_vecAgentSS.size(); i++)
		{

			XMFLOAT3 clr = m_vecAgentSS[i].vColor;
			rVar.g_arrColors[i] = XMFLOAT4(clr.x, clr.y, clr.z, 1.0f);
		}

		D3D11_SUBRESOURCE_DATA cbData;
		cbData.pSysMem = &rVar;
		cbData.SysMemPitch = 0;
		cbData.SysMemSlicePitch = 0;
		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_IMMUTABLE;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = 0;
		Desc.MiscFlags = 0;
		Desc.ByteWidth = sizeof(RENDER_VARIABLES);
		hr = m_pD3DDevice->CreateBuffer(&Desc, &cbData, &m_pCBRender);
		if (FAILED(hr)) return hr;
		D3DDEBUGNAME(m_pCBWorldPhysics, "Render CB");

		// Create type vertex buffer
		std::vector<AGENT_VERTEX> vecAgent(MAX_AGENTS);
		std::fill(vecAgent.begin(), vecAgent.end(), 0);
		D3D11_SUBRESOURCE_DATA vinitData = { 0 };
		vinitData.pSysMem = &vecAgent.front();
		vinitData.SysMemPitch = 0;
		vinitData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vbdColors(sizeof(AGENT_VERTEX) * MAX_AGENTS, D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE_IMMUTABLE);
		hr = m_pD3DDevice->CreateBuffer(&vbdColors, &vinitData, &m_pVBAgentColors);
		if (FAILED(hr)) return hr;
		D3DDEBUGNAME(m_pVBAgentColors, "Agent vertices");
	}

	return hr;
}

HRESULT CAgentCourse::InitializeWallBuffers()
{
	HRESULT hr = S_OK;
	D3D11_SUBRESOURCE_DATA vinitData = { 0 };

	std::vector<WALL_SEGMENT> vecSegments;
	for (size_t i = 0; i < m_vecWalls.size(); i++)
	{
		for (size_t j = 0; j < m_vecWalls[i].size() - 1; j++)
		{
			vecSegments.push_back(WALL_SEGMENT(m_vecWalls[i][j], m_vecWalls[i][j + 1]));
		}
	}

	m_iWallSegments = (UINT)vecSegments.size();

	// Create the wall segment buffers for simulation
	CD3D11_BUFFER_DESC vbdW(m_iWallSegments * sizeof(WALL_SEGMENT),
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(WALL_SEGMENT));
	vinitData.pSysMem = &vecSegments.front();
	hr = m_pD3DDevice->CreateBuffer(&vbdW, &vinitData, &m_pSBWalls);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSBWalls, "Wall Segments");

	// Create the resource view (read-only in shaders)
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvW(D3D11_SRV_DIMENSION_BUFFER, DXGI_FORMAT_UNKNOWN);
	srvW.Buffer.FirstElement = 0;
	srvW.Buffer.NumElements = m_iWallSegments;
	hr = m_pD3DDevice->CreateShaderResourceView(m_pSBWalls.Get(), &srvW, &m_pSRVWalls);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSRVWalls, "Wall Segment SRV");

	// If we're visualizing, then also need rendering data
	if (m_bVisualize)
	{
		std::vector<WALL_VERTEX> vecWVerts;
		std::vector<UINT> vecWInds;
		MakeSegmentVertices(vecWVerts, vecWInds, vecSegments, colorWall);

		// Draw sources and sinks
		for (size_t i = 0; i < m_vecAgentSS.size(); i++)
		{
			XMFLOAT3 color = m_vecAgentSS[i].vColor;
			color.x *= 0.5; color.y *= 0.5; color.z *= 0.5;
			std::vector<WALL_SEGMENT> vecSegs(2);
			vecSegs[0] = WALL_SEGMENT(m_vecAgentSS[i].lineSource[0], m_vecAgentSS[i].lineSource[1]);
			vecSegs[1] = WALL_SEGMENT(m_vecAgentSS[i].lineSink[0], m_vecAgentSS[i].lineSink[1]);
			MakeSegmentVertices(vecWVerts, vecWInds, vecSegs, color);
		}

		m_iWallVertices = (UINT)vecWVerts.size();
		m_iWallIndices = (UINT)vecWInds.size();

		// Create vertex buffer for walls
		CD3D11_BUFFER_DESC vbd(m_iWallVertices * sizeof(WALL_VERTEX), D3D11_BIND_VERTEX_BUFFER);
		ZeroMemory(&vinitData, sizeof(D3D11_SUBRESOURCE_DATA));
		vinitData.pSysMem = (PVOID)vecWVerts.data();
		hr = m_pD3DDevice->CreateBuffer(&vbd, &vinitData, &m_pVBWalls);
		if (FAILED(hr)) return hr;
		D3DDEBUGNAME(m_pVBWalls, "Walls Vertex Buffer");

		// Create index buffer for walls
		CD3D11_BUFFER_DESC ibd(m_iWallIndices * sizeof(UINT), D3D10_BIND_INDEX_BUFFER);
		ZeroMemory(&vinitData, sizeof(D3D11_SUBRESOURCE_DATA));
		vinitData.pSysMem = (PVOID)vecWInds.data();
		hr = m_pD3DDevice->CreateBuffer(&ibd, &vinitData, &m_pIBWalls);
		if (FAILED(hr)) return hr;
		D3DDEBUGNAME(m_pIBWalls, "Walls Index Buffer");
	}

	return hr;
}

void CAgentCourse::MakeSegmentVertices(std::vector<WALL_VERTEX>& vecVerts, std::vector<UINT>& vecInds, const std::vector<WALL_SEGMENT> &vecSegs, XMFLOAT3 color)
{
	size_t nSegs = vecSegs.size();
	const size_t strideVert = 4;
	const size_t strideInd = 6;
	size_t offsetVert = vecVerts.size();
	std::vector<WALL_VERTEX> newVerts(nSegs * strideVert);
	std::vector<UINT> newInds(nSegs * strideInd);

	for (size_t i = 0; i < nSegs; i++)
	{
		XMVECTOR v = XMLoadFloat2(vecSegs[i].End2.x - vecSegs[i].End1.x, vecSegs[i].End2.y - vecSegs[i].End1.y);
		v = XMVector2Normalize(v);
		XMVECTOR voffset = XMVector2Orthogonal(v) * m_sWorldPhysics.g_fParticleRadius;
		v = v * m_sWorldPhysics.g_fParticleRadius;
		XMVECTOR e1 = XMLoadFloat2(&vecSegs[i].End1) - v;
		XMVECTOR e2 = XMLoadFloat2(&vecSegs[i].End2) + v;

		newVerts[i * strideVert]     = { XMUnloadFloat2(e1 + voffset), color };
		newVerts[i * strideVert + 1] = { XMUnloadFloat2(e1 - voffset), color };
		newVerts[i * strideVert + 2] = { XMUnloadFloat2(e2 + voffset), color };
		newVerts[i * strideVert + 3] = { XMUnloadFloat2(e2 - voffset), color };

		newInds[i * strideInd]     = (UINT)(offsetVert + i * strideVert);
		newInds[i * strideInd + 1] = (UINT)(offsetVert + i * strideVert + 2);
		newInds[i * strideInd + 2] = (UINT)(offsetVert + i * strideVert + 1);
		newInds[i * strideInd + 3] = (UINT)(offsetVert + i * strideVert + 2);
		newInds[i * strideInd + 4] = (UINT)(offsetVert + i * strideVert + 3);
		newInds[i * strideInd + 5] = (UINT)(offsetVert + i * strideVert + 1);
	}

	vecVerts.insert(vecVerts.end(), newVerts.begin(), newVerts.end());
	vecInds.insert(vecInds.end(), newInds.begin(), newInds.end());
}

BOOL CAgentCourse::UpdateAgents(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables, float dt, float T)
{
	// Let the agents do their physics thing
	ComputeAgents(pD3DContext, pCBFrameVariables);

	// If total time exceeds the next scheduled spawn time, then 
	// spawn a new agent.
	if (T >= m_fNextSpawn && m_iMaxLiveAgents < MAX_AGENTS)
	{
		SpawnAgent(pD3DContext, T);
		m_fNextSpawn = T - logf(1.0f - frand()) / m_fSpawnRate;
		m_iMaxLiveAgents++;
	}

	return TRUE;
}

void CAgentCourse::SpawnAgent(ComPtr<ID3D11DeviceContext>& pD3DContext, float T)
{
	size_t iIndex;
	XMFLOAT2 ptSpawn = GetSpawnPoint(iIndex);
	XMFLOAT2 ptVelocity = XMFLOAT2(m_sWorldPhysics.g_fIdealSpeed, 0.0f);
	ID3D11UnorderedAccessView* pUAVNULL[1] = { nullptr };

	SPAWN_AGENT agent = { ptSpawn, m_vecAgentSS[iIndex].velStart, T, (int)iIndex, m_iMaxLiveAgents, MAX_AGENTS };

	// Select the compute shader that spawns new agents
	pD3DContext->CSSetShader(m_pAgentCSSpawn.Get(), NULL, 0);

	// Map new agent information into spawn compute shader
	HRESULT hr = CDXUtils::MapDataIntoBuffer(pD3DContext, &agent, sizeof(agent), m_pCBSpawnAgent);
	pD3DContext->CSSetConstantBuffers(0, 1, m_pCBSpawnAgent.GetAddressOf());

	// Give the spawn compute shader read access to agent data array
	pD3DContext->CSSetUnorderedAccessViews(0, 1, m_pUAVAgentData.GetAddressOf(), nullptr);

	// Run the spawn compute shader
	pD3DContext->Dispatch(1, 1, 1);

	// Unbind the resources
	pD3DContext->CSSetUnorderedAccessViews(0, 1, pUAVNULL, nullptr);

	m_iMaxLiveAgents++;
}

void CAgentCourse::ComputeAgents(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables)
{
	ID3D11UnorderedAccessView* pUAVNULL[1] = { nullptr };
	ID3D11ShaderResourceView* pSRVNULL[1] = { nullptr };

	// Select the compute shader that iterates agents through time
	pD3DContext->CSSetShader(m_pAgentCSIterate.Get(), NULL, 0);

	// Set the constant buffers defining the world physics and the frame variables
	ID3D11Buffer* CSCBuffers[2] = { m_pCBWorldPhysics.Get(), pCBFrameVariables.Get() };
	pD3DContext->CSSetConstantBuffers(0, 2, CSCBuffers);

	// Input to the shader - current agent kinematics
	pD3DContext->CSSetShaderResources(0, 1, m_pSRVAgentData.GetAddressOf());

	// Output from the shader - updated particle kinematics
	pD3DContext->CSSetUnorderedAccessViews(0, 1, m_pUAVAgentDataNext.GetAddressOf(), nullptr);

	// Run the compute shader - acts on a 1D array of agents
	pD3DContext->Dispatch(32, 1, 1);

	// Unbind the resources
	pD3DContext->CSSetUnorderedAccessViews(0, 1, pUAVNULL, nullptr);
	pD3DContext->CSSetShaderResources(0, 1, pSRVNULL);

	// Swap the new kinematic results with the current ones for rendering
	SwapComPtr<ID3D11Buffer>(m_pSBAgentData, m_pSBAgentDataNext);
	SwapComPtr<ID3D11ShaderResourceView>(m_pSRVAgentData, m_pSRVAgentDataNext);
	SwapComPtr<ID3D11UnorderedAccessView>(m_pUAVAgentData, m_pUAVAgentDataNext);
}

void CAgentCourse::RenderWalls(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables)
{
	ASSERT(m_bVisualize);

	// Set the render shaders for the walls
	pD3DContext->VSSetShader(m_pWallVS.Get(), NULL, 0);
	pD3DContext->PSSetShader(m_pWallPS.Get(), NULL, 0);

	// Set IA parameters
	pD3DContext->IASetInputLayout(m_pWallIL.Get());
	UINT stride = sizeof(WALL_VERTEX);
	UINT offset = 0;
	pD3DContext->IASetVertexBuffers(0, 1, m_pVBWalls.GetAddressOf(), &stride, &offset);
	pD3DContext->IASetIndexBuffer(m_pIBWalls.Get(), DXGI_FORMAT_R32_UINT, 0);
	pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set vertex shader resources 
	ID3D11Buffer* VSbuffers[2] = { m_pCBWorldPhysics.Get(), pCBFrameVariables.Get() };
	pD3DContext->VSSetConstantBuffers(0, 2, VSbuffers);

	// Draw the walls
	pD3DContext->DrawIndexed(m_iWallIndices, 0, 0);

	// Clear out the shaders
	ID3D11ShaderResourceView* pSRVNULL[1] = { nullptr };
	pD3DContext->VSSetShaderResources(0, 1, pSRVNULL);
	pD3DContext->PSSetShaderResources(0, 1, pSRVNULL);
	pD3DContext->VSSetShader(NULL, NULL, 0);
	pD3DContext->PSSetShader(NULL, NULL, 0);
}

void CAgentCourse::RenderAgents(ComPtr<ID3D11DeviceContext>& pD3DContext, const ComPtr<ID3D11Buffer>& pCBFrameVariables, 
	const ComPtr<ID3D11ShaderResourceView>& pSRVParticleDraw, const ComPtr<ID3D11SamplerState>& pTextureSampler)
{
	ASSERT(m_bVisualize);

	// Set the render shaders for the agents
	pD3DContext->VSSetShader(m_pAgentVS.Get(), NULL, 0);
	pD3DContext->GSSetShader(m_pAgentGS.Get(), NULL, 0);
	pD3DContext->PSSetShader(m_pAgentPS.Get(), NULL, 0);

	// Set IA parameters
	pD3DContext->IASetInputLayout(m_pAgentIL.Get());
	UINT stride = sizeof(AGENT_VERTEX);
	UINT offset = 0;
	pD3DContext->IASetVertexBuffers(0, 1, m_pVBAgentColors.GetAddressOf(), &stride, &offset);
	pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	pD3DContext->VSSetConstantBuffers(0, 1, m_pCBRender.GetAddressOf());
	pD3DContext->VSSetShaderResources(0, 1, m_pSRVAgentData.GetAddressOf());

	// Set geometry shader resources 
	ID3D11Buffer* GSbuffers[2] = { m_pCBWorldPhysics.Get(), pCBFrameVariables.Get() };
	pD3DContext->GSSetConstantBuffers(0, 2, GSbuffers);

	// Set pixel shader resources
	pD3DContext->PSSetShaderResources(0, 1, pSRVParticleDraw.GetAddressOf());
	pD3DContext->PSSetSamplers(0, 1, pTextureSampler.GetAddressOf());
	pD3DContext->PSSetConstantBuffers(0, 1, m_pCBWorldPhysics.GetAddressOf());

	// Draw the agents
	pD3DContext->Draw(m_iMaxLiveAgents, 0);

	// Clear out the shaders
	ID3D11ShaderResourceView* pSRVNULL[1] = { nullptr };
	pD3DContext->VSSetShaderResources(0, 1, pSRVNULL);
	pD3DContext->PSSetShaderResources(0, 1, pSRVNULL);
	pD3DContext->VSSetShader(NULL, NULL, 0);
	pD3DContext->GSSetShader(NULL, NULL, 0);
	pD3DContext->PSSetShader(NULL, NULL, 0);
}

HRESULT CAgentCourse::InitializeHourglass()
{
	m_strName = _T("Hourglass");
	m_fCourseLength = 100.0f;

	m_vecWalls.resize(2);

	m_vecWalls[0].resize(3);
	m_vecWalls[0][0] = XMFLOAT2(0.0f, -15.0f);
	m_vecWalls[0][1] = XMFLOAT2(50.0f, -5.0f);
	m_vecWalls[0][2] = XMFLOAT2(100.0f, -15.0f);

	m_vecWalls[1].resize(3);
	m_vecWalls[1][0] = XMFLOAT2(0.0f, 15.0f);
	m_vecWalls[1][1] = XMFLOAT2(50.0f, 5.0f);
	m_vecWalls[1][2] = XMFLOAT2(100.0f, 15.0f);

	m_vecAgentSS.resize(1);
	m_vecAgentSS[0].vColor = XMFLOAT3(0.5f, 0.5f, 1.0f);
	m_vecAgentSS[0].randLimit = 1.0f;

	m_vecAgentSS[0].lineSource.resize(2);
	m_vecAgentSS[0].lineSource[0] = XMFLOAT2(0.0f, -13.5f);
	m_vecAgentSS[0].lineSource[1] = XMFLOAT2(0.0f, 13.5f);
	m_vecAgentSS[0].lenSource = m_vecAgentSS[0].lineSource[1].y - m_vecAgentSS[0].lineSource[0].y;

	m_vecAgentSS[0].lineSink.resize(2);
	m_vecAgentSS[0].lineSink[0] = XMFLOAT2(100.0f, -13.5f);
	m_vecAgentSS[0].lineSink[1] = XMFLOAT2(100.0f, 13.5f);

	m_vecAgentSS[0].velStart = XMFLOAT2(m_sWorldPhysics.g_fIdealSpeed, 0.0f);

	m_fSpawnRate *= m_vecAgentSS[0].lenSource;

	return S_OK;
}

XMFLOAT2 CAgentCourse::GetSpawnPoint(size_t &iIndex)
{
	iIndex = 0;
	if (m_vecAgentSS.size() > 1)
	{
		float fi = frand();
		for (size_t i = 0; i < m_vecAgentSS.size(); i++)
		{
			if (fi < m_vecAgentSS[i].randLimit)
			{
				iIndex = i;
				break;
			}
		}
	}

	float f1 = frand();
	float f2 = 1.0f - f1;
	return XMFLOAT2(
		f1 * m_vecAgentSS[iIndex].lineSource[0].x + f2 * m_vecAgentSS[iIndex].lineSource[1].x,
		f1 * m_vecAgentSS[iIndex].lineSource[0].y + f2 * m_vecAgentSS[iIndex].lineSource[1].y);
}
