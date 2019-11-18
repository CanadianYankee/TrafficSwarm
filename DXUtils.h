#pragma once

using Microsoft::WRL::ComPtr;
using namespace DirectX;

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

	static HRESULT LoadShader(ComPtr<ID3D11Device>& pD3DDevice, ShaderType type, const CString& strFileName, ComPtr<ID3D11ClassLinkage> pClassLinkage, ComPtr<ID3D11DeviceChild>* pShader, VS_INPUTLAYOUTSETUP* pILS);
	static HRESULT LoadTexture(ComPtr<ID3D11Device>& pD3DDevice, const CString& strFileName, ComPtr<ID3D11Texture2D>* ppTexture, ComPtr<ID3D11ShaderResourceView>* ppSRVTexture);

private:
	static HRESULT LoadBinaryFile(const CString& strPath, std::vector<char>& buffer);
};

