#pragma once

#include "DrawTimer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

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
	BOOL OnResize();
	BOOL UpdateScene(float dt, float T);
	BOOL RenderScene();

	struct WORLD_PHYSICS
	{
		WORLD_PHYSICS() : g_fParticleRadius(1.0f), wpfDummy0(0.0f), wpfDummy1(0.0f), wpfDummy2(0.0f) {}
		float g_fParticleRadius;
		float wpfDummy0;
		float wpfDummy1;
		float wpfDummy2;
	};

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

	WORLD_PHYSICS m_sWorldPhysics;
	FRAME_VARIABLES m_sFrameVariables;

	ComPtr<ID3D11Device> m_pD3DDevice;
	ComPtr<ID3D11DeviceContext> m_pD3DContext;
	ComPtr<IDXGISwapChain> m_pSwapChain;
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	D3D11_VIEWPORT m_ScreenViewport;
};
