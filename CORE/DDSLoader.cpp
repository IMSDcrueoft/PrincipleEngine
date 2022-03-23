#pragma once
#include "DDSLoader.h"
#include "D3DMethods.h"
using namespace Microsoft::WRL;
using namespace DirectX;

HRESULT DirectX::CreateDDSTextureFromFile12(_In_ ID3D12Device* device,
	_In_ ID3D12GraphicsCommandList* cmdList,
	_In_z_ const wchar_t* szFileName,
	_Out_ ComPtr<ID3D12Resource>& texture,
	_Out_ ComPtr<ID3D12Resource>& textureUploadHeap,
	_In_ size_t maxsize,
	_Out_opt_ DDS_ALPHA_MODE* alphaMode)
{
	HRESULT hr = S_OK;

	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;

	hr = DirectX::LoadDDSTextureFromFile(device,
		szFileName,
		texture.ReleaseAndGetAddressOf(),
		ddsData,
		subresources);

	if (FAILED(hr))
	{
		return hr;
	}

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.Get(),
		0,
		static_cast<UINT>(subresources.size()));

	// Create the GPU upload buffer.
	hr = device->CreateCommittedResource(
			GetTypePointer(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)),
			D3D12_HEAP_FLAG_NONE,
			GetTypePointer(CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureUploadHeap));

	if (FAILED(hr))
	{
		return hr;
	}

	UpdateSubresources(cmdList,
		texture.Get(),
		textureUploadHeap.Get(),
		0,
		0,
		static_cast<UINT>(subresources.size()), subresources.data());

	return hr;
}
