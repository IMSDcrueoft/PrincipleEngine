#include "ModelLoader.h"
using namespace DirectX;

void ModelLoader::eliminateDuplicates_tri(
	_Out_	std::vector<uint32_t>& VertexIndex,
	_In_	std::vector<MathLib::PMUINT32X4>& INDEX3)
{
	VertexIndex.clear();
	VertexIndex.push_back(0u);

	//因为已经存了一个索引了
	for (uint32_t i = 1u, size = 1u; i < INDEX3.size(); i++)
	{
		size = static_cast<uint32_t>(VertexIndex.size());

		for (uint32_t j = 0; j < VertexIndex.size(); j++)
		{
			if (INDEX3[VertexIndex[j]] == INDEX3[i])
			{
				VertexIndex.push_back(VertexIndex[j]);
				break;
			}
		}

		if (size == VertexIndex.size())
			VertexIndex.push_back(i);
	}

	//VertexIndex.clear();
	//VertexIndex.resize(INDEX3.size(),0);

	//std::vector<bool> flags;
	//flags.resize(INDEX3.size(), true);

	//for (uint32_t i = 0u; i < INDEX3.size(); i++)
	//{
	//	if (!flags[i])continue;

	//	VertexIndex[i] = i;
	//	flags[i] = false;

	//	for (uint32_t j = i + 1; j < INDEX3.size(); j++)
	//		if (flags[j] && (INDEX3[j] == INDEX3[i]))
	//		{
	//			VertexIndex[j] = i;
	//			flags[j] = false;
	//		}
	//}
	//flags.clear();
}

void ModelLoader::assemblyVertex_tri(
	_In_	std::vector<uint32_t>& VertexIndex,
	_In_	assemblyPreStored_tri& data,
	_Inout_	MeshData& mesh)
{
	std::vector<ModelData::Vertex> VertexArray;

	for (uint32_t i = 0; i < VertexIndex.size(); i++)
	{
		//错位说明已经装配了
		if (VertexIndex[i] < i)	continue;

		static ModelData::Vertex vertex;

		vertex.Position = data.POS	 [data.INDEX3[i].x];
		vertex.TexC     = data.TEX	 [data.INDEX3[i].y];
		vertex.Normal   = data.NORMAL[data.INDEX3[i].z];

		VertexArray.push_back(vertex);
	}

	mesh.Vertices = std::move(VertexArray);
}

void ModelLoader::assemblyIndice_tri(
	_Inout_	std::vector<uint32_t>& VertexIndex,
	_Inout_	MeshData& mesh)
{
	uint32_t i = 0;
	//找到从何处开始出现错位
	while ((i < VertexIndex.size()) && (VertexIndex[i] == i))i++;

	for (uint32_t max = i; i < VertexIndex.size(); i++)/*VertexIndex[i - 1] + 1*/
	{
		if (VertexIndex[i] < max)	continue;
		//纠正所有的错位索引
		for (uint32_t id = VertexIndex[i], j = i; j < VertexIndex.size(); j++)
		{
			if (VertexIndex[j] == id)	VertexIndex[j] = max;
		}
		max++;
	}

	mesh.Indices32 = std::move(VertexIndex);
}

bool ModelLoader::LoadWavefrontOBJ_Tri(
	_In_	const wchar_t* fileName,
	_Inout_	MeshData& mesh)
{
	std::ifstream fin;

	if (fin.open(fileName);!fin) return false;//failed

	//make vectors
	assemblyPreStored_tri data;

	std::string Str;
	char ignoreCStr[256];

	auto readPos = [&fin, &data]
	{
		static DirectX::XMFLOAT3 p;

		fin >> p.x >> p.y >> p.z;
		data.POS.push_back(p);
	};

	auto readTex = [&fin,&data]
	{
		static DirectX::XMFLOAT2 t;

		fin >> t.x >> t.y;
		/*
		* right u,up V
		* to right u,down v
		*/
		t.y = 1.0f - t.y;
		data.TEX.push_back(t);
	};

	auto readNormal = [&fin, &data]
	{
		static DirectX::XMFLOAT3 n;

		fin >> n.x >> n.y >> n.z;
		data.NORMAL.push_back(n);
	};

	auto readIndex = [&fin, &data,&ignoreCStr]
	{
		static uint32_t v, vt, vn;

		fin >> v >> ignoreCStr[0] >> vt >> ignoreCStr[0] >> vn;
		data.INDEX3.push_back(MathLib::PMUINT32X4{ v - 1,vt - 1,vn - 1 });
	};

	auto excludeRowData = [&fin,&ignoreCStr] 
	{
		fin.getline(&ignoreCStr[0], sizeof(ignoreCStr));
	};

	//readfile
	while (!fin.eof())
	{
		fin >> Str;
		if (fin.eof())	break;	// 是文件末尾


		if (Str[0] == '#')	    // 是注释行,直接排除
			excludeRowData();
		else
		if (Str == "s")	        // 是目前用不着的东西,排除
			fin >> Str;
		else
		if (Str == "v")	        // 是顶点数据
			readPos();	        // 坐标
		else
		if(Str == "vt")
			readTex();	        // uv
		else
		if(Str == "vn")
			readNormal();	    // 法线
		else
		if (Str == "f")			// 面
		{
			// 三角形面有三组索引
			readIndex();
			readIndex();
			readIndex();
		}
		else                    //其他乱七八糟的也排除
			excludeRowData();
	}
	fin.close();

	//这个mesh里是有重复的点,造成空间的浪费,需要调整
	std::vector<uint32_t> VertexIndex;
	eliminateDuplicates_tri(VertexIndex, data.INDEX3);
	//装配顶点
	assemblyVertex_tri(VertexIndex,data,mesh);
	//装配索引
	assemblyIndice_tri(VertexIndex,mesh);

	return true;
}

bool ModelLoader::loadCustomized_Tri_Pcm(
	_In_	const wchar_t* fileName,
	_Inout_	MeshData& mesh)
{
	std::ifstream fin;

	if (fin.open(fileName); !fin) return false;//failed

	std::string ignore;
	char ignoreCStr[256];
	uint32_t VertexCount, TriFaceCount;

	//排除头部的注释
	fin.getline(&ignoreCStr[0], sizeof(ignoreCStr));

	fin >> ignore >> VertexCount;
	fin >> ignore >> TriFaceCount;

	mesh.Vertices.resize(VertexCount);
	mesh.Indices32.resize((TriFaceCount*3));

	fin >> ignore >> ignore >> ignore;//VertexList	(Position,Normal,TexC,TangentU) {

	for (uint32_t id = 0; id < VertexCount; id++)
	{
		fin >> mesh.Vertices[id].Position.x >> mesh.Vertices[id].Position.y >> mesh.Vertices[id].Position.z
		    >> mesh.Vertices[id].Normal.x   >> mesh.Vertices[id].Normal.y   >> mesh.Vertices[id].Normal.z
		    >> mesh.Vertices[id].TexC.x     >> mesh.Vertices[id].TexC.y
		    >> mesh.Vertices[id].TangentU.x >> mesh.Vertices[id].TangentU.y >> mesh.Vertices[id].TangentU.z;
	}

	fin >> ignore >> ignore >> ignore;//} TriFaceList {

	//索引非常多,但是文件不可以启用并行加速，
	for (uint32_t id = 0; id < TriFaceCount; id++)
	{
		fin >> mesh.Indices32[id * 3] >> mesh.Indices32[id * 3 + 1] >> mesh.Indices32[id * 3 + 2];
	};

	fin.close();

	return true;
}

std::tuple<DirectX::XMVECTOR, DirectX::XMVECTOR> ModelLoader::GetMinPointAndMaxPointFromModel(
	_In_	std::vector<ModelData::Vertex>& vertices)
{
	float Infinity = FLT_MAX;

	DirectX::XMFLOAT3 vMinf3(+Infinity, +Infinity, +Infinity);
	DirectX::XMFLOAT3 vMaxf3(-Infinity, -Infinity, -Infinity);

	DirectX::XMVECTOR vMin = DirectX::XMLoadFloat3(&vMinf3);
	DirectX::XMVECTOR vMax = DirectX::XMLoadFloat3(&vMaxf3);

	for (const auto& vertex : vertices)
	{
		static DirectX::XMVECTOR P = XMLoadFloat3(&vertex.Position);

		vMin = DirectX::XMVectorMin(vMin, P);
		vMax = DirectX::XMVectorMax(vMax, P);
	}

	return std::make_tuple(vMin, vMax);
}

void ModelLoader::LoadModelFromFile(
	_In_	MDLForm form,
	_In_	const wchar_t* fileName,
	_Out_	MeshData& mesh)
{	
	mesh = MeshData();

	bool LoadSuccess = false;

	switch (form)
	{
	case MDLForm::WavefrontOBJ_Tri:
		LoadSuccess = LoadWavefrontOBJ_Tri(fileName, mesh); break;

	case MDLForm::CustomizedPCM_Tri:
		LoadSuccess = loadCustomized_Tri_Pcm(fileName, mesh); break;

	default:
		assert("Load Model Error");
	}
	assert(LoadSuccess && "Critical error! Model File Not Found!");
}

bool ModelLoader::StorageMeshToFileToCustomize(
	_In_	const wchar_t* fileName,
	_In_	MeshData& mesh)
{
	std::ofstream fout;

	if (fout.open(fileName); !fout) return false;//failed

	fout << "# PrincipleEngine Customized Mesh Model PCM file." << std::endl;

	fout << "VertexCount:\t" << mesh.Vertices.size() << std::endl
		 << "TriFaceCount:\t" << (mesh.Indices32.size() / 3) << std::endl;

	fout.setf(std::ios::fixed,std::ios::floatfield);
	fout.precision(6);

	fout << "VertexList\t"   << "(Position,Normal,TexC,TangentU)" << std::endl << '{' << std::endl;

	for (const auto& vertex : mesh.Vertices)
	{
		fout << '\t' << vertex.Position.x << '\t' << vertex.Position.y << '\t' << vertex.Position.z
			 << '\t' << vertex.Normal.x   << '\t' << vertex.Normal.y   << '\t' << vertex.Normal.z
			 << '\t' << vertex.TexC.x     << '\t' << vertex.TexC.y
			 << '\t' << vertex.TangentU.x << '\t' << vertex.TangentU.y << '\t' << vertex.TangentU.z << std::endl;
	}

	fout << '}' << std::endl<< "TriFaceList" << std::endl << '{' << std::endl;

	for (uint32_t id = 0; id < mesh.Indices32.size(); id += 3)
	{
		fout << '\t' << mesh.Indices32[id] 
			 << '\t' << mesh.Indices32[id + 1] 
			 << '\t' << mesh.Indices32[id + 2] << std::endl;
	}

	fout << '}';
	fout.close();

	return true;
}

void ModelLoader::GetBoundingBoxFromModel(
	_In_	std::vector<ModelData::Vertex>& vertices,
	_Out_	DirectX::BoundingBox& box)
{
	const auto [vMin, vMax] = GetMinPointAndMaxPointFromModel(vertices);

	DirectX::XMStoreFloat3(&box.Center, 0.5f * (vMin + vMax));
	DirectX::XMStoreFloat3(&box.Extents, 0.5f * (vMax - vMin));
}

void ModelLoader::GetBoundingSphereFromModel(
	_In_	std::vector<ModelData::Vertex>& vertices,
	_Out_	DirectX::BoundingSphere& sphere)
{
	const auto [vMin, vMax] = GetMinPointAndMaxPointFromModel(vertices);

	DirectX::XMStoreFloat3(&sphere.Center, 0.5f * (vMin + vMax));
	DirectX::XMVECTOR radius = DirectX::XMVector3Length(0.5f * (vMax - vMin));
	sphere.Radius = DirectX::XMVectorGetX(radius);
}


