#include "pch.h"
#include "DXUtils.h"
#include "DDSTextureLoader.h"

extern CString g_strBasePath;

HRESULT CDXUtils::LoadShader(ComPtr<ID3D11Device>& pD3DDevice, ShaderType type, const CString& strFileName, ComPtr<ID3D11ClassLinkage> pClassLinkage, ComPtr<ID3D11DeviceChild>* pShader, VS_INPUTLAYOUTSETUP* pILS)
{
	HRESULT hr = S_OK;

	CString strFullPath = g_strBasePath + strFileName;

	std::vector<char> buffer;
	hr = LoadBinaryFile(strFullPath, buffer);

	if (SUCCEEDED(hr))
	{
		switch (type)
		{
		case VertexShader:
		{
			ComPtr<ID3D11VertexShader> pVertexShader;
			hr = pD3DDevice->CreateVertexShader(buffer.data(), buffer.size(), pClassLinkage.Get(), &pVertexShader);
			if (SUCCEEDED(hr) && (pILS))
			{
				hr = pD3DDevice->CreateInputLayout(pILS->pInputDesc, pILS->NumElements,
					buffer.data(), buffer.size(), &pILS->pInputLayout);
			}
			if (SUCCEEDED(hr))
			{
				hr = pVertexShader.As<ID3D11DeviceChild>(pShader);
			}
			break;
		}

		case PixelShader:
		{
			ComPtr<ID3D11PixelShader> pPixelShader;
			hr = pD3DDevice->CreatePixelShader(buffer.data(), buffer.size(), pClassLinkage.Get(), &pPixelShader);
			if (SUCCEEDED(hr))
			{
				hr = pPixelShader.As<ID3D11DeviceChild>(pShader);
			}
			break;
		}

		case GeometryShader:
		{
			ComPtr<ID3D11GeometryShader> pGeometryShader;
			hr = pD3DDevice->CreateGeometryShader(buffer.data(), buffer.size(), pClassLinkage.Get(), &pGeometryShader);
			if (SUCCEEDED(hr))
			{
				hr = pGeometryShader.As<ID3D11DeviceChild>(pShader);
			}
			break;
		}

		case ComputeShader:
		{
			ComPtr<ID3D11ComputeShader> pComputeShader;
			hr = pD3DDevice->CreateComputeShader(buffer.data(), buffer.size(), pClassLinkage.Get(), &pComputeShader);
			if (SUCCEEDED(hr))
			{
				hr = pComputeShader.As<ID3D11DeviceChild>(pShader);
			}
			break;
		}

		case HullShader:
		{
			ComPtr<ID3D11HullShader> pHullShader;
			hr = pD3DDevice->CreateHullShader(buffer.data(), buffer.size(), pClassLinkage.Get(), &pHullShader);
			if (SUCCEEDED(hr))
			{
				hr = pHullShader.As<ID3D11DeviceChild>(pShader);
			}
			break;
		}

		case DomainShader:
		{
			ComPtr<ID3D11DomainShader> pDomainShader;
			hr = pD3DDevice->CreateDomainShader(buffer.data(), buffer.size(), pClassLinkage.Get(), &pDomainShader);
			if (SUCCEEDED(hr))
			{
				hr = pDomainShader.As<ID3D11DeviceChild>(pShader);
			}
			break;
		}
		}
	}

	return hr;
}

HRESULT CDXUtils::LoadTexture(ComPtr<ID3D11Device>& pD3DDevice, const CString& strFileName, ComPtr<ID3D11Texture2D>* ppTexture, ComPtr<ID3D11ShaderResourceView>* ppSRVTexture)
{
	HRESULT hr = S_OK;

	CString strFullPath = g_strBasePath + strFileName;

	std::vector<char> buffer;
	hr = LoadBinaryFile(strFullPath, buffer);

	if (ppTexture) *ppTexture = nullptr;
	if (ppSRVTexture) *ppSRVTexture = nullptr;
	ComPtr<ID3D11Resource> pResource;
	ComPtr<ID3D11ShaderResourceView> pSRV;
	uint8_t* pData = reinterpret_cast<uint8_t*>(buffer.data());
	hr = CreateDDSTextureFromMemory(pD3DDevice.Get(), pData, buffer.size(), &pResource, &pSRV);
	if (SUCCEEDED(hr))
	{
		if (ppSRVTexture)
			pSRV.As<ID3D11ShaderResourceView>(ppSRVTexture);
		if (ppTexture)
			pResource.As<ID3D11Texture2D>(ppTexture);
	}

	return hr;
}

HRESULT CDXUtils::LoadBinaryFile(const CString& strPath, std::vector<char>& buffer)
{
	HRESULT hr = S_OK;

	std::ifstream fin(strPath, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	if (size > 0)
	{
		fin.seekg(0, std::ios_base::beg);
		buffer.resize(size);
		fin.read(buffer.data(), size);
	}
	else
	{
		hr = E_FAIL;
	}
	fin.close();

	return hr;
}
