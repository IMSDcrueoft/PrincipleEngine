#pragma once
#ifndef D3DMETHODS_H
#define D3DMETHODS_H

#include <windows.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include "d3d12.h"
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include <filesystem>
#include <tuple>
#include "d3dx12.h"
#include "DDSTextureLoader12.h"
#include "DDSLoader.h"
#include "LitTexMat.h"
#include "MeshGeometry.h"
#include "DxException.h"
#include "../Math/Utilitys.h"
#include "../Math/MathHelpTool.h"

/*
#if defined(_DEBUG)
    #ifndef Assert
    #define Assert(x, description)                                  \
    {                                                               \
        static bool ignoreAssert = false;                           \
        if(!ignoreAssert && !(x))                                   \
        {                                                           \
            Debug::AssertResult result = Debug::ShowAssertDialog(   \
            (L#x), description, AnsiToWString(__FILE__), __LINE__); \
        if(result == Debug::AssertIgnore)                           \
        {                                                           \
            ignoreAssert = true;                                    \
        }                                                           \
                    else if(result == Debug::AssertBreak)           \
        {                                                           \
            __debugbreak();                                         \
        }                                                           \
        }                                                           \
    }
    #endif
#else
    #ifndef Assert
    #define Assert(x, description) 
    #endif
#endif
*/

class D3DMethods
{
public:

    static std::string ToString(HRESULT hr);

    static inline void d3dSetDebugName(IDXGIObject* obj,LPCSTR name)
    {
        if (obj)
        {
            obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
        }
    }

    static inline UINT CalcConstantBufferByteSize(const UINT& byteSize)
    {
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        // ~255按位取反,再做按位与运算,这样255部分的二进制位永远是0
        return (UINT)MathHelpTool::alignedBytesAs256(byteSize);
    }

    static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);

    //6个静态采样器
    static std::array <const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};

#endif // !D3DMETHODS