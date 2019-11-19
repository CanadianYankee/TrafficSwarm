#pragma once

#include "DrawTimer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class CAgentCourse;

//
// CDXSandbox does all of the DirectX initialization and rendering. 
//
class CDXSandbox
{
public:
	CDXSandbox();

	BOOL Initialize(CWnd *pWnd);
	void CleanUp();
	void Tick();
	void Pause();
	void Resume(LPRECT pNewSize = NULL);

protected:
	HRESULT InitDirect3D();
	HRESULT LoadShaders();
	HRESULT PrepareShaderConstants();
	BOOL OnResize();
	BOOL UpdateScene(float dt, float T);
	BOOL RenderScene();

	struct FRAME_VARIABLES
	{
		FRAME_VARIABLES() : g_fGlobalTime(0.0f), g_fElapsedTime(0.0f), fvfDummy0(0.0f), fvfDummy1(0.0f) {}

		XMFLOAT4X4 fv_ViewTransform;
		float g_fGlobalTime;
		float g_fElapsedTime;
		float fvfDummy0;
		float fvfDummy1;
	};

protected:
	CWnd *m_pMyWindow;
	int m_iClientWidth;
	int m_iClientHeight;
	float m_fAspectRatio;
	bool m_bRunning;
	CDrawTimer m_Timer;

	CAgentCourse* m_pAgentCourse;

	FRAME_VARIABLES m_sFrameVariables;

	ComPtr<ID3D11Device> m_pD3DDevice;
	ComPtr<ID3D11DeviceContext> m_pD3DContext;
	ComPtr<IDXGISwapChain> m_pSwapChain;
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	D3D11_VIEWPORT m_ScreenViewport;

	ComPtr<ID3D11VertexShader> m_pAgentVS;
	ComPtr<ID3D11InputLayout> m_pAgentIL;
	ComPtr<ID3D11GeometryShader> m_pAgentGS;
	ComPtr<ID3D11PixelShader> m_pAgentPS;

	ComPtr<ID3D11Buffer> m_pCBFrameVariables;

	ComPtr<ID3D11ShaderResourceView> m_pSRVParticleDraw;
	ComPtr<ID3D11SamplerState> m_pTextureSampler;
};
