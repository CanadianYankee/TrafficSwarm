#include "pch.h"
#include "DXSandbox.h"

CDXSandbox::CDXSandbox() :
	m_hMyWindow(NULL),
	m_iClientWidth(0),
	m_iClientHeight(0),
	m_fAspectRatio(1.0),
	m_bRunning(false)
{
	
}

BOOL CDXSandbox::Initialize(HWND hWnd)
{
	HRESULT hr = S_OK;
	BOOL bSuccess = TRUE;

	if (!hWnd)
	{
		assert(false);
		return FALSE;
	}

	RECT rcClientArea;
	bSuccess = ::GetClientRect(hWnd, &rcClientArea);
	if (!bSuccess || !rcClientArea.right || !rcClientArea.bottom)
		return FALSE;

	m_hMyWindow = hWnd;
	m_iClientWidth = rcClientArea.right;
	m_iClientHeight = rcClientArea.bottom;
	m_fAspectRatio = (float)m_iClientWidth / (float)m_iClientHeight;

	m_Timer.Reset();

	hr = InitDirect3D();

	bSuccess = SUCCEEDED(hr);
	if (bSuccess)
	{
		bSuccess = TRUE; // InitSaverData();  
		if (bSuccess)
			m_bRunning = true;
	}

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
	sd.OutputWindow = m_hMyWindow;
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

	if (SUCCEEDED(hr))
	{
		hr = OnResize() ? S_OK : E_FAIL;
	}

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
	return TRUE;
}

BOOL CDXSandbox::RenderScene()
{
	assert(m_pD3DContext && m_pSwapChain);

	float colorBlack[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float colorRandom[4] = { rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1.0f };

	m_pD3DContext->ClearRenderTargetView(m_pRenderTargetView.Get(), colorRandom);
	m_pD3DContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Cache initial state of drawing context
//	ComPtr<ID3D11BlendState> oldBlendState;
//	ComPtr<ID3D11DepthStencilState> oldDepthStencilState;
//	UINT oldSampleMask, oldStencilRef;
//	XMFLOAT4 oldBlendFactor;
//	m_pD3DContext->OMGetBlendState(&oldBlendState, &oldBlendFactor.x, &oldSampleMask);
//	m_pD3DContext->OMGetDepthStencilState(&oldDepthStencilState, &oldStencilRef);

	// Set the render shaders
//	m_pD3DContext->VSSetShader(m_pRenderVS.Get(), NULL, 0);
//	m_pD3DContext->GSSetShader(m_pRenderGS.Get(), NULL, 0);
//	m_pD3DContext->PSSetShader(m_pRenderPS.Get(), NULL, 0);

	// Set IA parameters
//	m_pD3DContext->IASetInputLayout(m_pRenderIL.Get());

//	UINT stride = sizeof(COLOR_VERTEX);
//	UINT offset = 0;
//	m_pD3DContext->IASetVertexBuffers(0, 1, m_pVBParticleColors.GetAddressOf(), &stride, &offset);
//	m_pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Set vertex shader resources
//	m_pD3DContext->VSSetShaderResources(0, 1, m_pSRVPosVel.GetAddressOf());

	// Set geometry shader resources (note that frame variables should have been updated in the compute function)
//	ID3D11Buffer* GSbuffers[2] = { m_pCBWorldPhysics.Get(), m_pCBFrameVariables.Get() };
//	m_pD3DContext->GSSetConstantBuffers(0, 2, GSbuffers);

	// Set pixel shader resources
//	m_pD3DContext->PSSetShaderResources(0, 1, m_pSRVParticleDraw.GetAddressOf());
//	m_pD3DContext->PSSetSamplers(0, 1, m_pTextureSampler.GetAddressOf());
//	m_pD3DContext->PSSetConstantBuffers(0, 1, m_pCBWorldPhysics.GetAddressOf());

	// Set OM parameters
//	m_pD3DContext->OMSetBlendState(m_pRenderBlendState.Get(), colorBlack, 0xFFFFFFFF);
//	m_pD3DContext->OMSetDepthStencilState(m_pRenderDepthState.Get(), 0);

	// Draw the particles
//	m_pD3DContext->Draw(m_ConfigData.m_iParticleCount, 0);

	// Restore the initial state
//	ID3D11ShaderResourceView* pSRVNULL[1] = { nullptr };
//	m_pD3DContext->VSSetShaderResources(0, 1, pSRVNULL);
//	m_pD3DContext->PSSetShaderResources(0, 1, pSRVNULL);
//	m_pD3DContext->GSSetShader(NULL, NULL, 0);
//	m_pD3DContext->OMSetBlendState(oldBlendState.Get(), &oldBlendFactor.x, oldSampleMask);
//	m_pD3DContext->OMSetDepthStencilState(oldDepthStencilState.Get(), oldStencilRef);

	HRESULT hr = m_pSwapChain->Present(1, 0);

	return SUCCEEDED(hr);
}

void CDXSandbox::CleanUp()
{
}