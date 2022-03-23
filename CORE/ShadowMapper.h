#pragma once
#ifndef SHADOWMAPPER_H
#define SHADOWMAPPER_H

#include <cstdint>
#include <wrl.h>
#include "d3dx12.h"
#include "DxException.h"

class ShadowMapper
{
public:
	ShadowMapper(ID3D12Device* device,
		uint32_t width, uint32_t height);

	ShadowMapper(const ShadowMapper& rhs) = delete;
	ShadowMapper& operator=(const ShadowMapper& rhs) = delete;
	~ShadowMapper() = default;

	const uint32_t& Width() const;
	const uint32_t& Height() const;

	ID3D12Resource* Resource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Dsv() const;

	D3D12_VIEWPORT Viewport() const;
	D3D12_RECT ScissorRect() const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

	void OnResize(const uint32_t& newWidth,const uint32_t& newHeight);

private:
	void BuildDescriptors();
	void BuildResource();

private:
	ID3D12Device* md3dDevice = nullptr;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	uint32_t mWidth;
	uint32_t mHeight;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R24G8_TYPELESS;

	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv;

	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
};

#endif // !SHADOWMAPPER_H