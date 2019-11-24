// ComputeTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "ComputeTest.h"
#include "..\DXUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Path to our app so we can locate resources
CString g_strBasePath;


// The one and only application object

CWinApp theApp;

using namespace std;

void DoComputeTest()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ComPtr<ID3D11Device> pD3DDevice;
	ComPtr<ID3D11DeviceContext> pD3DContext;

	D3D_FEATURE_LEVEL featureLevel;
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		createDeviceFlags, NULL, 0, D3D11_SDK_VERSION, &pD3DDevice, &featureLevel, &pD3DContext);

	if (SUCCEEDED(hr))
	{
		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
			hr = E_FAIL;
	}
	assert(SUCCEEDED(hr));

	ComPtr<ID3D11ComputeShader> pAppendCS;
	ComPtr<ID3D11DeviceChild> pShader;
	hr = CDXUtils::LoadShader(pD3DDevice, CDXUtils::ComputeShader, _T("AppendCS.cso"), nullptr, &pShader);
	if (SUCCEEDED(hr))
	{
		hr = pShader.As<ID3D11ComputeShader>(&pAppendCS);
		pShader = nullptr;
		D3DDEBUGNAME(pAppendCS, "Append CS");
	}
	assert(SUCCEEDED(hr));

	ComPtr<ID3D11ComputeShader> pConsumeCS;
	hr = CDXUtils::LoadShader(pD3DDevice, CDXUtils::ComputeShader, _T("ConsumeCS.cso"), nullptr, &pShader);
	if (SUCCEEDED(hr))
	{
		hr = pShader.As<ID3D11ComputeShader>(&pConsumeCS);
		D3DDEBUGNAME(pAppendCS, "Consume CS");
	}
	assert(SUCCEEDED(hr));

	ComPtr<ID3D11Buffer> pSBData;
	D3D11_SUBRESOURCE_DATA vinitData = { 0 };
	const UINT MAX_DATA = 1024;
	struct DATA {
		UINT iSource;
	};
	std::vector<DATA> vecData(MAX_DATA);
	DATA dataNull = { 0 };
	std::fill(vecData.begin(), vecData.end(), dataNull);
	CD3D11_BUFFER_DESC vbdDead(MAX_DATA * sizeof(DATA),
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(DATA));
	vinitData.pSysMem = &vecData.front();
	hr = pD3DDevice->CreateBuffer(&vbdDead, &vinitData, &pSBData);
	assert(SUCCEEDED(hr));
	D3DDEBUGNAME(pSBData, "Data List");

	ComPtr<ID3D11UnorderedAccessView> pUAVData;
	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDead(pSBData.Get(), DXGI_FORMAT_UNKNOWN, 0, MAX_DATA);
	hr = pD3DDevice->CreateUnorderedAccessView(pSBData.Get(), &uavDead, &pUAVData);
	assert(SUCCEEDED(hr));
	D3DDEBUGNAME(pUAVData, "Data UAV");

	ComPtr<ID3D11Buffer> pOutputData;
	CD3D11_BUFFER_DESC vbdOutput(MAX_DATA * sizeof(DATA),
		0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ, 0, sizeof(DATA));
	hr = pD3DDevice->CreateBuffer(&vbdOutput, NULL, &pOutputData);
	assert(SUCCEEDED(hr));
	D3DDEBUGNAME(pSBData, "Data Output");


	pD3DContext->CSSetShader(pAppendCS.Get(), NULL, 0);
	UINT count = 0;
	pD3DContext->CSSetUnorderedAccessViews(0, 1, pUAVData.GetAddressOf(), &count);

	pD3DContext->Dispatch(1, 1, 1);

	// Copy the output data
	pD3DContext->CopyResource(pOutputData.Get(), pSBData.Get());
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	pD3DContext->Map(pOutputData.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	DATA* pOutput = reinterpret_cast<DATA*>(mappedResource.pData);
	DATA arrOut[MAX_DATA];
	memcpy(arrOut, pOutput, MAX_DATA * sizeof(DATA));

	pD3DContext->Unmap(pOutputData.Get(), 0);

	for (int i = 0; i < 6; i++)
	{
		UINT d = arrOut[i].iSource;
	}
	// Unbind the resources
	ID3D11UnorderedAccessView* pUAVNULL[1] = { nullptr };
	pD3DContext->CSSetUnorderedAccessViews(0, 1, pUAVNULL, nullptr);
	pD3DContext->CSSetShader(NULL, NULL, 0);


	pD3DContext->CSSetShader(pConsumeCS.Get(), NULL, 0);
	count = 0;
	pD3DContext->CSSetUnorderedAccessViews(0, 1, pUAVData.GetAddressOf(), NULL);

	pD3DContext->Dispatch(1, 1, 1);

	// Copy the output data
	pD3DContext->CopyResource(pOutputData.Get(), pSBData.Get());
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	pD3DContext->Map(pOutputData.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	pOutput = reinterpret_cast<DATA*>(mappedResource.pData);
	memcpy(arrOut, pOutput, MAX_DATA * sizeof(DATA));

	pD3DContext->Unmap(pOutputData.Get(), 0);

	for (int i = 0; i < 6; i++)
	{
		UINT d = arrOut[i].iSource;
	}
	// Unbind the resources
	pD3DContext->CSSetUnorderedAccessViews(0, 1, pUAVNULL, nullptr);
	pD3DContext->CSSetShader(NULL, NULL, 0);
}

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

	TCHAR path[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	GetModuleFileName(hModule, path, _MAX_PATH);
	_tsplitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	_tmakepath_s(path, _MAX_PATH, drive, dir, NULL, NULL);
	g_strBasePath = path;
	
	if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
			DoComputeTest();
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
