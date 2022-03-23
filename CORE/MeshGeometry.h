#pragma once

#ifndef MESHGEOMETRY
#define MESHGEOMETRY
#include <wrl.h>
#include <DirectXCollision.h>
#include <string>
#include <d3d12.h>
#include <unordered_map>
#include "DxException.h"

// 定义网格几何中的几何的子范围
// 当多个几何图形存储在一个顶点和索引缓冲区中
// 通过偏移量以及顶点和索引来获取
// 绘制几何存储子集所需的数据缓冲区
template<typename T = DirectX::BoundingBox>
struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// 此子网格定义的几何的边界框
	T Bounds;
};

/* 此结构用于打包物体数据与堆的指针 */
template<typename T = DirectX::BoundingBox>
struct MeshGeometry
{
	std::string Name;

	// 系统内存副本。 使用 Blob，因为顶点/索引格式可以是通用的
	// 由客户端决定是否适当地投射
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	// 有关缓冲区的数据
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// 网格几何可以在一个顶点/索引缓冲区中存储多个几何图形。
	// 使用此容器定义子网格几何图形,以便可以单独绘制子集
	std::unordered_map<std::string, SubmeshGeometry<T>> DrawArgs;

	MeshGeometry() = default;

	MeshGeometry(std::string name):Name(name){}

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// 通过类型自动保存有关缓冲的数据并创cpu端建内存缓冲区
	template<typename verticesType, typename indicesType>
	void SetPropertiesAndCreateBufferCPU(const verticesType& vertices, const indicesType& indices, DXGI_FORMAT format)
	{
		const auto& [vbByteSize, ibByteSize] = MathHelpTool::GetByteSize(vertices, indices);

		VertexBufferByteSize = vbByteSize;
		IndexBufferByteSize = ibByteSize;
		VertexByteStride = sizeof(vertices.front());
		IndexFormat = format;

		ThrowIfFailed(D3DCreateBlob(vbByteSize, VertexBufferCPU.GetAddressOf()));
		CopyMemory(VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, IndexBufferCPU.GetAddressOf()));
		CopyMemory(IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
	}

	// 完成上传到GPU后，我们可以释放此内存
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};
#endif // !MESHGEOMETRY