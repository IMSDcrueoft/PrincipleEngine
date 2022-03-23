#pragma once
#ifndef LITTEXMAT_H
#define LITTEXMAT_H

#include <string>
#include "../Math/MathHelpTool.h"

#ifndef Align256bits 
#define Align256bits alignas(32)
#endif // !Align256bits 

#define CheckStructSize false

/* 帧资源的数量由外部定义 */
extern const int32_t gNumFrameResources;
/* 预设灯光数量 */
static constexpr uint32_t MaxLights = 16u;

struct Align256bits Light // 所有光照的统一结构
{
    DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };  // 光照强度：色彩
    float FalloffStart = 1.0f;                          // 仅 /			/点光源		/聚光灯
    DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// 仅 /方向光	/			/聚光灯
    float FalloffEnd = 10.0f;                           // 仅 /			/点光源		/聚光灯
    DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // 仅 /			/点光源		/聚光灯
    float SpotPower = 64.0f;                            // 仅 /			/			/聚光灯
};
#if CheckStructSize
//sizeof Light is 48 byte ,as 1 x 256bit + 128bit
static constexpr UINT sizeLT = sizeof(Light);
#endif

struct BasicMaterial	// 用于轻量级演示程序的简单材质,产品级引擎应当拥有重量级的材质
{
	std::string Name;

	// 索引到与此材质对应的常量缓冲区
	int32_t MatCBIndex = -1;

	// 索引到 SRV 堆以获取diffuse纹理
	int32_t DiffuseSrvHeapIndex = -1;

	// 索引到 SRV 堆以获取normal纹理。
	int32_t NormalSrvHeapIndex = -1;

	// Dirty flag 指示材质已更改,需要更新常量缓冲区
    // 因为每个 FrameResource 都有一个材质常量缓冲区,所以必须应用更新到每个 FrameResource
    // 因此，当修改材质时，应该设置 NumFramesDirty = gNumFrameResources 以便每个帧资源获得更新
	int32_t NumFramesDirty = gNumFrameResources;

	// 用于着色的材质常量缓冲数据
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelpTool::Identity4x4();

	BasicMaterial() = default;

	BasicMaterial(std::string name)noexcept
	{
		this->Name = name;
	}
};

struct AdvancedMaterial :public BasicMaterial	// 高级材质,需要更换着色程序,金属流工作
{
	float Metallic = 0.0f;// 金属度，默认非金属

	// 仅为凑整256字节，后期按需求修改
	DirectX::XMFLOAT3 __pad_96byte = { 0.0f,0.0f, 0.0f};
	DirectX::XMFLOAT4 __pad_128byte = { 0.0f,0.0f, 0.0f ,0.0f };

	AdvancedMaterial() = default;

	AdvancedMaterial(std::string name)noexcept :BasicMaterial(name){}
};

struct Align256bits BasicMaterialConstants
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };	// 4X4=16
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };			// 3x4=12
	float Roughness = 0.25f;										// 1x4=4
																	// 4x4x4=64
	DirectX::XMFLOAT4X4 MatTransform = MathHelpTool::Identity4x4(); // 用于材质映射
};
#if CheckStructSize
//sizeof BasicMaterialConstants is 96 byte ,as 3 x 256bit
static constexpr UINT sizeBM = sizeof(BasicMaterialConstants);
#endif

struct Align256bits AdvancedMaterialConstants:public BasicMaterialConstants	// 96
{
	float Metallic = 0.0f;										// 1x4=4

	// 仅为凑整256字节，后期按需求修改
	DirectX::XMFLOAT3 __pad_96byte = { 0.0f,0.0f, 0.0f };		// 96
	DirectX::XMFLOAT4 __pad_128byte = { 0.0f,0.0f, 0.0f ,0.0f };// 128
};
#if CheckStructSize
//sizeof AdvancedMaterialConstants is 128 byte ,as 4 x 256bit
static constexpr UINT sizeAM = sizeof(AdvancedMaterialConstants);
#endif

struct Texture
{
	std::string Name;// 纹理的名称

	std::wstring Filename;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;

	Texture() = default;

	Texture(std::string name)noexcept
	{
		this->Name = name;
	}
};
#endif // !LITTEXMAT