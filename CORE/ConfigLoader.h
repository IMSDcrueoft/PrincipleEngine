#pragma once

#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H
#endif // !CONFIGLOADER_H

#include"LitTexMat.h"
#include"DDSLoader.h"
#include"DxException.h"
#include<vector>
#include<unordered_map>
#include<fstream>
#include<Windows.h>

class ConfigLoader
{
public:
	static bool LoadMaterialsFromFile(
		_In_	const wchar_t* fileName,
		_Inout_ std::unordered_map<std::string, std::unique_ptr<AdvancedMaterial>>& materials);

	static bool LoadTexturesFromFile(
		_In_	const wchar_t* fileName,
		_Inout_ std::unordered_map<std::string, std::unique_ptr<Texture>>& textures,
		_In_ ID3D12Device* device,
		_In_	ID3D12GraphicsCommandList* cmdList,
		_Out_ std::vector<std::string>& textureNames);
};

