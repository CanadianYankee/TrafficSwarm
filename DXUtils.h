#pragma once

using Microsoft::WRL::ComPtr;
using namespace DirectX;

#if (defined(DEBUG) || defined(_DEBUG))
#define D3DDEBUGNAME(pobj, name) pobj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name)
#else
#define D3DDEBUGNAME(pobj, name)
#endif 

class CDXUtils
{
public:
	enum ShaderType
	{
		VertexShader, PixelShader, GeometryShader, ComputeShader, HullShader, DomainShader
	};
	struct VS_INPUTLAYOUTSETUP
	{
		const D3D11_INPUT_ELEMENT_DESC* pInputDesc;
		UINT NumElements;
		ID3D11InputLayout* pInputLayout;
	};

	static HRESULT LoadShader(ComPtr<ID3D11Device>& pD3DDevice, ShaderType type, const CString& strFileName, ComPtr<ID3D11ClassLinkage> pClassLinkage, ComPtr<ID3D11DeviceChild>* pShader, VS_INPUTLAYOUTSETUP* pILS = NULL);
	static HRESULT LoadTexture(ComPtr<ID3D11Device>& pD3DDevice, const CString& strFileName, ComPtr<ID3D11Texture2D>* ppTexture, ComPtr<ID3D11ShaderResourceView>* ppSRVTexture);
	static HRESULT MapDataIntoBuffer(ComPtr<ID3D11DeviceContext>& pD3DDeviceContext, const void* pData, size_t nSize, ComPtr<ID3D11Resource> pResource, UINT Subresource = 0, D3D11_MAP MapType = D3D11_MAP_WRITE_DISCARD);

private:
	static HRESULT LoadBinaryFile(const CString& strPath, std::vector<char>& buffer);
};

