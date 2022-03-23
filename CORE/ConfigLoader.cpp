#include "ConfigLoader.h"

bool ConfigLoader::LoadMaterialsFromFile(
	_In_ const wchar_t* fileName,
	_Inout_ std::unordered_map<std::string, std::unique_ptr<AdvancedMaterial>>& materials)
{
	std::ifstream fin;

	if (fin.open(fileName); !fin) return false;//failed

	std::string ignore;
	char ignoreCStr[256];
	uint32_t materialCount;

	//排除头部的注释
	fin.getline(&ignoreCStr[0], sizeof(ignoreCStr));

	fin >> ignore >> materialCount;

	fin >> ignore >> ignore >> ignore;

	for (uint32_t id = 0; id < materialCount; ++id)
	{
		auto mat = std::make_unique<AdvancedMaterial>();

		mat->MatCBIndex = id;

		fin >> mat->Name
			>> mat->DiffuseSrvHeapIndex
			>> mat->NormalSrvHeapIndex
			>> mat->MetallicSrvHeapIndex
			>> mat->DiffuseAlbedo.x >> mat->DiffuseAlbedo.y >> mat->DiffuseAlbedo.z >> mat->DiffuseAlbedo.w
			>> mat->FresnelR0.x >> mat->FresnelR0.y >> mat->FresnelR0.z
			>> mat->Roughness
			>> mat->Metallic;

		materials[mat->Name] = std::move(mat);
	}

	fin.close();

	return true;
}

bool ConfigLoader::LoadTexturesFromFile(
	_In_ const wchar_t* fileName,
	_Inout_ std::unordered_map<std::string, std::unique_ptr<Texture>>& textures,
	_In_ ID3D12Device* device,
	_In_	ID3D12GraphicsCommandList* cmdList,
	_Out_ std::vector<std::string>& textureNames)
{
	std::ifstream fin;

	if (fin.open(fileName); !fin) return false;//failed

	std::string ignore;
	char ignoreCStr[256];
	uint32_t textureCount;

	//排除头部的注释
	fin.getline(&ignoreCStr[0], sizeof(ignoreCStr));

	fin >> ignore >> textureCount;

	fin >> ignore >> ignore >> ignore;

	textureNames.resize(textureCount);

	for (uint32_t id = 0; id < textureCount; ++id)
	{
		auto tex = std::make_unique<Texture>();

		std::string fileName;

		fin >> tex->Name
			>> fileName;

		textureNames[id] = tex->Name;

		uint32_t size = MultiByteToWideChar(CP_ACP, 0, fileName.data(), -1, nullptr, 0);

		tex->Filename.resize(size * sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, fileName.data(), -1, tex->Filename.data(), size);

		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(device,
			cmdList, tex->Filename.c_str(),
			tex->Resource, tex->UploadHeap));

		textures[tex->Name] = std::move(tex);
	}

	fin.close();

	return false;
}
