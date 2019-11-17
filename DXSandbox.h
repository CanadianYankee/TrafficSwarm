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

	BOOL Initialize(HWND hWnd);
	void CleanUp();
	void Tick();
	void Pause();
	void Resume(LPRECT pNewSize = NULL);

protected:
	HRESULT InitDirect3D();
	BOOL OnResize();
	BOOL UpdateScene(float dt, float T);
	BOOL RenderScene();

protected:
	HWND m_hMyWindow;
	int m_iClientWidth;
	int m_iClientHeight;
	float m_fAspectRatio;
	bool m_bRunning;
	CDrawTimer m_Timer;

	ComPtr<ID3D11Device> m_pD3DDevice;
	ComPtr<ID3D11DeviceContext> m_pD3DContext;
	ComPtr<IDXGISwapChain> m_pSwapChain;
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	D3D11_VIEWPORT m_ScreenViewport;
};
