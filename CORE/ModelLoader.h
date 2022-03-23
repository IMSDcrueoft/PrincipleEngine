#pragma once
#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <fstream> 
#include <iostream>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "ModelData.h"
#include "../Math/MathInt.h"

// 用于从文件加载模型
class ModelLoader :public ModelData
{
protected:
	struct assemblyPreStored_tri
	{
		std::vector<DirectX::XMFLOAT3> POS;
		std::vector<DirectX::XMFLOAT2> TEX;
		std::vector<DirectX::XMFLOAT3> NORMAL;

		std::vector<MathLib::PMUINT32X4> INDEX3;
	};

	// 有大量顶点重复,需要剔除
	static void eliminateDuplicates_tri(
		_Out_   std::vector<uint32_t>& VertexIndex,			// 返回对于INDEX3的索引
		_In_	std::vector<MathLib::PMUINT32X4>& INDEX3);	// 文件中读出的索引

	// 修正并装配顶点
	static void assemblyVertex_tri(
		_In_	std::vector<uint32_t>& VertexIndex,		// 对于INDEX3的索引
		_In_	assemblyPreStored_tri& data,			// 文件中读出的数据
		_Inout_	MeshData& mesh
	);

	// 修正并装配索引
	static void assemblyIndice_tri(
		_Inout_ std::vector<uint32_t>& VertexIndex,
		_Inout_	MeshData& mesh
	);

	// 从文件载入OBJ三角形面片网格模型
	static bool LoadWavefrontOBJ_Tri(
		_In_	const wchar_t* fileName,
		_Inout_ MeshData& mesh);

	// 从文件加载PCM三角形面片网格模型
	static bool loadCustomized_Tri_Pcm(
		_In_	const wchar_t* fileName,
		_Inout_	MeshData& mesh
	);

	// 从网格获取最大最小点
	static std::tuple<DirectX::XMVECTOR, DirectX::XMVECTOR> GetMinPointAndMaxPointFromModel(
		_In_	std::vector<ModelData::Vertex>& vertices);

public:
	enum class MDLForm
	{
		WavefrontOBJ_Tri,
		CustomizedPCM_Tri
	};

	// 从模型文件加载模型网格,请按需求导出平滑着色与平直着色网格
	static void LoadModelFromFile(
		_In_	MDLForm form,
		_In_	const wchar_t* fileName,
		_Out_	MeshData& mesh);

	// 将网格模型存储成定制的便于加载的格式
	static bool StorageMeshToFileToCustomize(
		_In_	const wchar_t* fileName,
		_In_	MeshData& mesh
	);

	// 从网格获取包围体
	static void GetBoundingBoxFromModel(
		_In_	std::vector<ModelData::Vertex>& vertices,
		_Out_	DirectX::BoundingBox& box);

	static void GetBoundingSphereFromModel(
		_In_	std::vector<ModelData::Vertex>& vertices,
		_Out_	DirectX::BoundingSphere& sphere);
};

#endif // !MODELLOADER