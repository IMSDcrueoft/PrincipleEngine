#ifdef _MSC_VER
#pragma once
#endif

#include <wrl.h>
#include "d3dx12.h"
#include "DDSTextureLoader12.h"

namespace DirectX
{
	HRESULT CreateDDSTextureFromFile12(_In_ ID3D12Device* device,
		_In_ ID3D12GraphicsCommandList* cmdList,
		_In_z_ const wchar_t* szFileName,
		_Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
		_Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,
		_In_ size_t maxsize = 0,
		_Out_opt_ DDS_ALPHA_MODE* alphaMode = nullptr
	);
}