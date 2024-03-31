//#include "ShadowMapper.h"
//
//ShadowMapper::ShadowMapper(ID3D12Device* device, uint32_t width, uint32_t height)
//{
//	this->md3dDevice = device;
//	this->mWidth = width;
//	this->mHeight = height;
//
//	this->mViewport = { 0.0f, 0.0f, static_cast<float>(this->mWidth), static_cast<float>(this->mHeight), 0.0f, 1.0f };
//	this->mScissorRect = { 0, 0, static_cast<int32_t>(this->mWidth), static_cast<int32_t>(this->mHeight) };
//
//	this->BuildResource();
//}
//
//void ShadowMapper::BuildResource()
//{
//	D3D12_RESOURCE_DESC texDesc;
//	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
//	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	texDesc.Alignment = 0;
//	texDesc.Width = this->mWidth;
//	texDesc.Height = this->mHeight;
//	texDesc.DepthOrArraySize = 1;
//	texDesc.MipLevels = 1;
//	texDesc.Format = this->mFormat;
//	texDesc.SampleDesc.Count = 1;
//	texDesc.SampleDesc.Quality = 0;
//	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
//
//	CD3DX12_CLEAR_VALUE optClear;
//	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	optClear.DepthStencil.Depth = 1.0f;
//	optClear.DepthStencil.Stencil = 0;
//
//	ThrowIfFailed(this->md3dDevice->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
//		D3D12_HEAP_FLAG_NONE,
//		&texDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		&optClear,
//		IID_PPV_ARGS(&this->mShadowMap)));
//}
//
//ID3D12Resource* ShadowMapper::Resource()
//{
//	return this->mShadowMap.Get();
//}
//
//CD3DX12_GPU_DESCRIPTOR_HANDLE ShadowMapper::Srv() const
//{
//	return this->hGpuSrv;
//}
//
//CD3DX12_CPU_DESCRIPTOR_HANDLE ShadowMapper::Dsv() const
//{
//	return this->hCpuDsv;
//}
