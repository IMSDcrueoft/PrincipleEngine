#pragma once
#ifndef MATHPOINT
#define MATHPOINT
#include <cstdint>
#include <tuple>

namespace MathLib
{
	template<typename T,uint32_t alignSize>
	struct alignas(alignSize) Point
	{
		T x;
		T y;

	public:
		Point() = default;

		template<typename T2>
		Point(const T2& x, const T2& y)
		{
			this->x = static_cast<T>(x);
			this->y = static_cast<T>(y);
		}

		Point operator+(const Point& p)
		{
			Point result;
			result.x = this->x + p.x;
			result.y = this->y + p.y;
			
			return result;
		}

		Point operator-(const Point& p)
		{
			Point result;
			result.x = this->x - p.x;
			result.y = this->y - p.y;

			return result;
		}

		Point operator*(const Point& p)
		{
			Point result;
			result.x = this->x * p.x;
			result.y = this->y * p.y;

			return result;
		}

		Point operator/(const Point& p)
		{
			Point result;
			result.x = this->x / p.x;
			result.y = this->y / p.y;

			return result;
		}

		Point& operator+=(const Point& p)
		{
			this->x += p.x;
			this->y += p.y;

			return *this;
		}

		Point& operator-=(const Point& p)
		{
			this->x -= p.x;
			this->y -= p.y;

			return *this;
		}

		Point& operator*=(const Point& p)
		{
			this->x *= p.x;
			this->y *= p.y;

			return *this;
		}

		Point& operator/=(const Point& p)
		{
			this->x /= p.x;
			this->y /= p.y;

			return *this;
		}

		bool operator==(const Point& p)
		{
			return std::tie(this->x, this->y) == std::tie(p.x, p.y);
		}

		bool operator!=(const Point& p)
		{
			return std::tie(this->x, this->y) != std::tie(p.x, p.y);
		}

		//template<typename T1, uint32_t alignSize1, typename T2, uint32_t alignSize2>
		//static inline Point<T1, alignSize1> PointCast(const Point<T2, alignSize2>& p)
		//{
		//	return Point<T1, alignSize1>{ static_cast<T1>(p.x),static_cast<T1>(p.y) };
		//}
	};

	//int16_t
	using PointInt16 = Point<int16_t, 4u>;
	//uint16_t
	using PointUint16 = Point<uint16_t, 4u>;
	//int32_t
	using PointInt32 = Point<int32_t, 8u>;
	//uint32_t
	using PointUint32 = Point<uint32_t, 8u>;
	//float
	using PointFloat = Point<float, 8u>;
	//int64_t
	using PointInt64 = Point<int64_t, 16u>;
	//uint64_t
	using PointUint64 = Point<uint64_t, 16u>;
	//double
	using PointDouble = Point<double, 16u>;


	using PointS = PointInt16;
	using PointUS = PointUint16;
	using PointI = PointInt32;
	using PointU = PointUint32;
	using PointF = PointFloat;
	using PointLL = PointInt64;
	using PointULL = PointUint64;
	using PointD = PointDouble;
}
#endif