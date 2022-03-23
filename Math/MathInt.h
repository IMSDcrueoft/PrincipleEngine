#pragma once
#ifndef MATHLIBINT_H
#define MATHLIBINT_H

#include <emmintrin.h>
#include <cstdint>
#include <cassert>
#include <tuple>

namespace MathLib
{
	//---------------------------------------------------------------------------
	//4 x UINT32_T
	using Int32X4 = __m128i;

	struct PMUINT32X4
	{
		union
		{
			Int32X4 data16byte;

			/*__declspec(align(16))*/
			struct alignas(16)
			{
				uint32_t x;
				uint32_t y;
				uint32_t z;
				uint32_t w;
			};
		};
	public:

		explicit PMUINT32X4(
			const uint32_t x = 0u,
			const uint32_t y = 0u,
			const uint32_t z = 0u,
			const uint32_t w = 0u);

		explicit PMUINT32X4(Int32X4 data);

		//C++20 with override <,>,==,!=,<=,>=
		//std::strong_ordering operator<=>(const PMUINT32X4&) const = default;
		uint32_t operator[](const uint32_t&);

		bool operator==(const PMUINT32X4&) const;

		PMUINT32X4& operator= (const PMUINT32X4&);
		PMUINT32X4& operator+=(const PMUINT32X4&);
		PMUINT32X4& operator-=(const PMUINT32X4&);
		PMUINT32X4& operator*=(const PMUINT32X4&);
		PMUINT32X4& operator/=(const PMUINT32X4&);

		PMUINT32X4& operator= (const uint32_t&);
		PMUINT32X4& operator+=(const uint32_t&);
		PMUINT32X4& operator-=(const uint32_t&);
		PMUINT32X4& operator*=(const uint32_t&);
		PMUINT32X4& operator/=(const uint32_t&);

		PMUINT32X4 operator+(const PMUINT32X4&);
		PMUINT32X4 operator-(const PMUINT32X4&);
		PMUINT32X4 operator*(const PMUINT32X4&);
		PMUINT32X4 operator/(const PMUINT32X4&);

		PMUINT32X4 operator+(const uint32_t&);
		PMUINT32X4 operator-(const uint32_t&);
		PMUINT32X4 operator*(const uint32_t&);
		PMUINT32X4 operator/(const uint32_t&);
	};

	//-------------------------------------------------------------------------

	inline PMUINT32X4::PMUINT32X4(
		const uint32_t x,
		const uint32_t y,
		const uint32_t z,
		const uint32_t w)
	{
		std::tie(this->x, this->y, this->z, this->w) = std::tie(x, y, z, w);
	}

	inline PMUINT32X4::PMUINT32X4(Int32X4 data)
	{
		data16byte = data;
	}

	//C++20 with override <,>,==,!=,<=,>=
	//std::strong_ordering operator<=>(const PMUINT32X4&) const = default;
	inline uint32_t PMUINT32X4::operator[](const uint32_t& index)
	{
#if defined(DEBUG) || defined(_DEBUG)
		assert(0 <= index && "Index out of range");
		assert(index <= 3 && "Index out of range");
#endif
		return data16byte.m128i_u32[index];
	}

	inline bool PMUINT32X4::operator==(const PMUINT32X4& other) const
	{
#if defined(DEBUG) || defined(_DEBUG)
		//Better in Debug,but not as good as Release
		Int32X4 _result = _mm_cmpeq_epi32(data16byte, other.data16byte);

		//这一步拖累了计算
		return (_result.m128i_u32[0] &
			_result.m128i_u32[1] &
			_result.m128i_u32[2] &
			_result.m128i_u32[3]);
#else
		//c++11
		return std::tie(x, y, z, w) == std::tie(other.x, other.y, other.z, other.w);
#endif
	}

	inline PMUINT32X4& PMUINT32X4::operator=(const PMUINT32X4& other)
	{
		if (this == &other)return *this;

		data16byte = other.data16byte;
		return *this;
	}

	inline PMUINT32X4& PMUINT32X4::operator=(const uint32_t& num)
	{
		data16byte = _mm_set1_epi32(num);
		return *this;
	}

	inline PMUINT32X4& PMUINT32X4::operator+=(const PMUINT32X4& other)
	{
		data16byte = _mm_add_epi32(data16byte, other.data16byte);
		return *this;
	}

	inline PMUINT32X4& PMUINT32X4::operator+=(const uint32_t& num)
	{
		data16byte = _mm_add_epi32(data16byte, _mm_set1_epi32(num));
		return *this;
	}

	inline PMUINT32X4& PMUINT32X4::operator-=(const PMUINT32X4& other)
	{
		data16byte = _mm_sub_epi32(data16byte, other.data16byte);
		return *this;
	}

	inline PMUINT32X4& PMUINT32X4::operator-=(const uint32_t& num)
	{
		data16byte = _mm_sub_epi32(data16byte, _mm_set1_epi32(num));
		return *this;
	}

	inline PMUINT32X4 PMUINT32X4::operator+(const PMUINT32X4& other)
	{
		return PMUINT32X4{ _mm_add_epi32(data16byte, other.data16byte) };
	}

	inline PMUINT32X4 PMUINT32X4::operator+(const uint32_t& num)
	{
		return PMUINT32X4{ _mm_add_epi32(data16byte, _mm_set1_epi32(num)) };
	}

	inline PMUINT32X4 PMUINT32X4::operator-(const PMUINT32X4& other)
	{
		return PMUINT32X4{ _mm_sub_epi32(data16byte, other.data16byte) };
	}

	inline PMUINT32X4 PMUINT32X4::operator-(const uint32_t& num)
	{
		return PMUINT32X4{ _mm_sub_epi32(data16byte, _mm_set1_epi32(num)) };
	}

	inline PMUINT32X4 PMUINT32X4::operator*(const PMUINT32X4& other)
	{
		return PMUINT32X4{ _mm_mul_epi32(data16byte, other.data16byte) };
	}

	inline PMUINT32X4& PMUINT32X4::operator*=(const PMUINT32X4& other)
	{
		data16byte = _mm_mul_epi32(data16byte, other.data16byte);
		return *this;
	}

	inline PMUINT32X4 PMUINT32X4::operator*(const uint32_t& num)
	{
		return PMUINT32X4{ _mm_mul_epi32(data16byte,  _mm_set1_epi32(num)) };
	}

	inline PMUINT32X4& PMUINT32X4::operator*=(const uint32_t& num)
	{
		data16byte = _mm_mul_epi32(data16byte, _mm_set1_epi32(num));
		return *this;
	}

	inline PMUINT32X4 PMUINT32X4::operator/(const PMUINT32X4& other)
	{
#if defined(DEBUG) || defined(_DEBUG)
		assert(other.x && "Dividend cannot be 0");
		assert(other.y && "Dividend cannot be 0");
		assert(other.z && "Dividend cannot be 0");
		assert(other.w && "Dividend cannot be 0");
#endif
		return PMUINT32X4{ _mm_div_epi32(data16byte, other.data16byte) };
	}

	inline PMUINT32X4& PMUINT32X4::operator/=(const PMUINT32X4& other)
	{
#if defined(DEBUG) || defined(_DEBUG)
		assert(other.x && "Dividend cannot be 0");
		assert(other.y && "Dividend cannot be 0");
		assert(other.z && "Dividend cannot be 0");
		assert(other.w && "Dividend cannot be 0");
#endif
		data16byte = _mm_div_epi32(data16byte, other.data16byte);
		return *this;
	}

	inline PMUINT32X4 PMUINT32X4::operator/(const uint32_t& num)
	{
#if defined(DEBUG) || defined(_DEBUG)
		assert(num && "Dividend cannot be 0");
#endif
		return PMUINT32X4{ _mm_div_epi32(data16byte, _mm_set1_epi32(num)) };
	}

	inline PMUINT32X4& PMUINT32X4::operator/=(const uint32_t& num)
	{
#if defined(DEBUG) || defined(_DEBUG)
		assert(num && "Dividend cannot be 0");
#endif
		data16byte = _mm_div_epi32(data16byte, _mm_set1_epi32(num));
		return *this;
	}
}
#endif