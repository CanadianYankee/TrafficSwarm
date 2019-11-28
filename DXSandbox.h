#pragma once

#include "DrawTimer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class CAgentCourse;
class CRunStatistics;
class CCourse;
class CAgentGenome;

//
// CDXSandbox does DirectX initialization and rendering. 
//
class CDXSandbox
{
public:
	CDXSandbox(UINT nMaxRunAgents = 0);

	BOOL Initialize(CWnd *pWnd, std::shared_ptr<CCourse> pCourse, const CAgentGenome& cGenome);
	void Tick();
	void Pause();
	void Resume(LPRECT pNewSize = NULL);

protected:
	HRESULT InitDirect3D();
	HRESULT PrepareShaderConstants();
	BOOL OnResize();
	BOOL UpdateScene(float dt, float T);
	BOOL RenderScene();

	struct FRAME_VARIABLES
	{
		FRAME_VARIABLES() : g_fGlobalTime(0.0f), g_fElapsedTime(0.0f), g_iMaxAliveAgents(0), fviDummy0(0) {}

		XMFLOAT4X4 fv_ViewTransform;
		float g_fGlobalTime;
		float g_fElapsedTime;
		UINT g_iMaxAliveAgents;
		UINT fviDummy0;
	};

protected:
	CWnd *m_pMyWindow;
	int m_iClientWidth;
	int m_iClientHeight;
	float m_fAspectRatio;
	bool m_bRunning;
	CDrawTimer m_Timer;
	UINT m_nMaxRunAgents;

	std::shared_ptr<CAgentCourse> m_pAgentCourse;
	std::shared_ptr<CCourse> m_pCourse;
	std::shared_ptr<CRunStatistics> m_pRunStats;

	FRAME_VARIABLES m_sFrameVariables;

	ComPtr<ID3D11Device> m_pD3DDevice;
	ComPtr<ID3D11DeviceContext> m_pD3DContext;
	ComPtr<IDXGISwapChain> m_pSwapChain;
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	D3D11_VIEWPORT m_ScreenViewport;

	ComPtr<ID3D11Buffer> m_pCBFrameVariables;

	ComPtr<ID3D11ShaderResourceView> m_pSRVParticleDraw;
	ComPtr<ID3D11SamplerState> m_pTextureSampler;
	ComPtr<ID3D11BlendState> m_pRenderBlendState;
	ComPtr<ID3D11DepthStencilState> m_pRenderDepthState;
};
