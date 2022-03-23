#pragma once
#ifndef MODELDATA_H
#define MODELDATA_H

#include <cstdint>
#include <DirectXMath.h>
#include <vector>

class ModelData
{
public:
	struct Vertex
	{
		explicit Vertex() 
		{
			Position = { 0.0f,0.0f,0.0f };
			Normal   = { 0.0f,0.0f,0.0f };
			TangentU = { 0.0f,0.0f,0.0f };
			TexC     = { 0.0f,0.0f };
		}
		Vertex(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tan,
			const DirectX::XMFLOAT2& uv) :
			Position(pos),
			Normal(normal),
			TangentU(tan),
			TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			Position(px, py, pz),
			Normal(nx, ny, nz),
			TangentU(tx, ty, tz),
			TexC(u, v) {}

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 TangentU;
		DirectX::XMFLOAT2 TexC;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices32;

		std::vector<uint16_t>& GetIndices16()
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
					mIndices16[i] = static_cast<uint16_t>(Indices32[i]);
			}

			return mIndices16;
		}

	private:
		std::vector<uint16_t> mIndices16;
	};
};
#endif // !MODELDATA