#include "pch.h"
#include "DXSandbox.h"
#include "DXUtils.h"
#include "AgentCourse.h"
#include "RunStatistics.h"
#include "AgentGenome.h"

CDXSandbox::CDXSandbox(UINT nMaxRunAgents) :
	m_pMyWindow(nullptr),
	m_iClientWidth(0),
	m_iClientHeight(0),
	m_fAspectRatio(1.0),
	m_bRunning(false),
	m_nMaxRunAgents(nMaxRunAgents)
{
}

BOOL CDXSandbox::Initialize(CWnd *pWnd, std::shared_ptr<CCourse> pCourse, const CAgentGenome &cGenome)
{
	HRESULT hr = S_OK;
	BOOL bSuccess = TRUE;

	if (!pWnd || !pCourse)
	{
		assert(false);
		return FALSE;
	}

	RECT rcClientArea;
	pWnd->GetClientRect(&rcClientArea);
	if (!rcClientArea.right || !rcClientArea.bottom)
		return FALSE;

	m_pMyWindow = pWnd;
	m_iClientWidth = rcClientArea.right;
	m_iClientHeight = rcClientArea.bottom;
	m_fAspectRatio = (float)m_iClientWidth / (float)m_iClientHeight;

	m_Timer.Reset();

	hr = InitDirect3D();
	bSuccess = SUCCEEDED(hr);
	if (!bSuccess) return bSuccess;

	m_pRunStats = std::make_shared<CRunStatistics>(m_nMaxRunAgents);
	m_pCourse = pCourse;
	m_pAgentCourse = std::make_shared<CAgentCourse>(true, m_pRunStats);
	hr = m_pAgentCourse->Initialize(m_pD3DDevice, m_pD3DContext, m_pCourse, cGenome);
	if (FAILED(hr))	return FALSE;

	hr = PrepareShaderConstants();
	if (FAILED(hr))	return FALSE;

	bSuccess = OnResize();
	if(bSuccess)
		m_bRunning = true;

	return bSuccess;
}

// Create D3D device, context, and swap chain
HRESULT CDXSandbox::InitDirect3D()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_iClientWidth;
	sd.BufferDesc.Height = m_iClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = m_pMyWindow->GetSafeHwnd();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		createDeviceFlags, NULL, 0, D3D11_SDK_VERSION, &sd, &m_pSwapChain,
		&m_pD3DDevice, &featureLevel, &m_pD3DContext);

	if (SUCCEEDED(hr))
	{
		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
			hr = E_FAIL;
	}
	assert(SUCCEEDED(hr));

	return hr;
}

HRESULT CDXSandbox::PrepareShaderConstants()
{
	HRESULT hr = S_OK;

	// Frame variables will be set for each render frame
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;
	Desc.ByteWidth = sizeof(FRAME_VARIABLES);
	hr = m_pD3DDevice->CreateBuffer(&Desc, NULL, &m_pCBFrameVariables);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pCBFrameVariables, "FrameVariables CB");

	// Load the particle drawing texture
	hr = CDXUtils::LoadTexture(m_pD3DDevice, _T("RoundAgent.dds"), nullptr, &m_pSRVParticleDraw);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pSRVParticleDraw, "ParticleDraw SRV");

	// Set up the texture sampler, blend state, and depth/stencil state
	D3D11_SAMPLER_DESC SamplerDesc;
	ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = m_pD3DDevice->CreateSamplerState(&SamplerDesc, &m_pTextureSampler);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pTextureSampler, "Texture Sampler");

	D3D11_BLEND_DESC BlendStateDesc;
	ZeroMemory(&BlendStateDesc, sizeof(BlendStateDesc));
	BlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	hr = m_pD3DDevice->CreateBlendState(&BlendStateDesc, &m_pRenderBlendState);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pTextureSampler, "Render Blend State");

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
	DepthStencilDesc.DepthEnable = FALSE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = m_pD3DDevice->CreateDepthStencilState(&DepthStencilDesc, &m_pRenderDepthState);
	if (FAILED(hr)) return hr;
	D3DDEBUGNAME(m_pTextureSampler, "Render Depth State");

	return hr;
}

void CDXSandbox::Pause()
{
	m_Timer.Stop();
	m_bRunning = false;
}

void CDXSandbox::Resume(LPRECT pNewSize /* = NULL*/)
{
	// Check for a change in size that requires a new viewport
	if (pNewSize)
	{
		int cx = pNewSize->right - pNewSize->left;
		int cy = pNewSize->bottom - pNewSize->top;

		ASSERT(cx > 0 && cy > 0);

		if (cx > 0 && cy > 0 && (cx != m_iClientWidth || cy != m_iClientHeight))
		{
			m_iClientWidth = cx;
			m_iClientHeight = cy;
			OnResize();
		}
	}
	
	m_Timer.Start();
	m_bRunning = true;
}

// Create/recreate the depth/stencil view and render target based on new size
BOOL CDXSandbox::OnResize()
{
	HRESULT hr = S_OK;

	RECT rcClientArea;
	m_pMyWindow->GetClientRect(&rcClientArea);
	m_iClientWidth = rcClientArea.right;
	m_iClientHeight = rcClientArea.bottom;
	m_fAspectRatio = (float)m_iClientWidth / (float)m_iClientHeight;

	if(m_pSwapChain)
	{
		// Release the old views, as they hold references to the buffers we
		// will be destroying.  Also release the old depth/stencil buffer.
		m_pRenderTargetView = nullptr;
		m_pDepthStencilView = nullptr;
		m_pDepthStencilBuffer = nullptr;

		// Resize the swap chain and recreate the render target view.
		hr = m_pSwapChain->ResizeBuffers(1, m_iClientWidth, m_iClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		if(SUCCEEDED(hr))
		{
			ComPtr<ID3D11Texture2D> pBackBuffer;
			hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
			if(SUCCEEDED(hr))
			{
				hr = m_pD3DDevice->CreateRenderTargetView(pBackBuffer.Get(), 0, &m_pRenderTargetView);
			}
		}

		if(SUCCEEDED(hr))
		{
			// Create the depth/stencil buffer and view.
			D3D11_TEXTURE2D_DESC depthStencilDesc;
	
			depthStencilDesc.Width     = m_iClientWidth;
			depthStencilDesc.Height    = m_iClientHeight;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.ArraySize = 1;
			depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilDesc.SampleDesc.Count   = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
			depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
			depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
			depthStencilDesc.CPUAccessFlags = 0; 
			depthStencilDesc.MiscFlags      = 0;

			hr = m_pD3DDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
			if(SUCCEEDED(hr))
			{
				hr = m_pD3DDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), 0, &m_pDepthStencilView);
			}
		}

		if(SUCCEEDED(hr))
		{
			// Bind the render target view and depth/stencil view to the pipeline.
			m_pD3DContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
	
			// Set the viewport transform.
			m_ScreenViewport.TopLeftX = 0;
			m_ScreenViewport.TopLeftY = 0;
			m_ScreenViewport.Width    = static_cast<float>(m_iClientWidth);
			m_ScreenViewport.Height   = static_cast<float>(m_iClientHeight);
			m_ScreenViewport.MinDepth = 0.0f;
			m_ScreenViewport.MaxDepth = 1.0f;

			m_pD3DContext->RSSetViewports(1, &m_ScreenViewport);
		}
	}

	float fLen = m_pAgentCourse->GetCourseLength() * 0.5f;
	XMVECTOR vCenter = XMLoadFloat2(fLen, 0.0f);
	XMVECTOR vScale = XMLoadFloat2(1.0f / fLen, 1.0f * m_fAspectRatio / fLen);
	XMMATRIX mat = XMMatrixTransformation2D(vCenter, 0.0f, vScale, vCenter, 0.0f, -vCenter);
	XMStoreFloat4x4(&(m_sFrameVariables.fv_ViewTransform), XMMatrixTranspose(mat));

	assert(SUCCEEDED(hr));

	return SUCCEEDED(hr);
}


void CDXSandbox::Tick()
{
	if (m_bRunning)
	{
		m_Timer.Tick();

		float dt = m_Timer.DeltaTime();
		float T = m_Timer.TotalTime();

		BOOL bResult = UpdateScene(dt, T);

		if (bResult)
		{
			bResult = RenderScene();
		}

		assert(bResult);
	}
	else
	{
	}
}

BOOL CDXSandbox::UpdateScene(float dt, float T)
{
	m_sFrameVariables.g_fElapsedTime = dt;
	m_sFrameVariables.g_fGlobalTime = T;
	m_sFrameVariables.g_iMaxAliveAgents = m_pAgentCourse->GetMaxAlive();

	HRESULT hr = CDXUtils::MapDataIntoBuffer(m_pD3DContext, &m_sFrameVariables, sizeof(m_sFrameVariables), m_pCBFrameVariables);
	if (FAILED(hr))
	{
		assert(false);
		return FALSE;
	}

	BOOL bSuccess = m_pAgentCourse->UpdateAgents(m_pCBFrameVariables, dt, T);

	if (m_nMaxRunAgents && m_pAgentCourse->GetNumSpawned() >= m_nMaxRunAgents)
	{
		m_pAgentCourse->SetSpawnActive(false);
		if (m_pAgentCourse->GetMaxAlive() == 0)
		{
			CTrialRun::RUN_RESULTS results;
			m_pRunStats->RecordRunResults(results);
			CString str;
			results.fSimulatedTime = T;
			results.strCourseName = m_pCourse->m_strName;
			str.Format(_T("Run of \"%s\": %d/%d complete;\nAvg Life = %f  Avg AA = %f  Avg AW = %f\nSimulated %f seconds at %f FPS.\n"),
				results.strCourseName.GetBuffer(), results.nComplete, results.nAgents, results.fAvgLifetime,
				results.fAvgAACollisions, results.fAvgAWCollisions, results.fSimulatedTime, results.fFPS);
			OutputDebugString(str);
			MessageBox(NULL, str, _T("Trial Results"), MB_OK);
			m_pMyWindow->PostMessage(WM_CLOSE);
		}
	}

	return bSuccess;
}

BOOL CDXSandbox::RenderScene()
{
	assert(m_pD3DContext && m_pSwapChain);

	float colorBlack[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	m_pD3DContext->ClearRenderTargetView(m_pRenderTargetView.Get(), colorBlack);
	m_pD3DContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Cache initial state of drawing context
	ComPtr<ID3D11BlendState> oldBlendState;
	ComPtr<ID3D11DepthStencilState> oldDepthStencilState;
	UINT oldSampleMask, oldStencilRef;
	XMFLOAT4 oldBlendFactor;
	m_pD3DContext->OMGetBlendState(&oldBlendState, &oldBlendFactor.x, &oldSampleMask);
	m_pD3DContext->OMGetDepthStencilState(&oldDepthStencilState, &oldStencilRef);

	// Render the walls before setting blending
	m_pAgentCourse->RenderWalls(m_pCBFrameVariables);

	// Set OM parameters
	m_pD3DContext->OMSetBlendState(m_pRenderBlendState.Get(), colorBlack, 0xFFFFFFFF);
	m_pD3DContext->OMSetDepthStencilState(m_pRenderDepthState.Get(), 0);

	// Render the agents
	m_pAgentCourse->RenderAgents(m_pCBFrameVariables, m_pSRVParticleDraw, m_pTextureSampler);

	// Restore the initial state
	m_pD3DContext->OMSetBlendState(oldBlendState.Get(), &oldBlendFactor.x, oldSampleMask);
	m_pD3DContext->OMSetDepthStencilState(oldDepthStencilState.Get(), oldStencilRef);

	HRESULT hr = m_pSwapChain->Present(1, 0);

	return SUCCEEDED(hr);
}
