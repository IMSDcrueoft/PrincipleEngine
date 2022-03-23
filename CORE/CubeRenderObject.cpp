#include "CubeRenderObject.h"

CubeRenderObject::CubeRenderObject(
	ID3D12Device* device,
	const uint32_t& width, const uint32_t& height,
	const DXGI_FORMAT& format)
{
	md3dDevice = device;

	mWidth = width;
	mHeight = height;
	mFormat = format;

	mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
	mScissorRect = { 0, 0, (int32_t)width, (int32_t)height };

	BuildResource();
}

ID3D12Resource* CubeRenderObject::Resource()
{
	return mCubeMap.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CubeRenderObject::Srv()
{
	return mhGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CubeRenderObject::Rtv(const int32_t& faceIndex)
{
	return mhCpuRtv[faceIndex];
}

D3D12_VIEWPORT CubeRenderObject::Viewport()const
{
	return mViewport;
}

D3D12_RECT CubeRenderObject::ScissorRect()const
{
	return mScissorRect;
}

void CubeRenderObject::BuildDescriptors(
	const CD3DX12_CPU_DESCRIPTOR_HANDLE &hCpuSrv,
	const CD3DX12_GPU_DESCRIPTOR_HANDLE &hGpuSrv,
	const CD3DX12_CPU_DESCRIPTOR_HANDLE(&hCpuRtv)[6])
{
	// 把引用保存到描述符
	mhCpuSrv = hCpuSrv;
	mhGpuSrv = hGpuSrv;

	/*for (int i = 0; i < 6; ++i)
		mhCpuRtv[i] = hCpuRtv[i];*/
	// 展开循环，提高运行速度
	mhCpuRtv[0] = hCpuRtv[0];
	mhCpuRtv[1] = hCpuRtv[1];
	mhCpuRtv[2] = hCpuRtv[2];
	mhCpuRtv[3] = hCpuRtv[3];
	mhCpuRtv[4] = hCpuRtv[4];
	mhCpuRtv[5] = hCpuRtv[5];

	// 创建描述符
	BuildDescriptors();
}

void CubeRenderObject::OnResize(const uint32_t& newWidth, const uint32_t& newHeight)
{
	if ((mWidth != newWidth) || (mHeight != newHeight))
	{
		mWidth = newWidth;
		mHeight = newHeight;

		BuildResource();

		// 新的资源需要新的描述符去指向
		BuildDescriptors();
	}
}

void CubeRenderObject::BuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = mFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	// 为整个立方体贴图资源创建 SRV
	md3dDevice->CreateShaderResourceView(mCubeMap.Get(), &srvDesc, mhCpuSrv);

	// 为每个立方体面创建RTV
	for (int32_t i = 0; i < 6; ++i)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Format = mFormat;
		rtvDesc.Texture2DArray.MipSlice = 0;
		rtvDesc.Texture2DArray.PlaneSlice = 0;

		// 为每个元素创建渲染目标视图
		rtvDesc.Texture2DArray.FirstArraySlice = i;

		// 仅为看数组的每一个元素创造一个视图
		rtvDesc.Texture2DArray.ArraySize = 1;

		// 为第 i 个立方体贴图面创建 RTV
		md3dDevice->CreateRenderTargetView(mCubeMap.Get(), &rtvDesc, mhCpuRtv[i]);
	}
}

void CubeRenderObject::BuildResource()
{
	// 注意，压缩格式不能用于 UAV。 我们得到如下错误：
	// 错误: ID3D11Device::CreateTexture2D: 
	// 格式 (0x4d, BC3_UNORM) 不能绑定为 UnorderedAccessView，
	// 或强制转换为可以绑定为 UnorderedAccessView。
	// 因此这种格式不支持 D3D11_BIND_UNORDERED_ACCESS。

	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.DepthOrArraySize = 6;
	texDesc.MipLevels = 1;
	texDesc.Format = mFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		GetTypePointer(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mCubeMap)));
}