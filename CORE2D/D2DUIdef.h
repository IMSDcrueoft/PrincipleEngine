#pragma once
#ifndef D2DUIDEF
#define D2DUIDEF
#include<d2d1_3.h>
#pragma comment(lib,"d2d1.lib")

using PointU = D2D1_POINT_2U;
using PointL = D2D1_POINT_2L;
using PointF = D2D1_POINT_2F;

using RadiusXY = D2D1_POINT_2F;
using TextureSize = D2D1_POINT_2U;
using DivisionXY = D2D1_POINT_2U;

using RectF = D2D_RECT_F;
using ColorF = D2D1::ColorF;

template<typename TypePoint1, typename TypePoint2>
inline TypePoint1 PointAdd(const TypePoint1& p1, const TypePoint2& p2)
{
	TypePoint1 result;
	result.x = p1.x + p2.x;
	result.y = p1.y + p2.y;

	return result;
}

template<typename TypePoint>
inline TypePoint PointMul(const TypePoint& p1, const PointF& p2)
{
	TypePoint result;
	result.x = (static_cast<float>(p1.x) * p2.x);
	result.y = (static_cast<float>(p1.y) * p2.y);

	return result;
}

template<typename TypePoint>
inline TypePoint PointDiv(const TypePoint& p1, const PointF& p2)
{
	TypePoint result;
	result.x = (static_cast<float>(p1.x) / p2.x);
	result.y = (static_cast<float>(p1.y) / p2.y);

	return result;
}

template<typename TypePoint,typename D2D_Size>
inline TypePoint PointCast(const D2D_Size& p)
{
	return TypePoint{ p.width,p.height };
}

template<typename value_type, typename TypePoint, typename D2D_Point>
inline TypePoint PointTrans(const D2D_Point& p)
{
	return TypePoint{static_cast<value_type>(p.x),static_cast<value_type>(p.y) };
}

#endif // !D2DUIDEF