#pragma once
#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>
#include <float.h>
#include <cmath>
#include "RandomBase.h"

// just get pointer of return value
template<typename T>
inline constexpr T* GetTypePointer(T&& value)
{
	return &value;
}

class MathHelpTool:public RandomBase
{
public:
	static constexpr float Infinity = FLT_MAX;
	static constexpr float Pi = DirectX::XM_PI;

	static inline uint32_t alignedBytesAs64(const uint32_t& originalSize)
	{
		return (originalSize + 63u) & ~63u;
	}

	static inline uint32_t alignedBytesAs128(const uint32_t& originalSize)
	{
		return (originalSize + 127u) & ~127u;
	}

	static inline uint32_t alignedBytesAs256(const uint32_t& originalSize)
	{
		return (originalSize + 255u) & ~255u;
	}

	static inline uint32_t alignedBytesAsCustom(const uint32_t& originalSize, const uint32_t& targetAlignSize)
	{
		return targetAlignSize * static_cast<uint32_t>(std::ceilf(static_cast<float>(originalSize) / static_cast<float>(targetAlignSize)));
	}

	/* 判断浮点数相等 */
	template <typename T>
	static inline bool fpEqual(T A, T B, T error) {}

	/* 判断float相等 */
	template<> static inline bool fpEqual<float>(float A, float B, float error)
	{
		return (fabsf(A - B) <= error);
	}
	/* 判断double相等 */
	template<> static inline bool fpEqual<double>(double A, double B, double error)
	{
		return (fabs(A - B) <= error);
	}

	template<typename T>
	static inline T Lerp(const T& a, const T& b, float t)
	{
		return (b - a) * t + a;
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static inline float AngleFromXY(float x, float y)
	{
		float theta = 0.0f;

		// Quadrant I or IV
		if (x >= 0.0f)
		{
			// If x = 0, then atanf(y/x) = +pi/2 if y > 0
			//                atanf(y/x) = -pi/2 if y < 0
			theta = atanf(y / x); // in [-pi/2, +pi/2]

			if (theta < 0.0f)
				theta += 2.0f * Pi; // in [0, 2*pi).
		}

		// Quadrant II or III
		else
			theta = atanf(y / x) + Pi; // in [0, 2*pi).

		return theta;
	}

	// 获取逆矩阵
	static inline DirectX::XMMATRIX GetXMMatrixInverse(const DirectX::XMMATRIX& matrix)
	{
		//return M^-1,with XMMatrixInverse(*det(M),M)
		return DirectX::XMMatrixInverse(GetTypePointer(DirectX::XMMatrixDeterminant(matrix)), matrix);
	}

	static inline DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius*sinf(phi)*cosf(theta),
			radius*cosf(phi),
			radius*sinf(phi)*sinf(theta),
			1.0f);
	}

	static inline DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
        DirectX::XMMATRIX A = M;
        A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
        return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

	/* 获取4x4的单位矩阵 */
	static inline DirectX::XMFLOAT4X4 Identity4x4()
    {
		static constexpr DirectX::XMFLOAT4X4 I
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f 
		};

		return I;
    }

	static inline DirectX::XMVECTOR RandUnitVec3()
	{
		static const DirectX::XMVECTOR One = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		static const DirectX::XMVECTOR Zero = DirectX::XMVectorZero();

		// Keep trying until we get a point on/in the hemisphere.
		while (true)
		{
			// Generate random point in the cube [-1,1]^3.
			DirectX::XMVECTOR v = DirectX::XMVectorSet(MathHelpTool::RandF(-1.0f, 1.0f), MathHelpTool::RandF(-1.0f, 1.0f), MathHelpTool::RandF(-1.0f, 1.0f), 0.0f);

			// Ignore points outside the unit sphere in order to get an even distribution 
			// over the unit sphere.  Otherwise points will clump more on the sphere near 
			// the corners of the cube.

			if (DirectX::XMVector3Greater(DirectX::XMVector3LengthSq(v), One))
				continue;

			return DirectX::XMVector3Normalize(v);
		}
	}

	static inline DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n)
	{
		static const DirectX::XMVECTOR One = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		static const DirectX::XMVECTOR Zero = DirectX::XMVectorZero();

		// Keep trying until we get a point on/in the hemisphere.
		while (true)
		{
			// Generate random point in the cube [-1,1]^3.
			DirectX::XMVECTOR v = DirectX::XMVectorSet(MathHelpTool::RandF(-1.0f, 1.0f), MathHelpTool::RandF(-1.0f, 1.0f), MathHelpTool::RandF(-1.0f, 1.0f), 0.0f);

			// Ignore points outside the unit sphere in order to get an even distribution 
			// over the unit sphere.  Otherwise points will clump more on the sphere near 
			// the corners of the cube.

			if (DirectX::XMVector3Greater(DirectX::XMVector3LengthSq(v), One))
				continue;

			// Ignore points in the bottom hemisphere.
			if (DirectX::XMVector3Less(DirectX::XMVector3Dot(n, v), Zero))
				continue;

			return DirectX::XMVector3Normalize(v);
		}
	}

	/* 获取容器内总字节数,需要连续的线性的数据,例如array vector或者其他支持size()和front()的数据类型,多容器一次性返回 */
	template<typename T, typename...Args>
	static inline auto GetByteSize(const T& container, const Args& ...args)
	{
		auto ByteSize = [&container] {
			if (container.size() > 0)
				return container.size() * sizeof(container.front());
			else
				return size_t(0u);
		};

		if constexpr (sizeof ...(args) > 0)
			return std::make_tuple(ByteSize(), GetByteSize(args...));
		else
			return ByteSize();
	}

	/// *获取容器内总字节数For linear STL*/
	//template<typename T>
	//static inline uint64_t GetByteSize(T& container)
	//{
	//	return container.size() * sizeof(container.front());
	//}

	static inline DirectX::XMFLOAT4 SetColorRGBAf(
		const uint8_t RED   = 0xFF,
		const uint8_t GREEN = 0xFF,
		const uint8_t BLUE  = 0xFF,
		const float   ALPHA = 1.0f)
	{
		return DirectX::XMFLOAT4(static_cast<float>(RED  )/ 255.0f,
								 static_cast<float>(GREEN)/ 255.0f,
								 static_cast<float>(BLUE )/ 255.0f,
										 std::clamp(ALPHA, 0.0f, 1.0f));
	}

	// 4x8进制颜色转换RGBA to XMFLOAT4
	static inline DirectX::XMFLOAT4 SetColorRGBA(
		const uint8_t RED   = 0xFF,
		const uint8_t GREEN = 0xFF,
		const uint8_t BLUE  = 0xFF,
		const uint8_t ALPHA = 0xFF)
	{
		return DirectX::XMFLOAT4(static_cast<float>(RED  )/ 255.0f,
								 static_cast<float>(GREEN)/ 255.0f,
								 static_cast<float>(BLUE )/ 255.0f,
								 static_cast<float>(ALPHA)/ 255.0f);
	}

	// 16进制颜色转换RGBA to XMFLOAT4
	static inline DirectX::XMFLOAT4 SetColorHEX(
		const uint32_t RGBA = 0xFF'FF'FF'FF)
	{
		return SetColorRGBA(static_cast<uint8_t>(RGBA >> 24),
							static_cast<uint8_t>(RGBA >> 16),
							static_cast<uint8_t>(RGBA >>  8),
							static_cast<uint8_t>(RGBA));
	}

	// 3x8进制颜色转换RGBA to XMFLOAT4
	static inline DirectX::XMFLOAT3 SetColorRGB(
		const uint8_t RED   = 0xFF,
		const uint8_t GREEN = 0xFF,
		const uint8_t BLUE  = 0xFF)
	{
		return DirectX::XMFLOAT3(static_cast<float>(RED  )/ 255.0f,
								 static_cast<float>(GREEN)/ 255.0f,
								 static_cast<float>(BLUE )/ 255.0f);
	}
};
#endif // MATHHELPER_H
