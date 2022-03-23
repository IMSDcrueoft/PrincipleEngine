#pragma once
#ifndef CUBERENDEROBJECT_H
#define CUBERENDEROBJECT_H

#include "d3dMethods.h"

enum class CubeMapFace : int32_t
{
	PositiveX = 0,
	NegativeX = 1,
	PositiveY = 2,
	NegativeY = 3,
	PositiveZ = 4,
	NegativeZ = 5
};

class CubeRenderObject
{
public:
	CubeRenderObject(ID3D12Device* device,
		const uint32_t& width, const uint32_t& height,
		const DXGI_FORMAT& format);

	CubeRenderObject(const CubeRenderObject& rhs) = delete;
	CubeRenderObject& operator=(const CubeRenderObject& rhs) = delete;
	~CubeRenderObject() = default;

	ID3D12Resource* Resource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv();
	CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv(const int32_t &faceIndex);

	D3D12_VIEWPORT Viewport()const;
	D3D12_RECT ScissorRect()const;

	void BuildDescriptors(
		const CD3DX12_CPU_DESCRIPTOR_HANDLE &hCpuSrv,
		const CD3DX12_GPU_DESCRIPTOR_HANDLE &hGpuSrv,
		const CD3DX12_CPU_DESCRIPTOR_HANDLE (&hCpuRtv)[6]);

	void OnResize(const uint32_t &newWidth,const uint32_t &newHeight);

private:
	void BuildDescriptors();
	void BuildResource();

private:

	ID3D12Device* md3dDevice = nullptr;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	uint32_t mWidth = 0;
	uint32_t mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv[6];

	Microsoft::WRL::ComPtr<ID3D12Resource> mCubeMap = nullptr;
};
#endif // !CUBERENDEROBJECT_H