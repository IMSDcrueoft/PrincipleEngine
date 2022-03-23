#pragma once
#ifndef MATHLIB_H
#define MATHLIB_H

#include <intrin.h>
#include <cstdint>
#include <cassert>
#include <tuple>
#include <cmath>

#include "MathInt.h"
#include "MathNum.h"

//require AVX256
namespace MathLib
{
//---------------------------------------------------------------------------
#define PM_ADD(a,b)			_mm256_add_pd(a,b)
#define PM_SUB(a,b)			_mm256_sub_pd(a,b)
#define PM_MUL(a,b)			_mm256_mul_pd(a,b)
#define PM_DIV(a,b)			_mm256_div_pd(a,b)

#define PM_MULADD(a,b,c)	_mm256_add_pd(_mm256_mul_pd(a,b),c)
#define PM_MULSUB(a,b,c)	_mm256_sub_pd(_mm256_mul_pd(a,b),c)

#define PM_ZERO4D()			_mm256_setzero_pd()
#define PM_ONE4D()			_mm256_set1_pd(1.0)
#define PM_Replicate4D(a)	_mm256_set1_pd(a)
#define PM_LOAD(a,b,c,d)	_mm256_set_pd(a,b,c,d)

#define PM_MULNUM(a,b)		_mm256_mul_pd(a,_mm256_set1_pd(b))
#define PM_DIVNUM(a,b)		_mm256_div_pd(a,_mm256_set1_pd(b))

// _mm256_permute4x64_pd接受一个__m256d参数和一个8位二进制数
// 8位二进制数每两个一组根据其在相应的8位二进制数中的位置
// 将传入的__m256d的第几位赋到返回的__m256d的第几位
// 例如传入a0,a1,a2,a3和0b11100100(转换为四个十进制数是3,2,1,0)
// 那么返回的值就是a3,a2,a1,a0，这里需要注意一点是寄存器中的数据排列问题
// 如果我们要倒置内存中的整个序列实际上应该给的二进制数是0b00011011

//0b01001110 = register 1,0,3,2 =>mem 2,3,0,1
//register 3,1,2,0 =>mem 0,2,1,3
//register 2,3,0,1 =>mem 1,0,3,2
//register 0,1,2,3 =>mem 3,2,1,0
#define PM_Swap_2301(a) _mm256_permute4x64_pd(a,0b01'00'11'10)
#define PM_Swap_0213(a) _mm256_permute4x64_pd(a,0b11'01'10'00)
#define PM_Swap_1032(a) _mm256_permute4x64_pd(a,0b10'11'00'01)
#define PM_Swap_3210(a) _mm256_permute4x64_pd(a,0b00'01'10'11)

// _mm256_blend_pd接受两个__m256d参数a和b，和一个四位二进制数
// 根据二进制数中每一位是1还是0，确定生成的新的__m256d对应位置是取a的值还是b的值
// 例如传入a0,a1,a2,a3和b0,b1,b2,b3和 0b0101 则生成的数据应该a0,b1,a2,b3
// 同样这里也需要注意寄存器中数据的排列
//reg 0b1100 =>mem 0011=> x1 x2 y3 y4-> 0 a->1 b
#define PM_Blend_aabb(a,b) _mm256_blend_pd(a,b,0b11'00)
#define PM_Blend_bbaa(a,b) _mm256_blend_pd(a,b,0b00'11)
#define PM_Blend_abab(a,b) _mm256_blend_pd(a,b,0b10'10)
#define PM_Blend_baba(a,b) _mm256_blend_pd(a,b,0b01'01)

#define PM_Blend_abbb(a,b) _mm256_blend_pd(a,b,0b11'10)
#define PM_Blend_babb(a,b) _mm256_blend_pd(a,b,0b11'01)
#define PM_Blend_bbab(a,b) _mm256_blend_pd(a,b,0b10'11)
#define PM_Blend_bbba(a,b) _mm256_blend_pd(a,b,0b01'11)

// _mm256_unpacklo_pd接受两个__m256d参数
// 并且将两个传入的参数中的第一、第三个数按顺序组成新的__m256d返回
// 例如当传入a0, a1, a2, a3和b0, b1, b2, b3时，则返回a0, b0, a2, b2
// _mm256_unpackhi_pd与_mm256_unpacklo_pd基本相似，只不过是取得传入值的第二、第四个数组合返回
#define PM_Combine1133(a,b) _mm256_unpacklo_pd(a,b)
#define PM_Combine2244(a,b) _mm256_unpackhi_pd(a,b)
	//---------------------------------------------------------------------------
	//4D Vector
	using DoubleX4 = __m256d;

	struct PMMATRIXD;
	struct PMVECTOR4D
	{
		union
		{
			DoubleX4 VECTOR4D;

			/*__declspec(align(32)) struct*/
			struct alignas(32)
			{
				double x;
				double y;
				double z;
				double w;
			};
		};

		 explicit PMVECTOR4D(
			const double x = 0.0,
			const double y = 0.0,
			const double z = 0.0,
			const double w = 0.0);

		 explicit PMVECTOR4D(DoubleX4 D4);

		 PMVECTOR4D& operator= (const PMVECTOR4D&);
		 PMVECTOR4D& operator+=(const PMVECTOR4D&);
		 PMVECTOR4D& operator-=(const PMVECTOR4D&);
		 PMVECTOR4D& operator*=(const PMVECTOR4D&);
		 PMVECTOR4D& operator/=(const PMVECTOR4D&);

		 PMVECTOR4D& operator*=(const double);
		 PMVECTOR4D& operator/=(const double);

		 PMVECTOR4D& operator*=(const PMMATRIXD&);

		 PMVECTOR4D operator+(const PMVECTOR4D&)const;
		 PMVECTOR4D operator-(const PMVECTOR4D&)const;
		 PMVECTOR4D operator*(const PMVECTOR4D&)const;
		 PMVECTOR4D operator/(const PMVECTOR4D&)const;

		 PMVECTOR4D operator*(const double)const;
		 PMVECTOR4D operator/(const double)const;

		 friend PMVECTOR4D operator*(const double, const PMVECTOR4D&);
		 friend PMVECTOR4D operator/(const double, const PMVECTOR4D&);

		 inline PMVECTOR4D operator*(const PMMATRIXD&)const;
	};

	//---------------------------------------------------------------------------
	//4x4 Matrix
	struct PMMATRIXD
	{
		union
		{
			alignas(32) PMVECTOR4D ROW[4];

			//To facilitate calculation, save in the form of transposed matrix
			struct alignas(32)
			{
				double _11, _21, _31, _41;
				double _12, _22, _32, _42;
				double _13, _23, _33, _43;
				double _14, _24, _34, _44;
			};
		};

		PMMATRIXD();

		PMMATRIXD(
			double __11, double __12, double __13, double __14,
			double __21, double __22, double __23, double __24,
			double __31, double __32, double __33, double __34,
			double __41, double __42, double __43, double __44);

		PMMATRIXD(
			const PMVECTOR4D& v1,
			const PMVECTOR4D& v2,
			const PMVECTOR4D& v3,
			const PMVECTOR4D& v4 = PMVECTOR4D{ 0.0,0.0,0.0,1.0 },
			bool transform = true);

		PMMATRIXD operator*(const PMMATRIXD&);
		PMMATRIXD operator+(const PMMATRIXD&);
		PMMATRIXD operator-(const PMMATRIXD&);
	};

	double degreeToRadian(double);
	double radianToDegree(double);
	double Length(const PMVECTOR4D&);
	double Dot(const PMVECTOR4D&, const PMVECTOR4D&);

	PMVECTOR4D GetZeroVector4D();
	PMVECTOR4D GetOneVector3D();
	PMVECTOR4D GetOneVector4D();
	PMVECTOR4D GetZeroPoint3D();
	PMVECTOR4D SetPoint3D(const double, const double, const double);
	PMVECTOR4D Normalize(const PMVECTOR4D&);
	PMVECTOR4D Cross(const PMVECTOR4D&, const PMVECTOR4D&);

	PMMATRIXD MakeIdentityMatrix();
	PMMATRIXD MakeRotationMatrixX(const double);
	PMMATRIXD MakeInvRotationMatrixX(const double);
	PMMATRIXD MakeRotationMatrixY(const double);
	PMMATRIXD MakeInvRotationMatrixY(const double);
	PMMATRIXD MakeRotationMatrixZ(const double);
	PMMATRIXD MakeInvRotationMatrixZ(const double);
	PMMATRIXD MakeScaleMatrix(const double);
	PMMATRIXD MakeRotationMatrix(const PMVECTOR4D&, const double);
	PMMATRIXD MakeInvRotationMatrix(const PMVECTOR4D&, const double);
	PMMATRIXD MakeScaleMatrix(const PMVECTOR4D&);
	PMMATRIXD MakeInvScaleMatrix(const PMVECTOR4D&);
	PMMATRIXD MakeTranslationMatrix(const PMVECTOR4D&);
	PMMATRIXD GetTransposeMatrix(const PMMATRIXD&);
	PMMATRIXD MakeViewMatrix(const PMVECTOR4D&, const PMVECTOR4D&, const PMVECTOR4D&);// eyePos,focusPos,WorldUpVec
	PMMATRIXD MakePerspectProjMatrix(const double, const double, const double, const double); // verticalFOV,aspectR,nearZ,farZ

	void TransposeMatrix(_Inout_ PMMATRIXD&);

	//-------------------------------------------------------------------------
	inline PMVECTOR4D::PMVECTOR4D(
		const double x,
		const double y,
		const double z,
		const double w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	inline PMVECTOR4D::PMVECTOR4D(DoubleX4 D4)
	{
		VECTOR4D = D4;
	}

	inline PMVECTOR4D& PMVECTOR4D::operator=(const PMVECTOR4D& other)
	{
		// TODO: 在此处插入 return 语句
		if (this == &other)return *this;

		this->VECTOR4D = other.VECTOR4D;
		return *this;
	}

	inline PMVECTOR4D PMVECTOR4D::operator+(const PMVECTOR4D& other)const
	{
		return PMVECTOR4D{ PM_ADD(VECTOR4D, other.VECTOR4D) };
	}

	inline PMVECTOR4D& PMVECTOR4D::operator+=(const PMVECTOR4D& other)
	{
		VECTOR4D = PM_ADD(VECTOR4D, other.VECTOR4D);
		return *this;
	}

	inline PMVECTOR4D PMVECTOR4D::operator-(const PMVECTOR4D& other)const
	{
		return PMVECTOR4D{ PM_SUB(VECTOR4D, other.VECTOR4D) };
	}

	inline PMVECTOR4D& PMVECTOR4D::operator-=(const PMVECTOR4D& other)
	{
		VECTOR4D = PM_SUB(VECTOR4D, other.VECTOR4D);
		return *this;
	}

	inline PMVECTOR4D PMVECTOR4D::operator*(const PMVECTOR4D& other)const
	{
		return PMVECTOR4D{ PM_MUL(VECTOR4D, other.VECTOR4D) };
	}

	inline PMVECTOR4D PMVECTOR4D::operator*(const double num)const
	{
		return PMVECTOR4D{ PM_MUL(VECTOR4D, PM_Replicate4D(num)) };
	}

	inline PMVECTOR4D PMVECTOR4D::operator*(const PMMATRIXD& matrix)const
	{
		PMMATRIXD tempMatrix{
			PMVECTOR4D{PM_MUL(VECTOR4D,matrix.ROW[0].VECTOR4D)},
			PMVECTOR4D{PM_MUL(VECTOR4D,matrix.ROW[1].VECTOR4D)},
			PMVECTOR4D{PM_MUL(VECTOR4D,matrix.ROW[2].VECTOR4D)},
			PMVECTOR4D{PM_MUL(VECTOR4D,matrix.ROW[3].VECTOR4D)}
		};
		DoubleX4 sum1 = PM_ADD(tempMatrix.ROW[0].VECTOR4D, tempMatrix.ROW[1].VECTOR4D);
		DoubleX4 sum2 = PM_ADD(tempMatrix.ROW[2].VECTOR4D, tempMatrix.ROW[3].VECTOR4D);

		return PMVECTOR4D{ PM_ADD(sum1,sum2) };
	}

	inline PMVECTOR4D& PMVECTOR4D::operator*=(const PMVECTOR4D& other)
	{
		VECTOR4D = PM_MUL(VECTOR4D, other.VECTOR4D);
		return *this;
	}

	inline PMVECTOR4D& PMVECTOR4D::operator*=(const double num)
	{
		VECTOR4D = PM_MULNUM(VECTOR4D,num);
		return *this;
	}

	inline PMVECTOR4D& PMVECTOR4D::operator*=(const PMMATRIXD& matrix)
	{
		PMMATRIXD tempMatrix{
			PMVECTOR4D{PM_MUL(VECTOR4D,matrix.ROW[0].VECTOR4D)},
			PMVECTOR4D{PM_MUL(VECTOR4D,matrix.ROW[1].VECTOR4D)},
			PMVECTOR4D{PM_MUL(VECTOR4D,matrix.ROW[2].VECTOR4D)},
			PMVECTOR4D{PM_MUL(VECTOR4D,matrix.ROW[3].VECTOR4D)}
		};

		DoubleX4 sum1 = PM_ADD(tempMatrix.ROW[0].VECTOR4D, tempMatrix.ROW[1].VECTOR4D);
		DoubleX4 sum2 = PM_ADD(tempMatrix.ROW[2].VECTOR4D, tempMatrix.ROW[3].VECTOR4D);

		VECTOR4D = PM_ADD(sum1, sum2);
		return *this;
	}

	inline PMVECTOR4D PMVECTOR4D::operator/(const PMVECTOR4D& other)const
	{
		PMVECTOR4D vec = PMVECTOR4D{ PM_DIV(VECTOR4D, other.VECTOR4D) };
		vec.w = 0.0;
		return vec;
	}

	inline PMVECTOR4D PMVECTOR4D::operator/(const double num)const
	{
		PMVECTOR4D vec = PMVECTOR4D{ PM_DIVNUM(VECTOR4D,num)};
		vec.w = 0.0;
		return vec;
	}

	inline PMVECTOR4D& PMVECTOR4D::operator/=(const PMVECTOR4D& other)
	{
		VECTOR4D = PM_DIV(VECTOR4D, other.VECTOR4D);
		w = 0.0;
		return *this;
	}

	inline PMVECTOR4D& PMVECTOR4D::operator/=(const double num)
	{
		VECTOR4D = PM_DIVNUM(VECTOR4D,num);
		w = 0.0;
		return *this;
	}

	PMVECTOR4D operator*(const double num, const PMVECTOR4D& vec)
	{
		return vec * num;
	}

	PMVECTOR4D operator/(const double num, const PMVECTOR4D& vec)
	{
		return vec / num;
	}

	inline PMMATRIXD::PMMATRIXD()
	{
		ROW[0] = PMVECTOR4D{ PM_Replicate4D(0.0) };
		ROW[1] = PMVECTOR4D{ PM_Replicate4D(0.0) };
		ROW[2] = PMVECTOR4D{ PM_Replicate4D(0.0) };
		ROW[3] = PMVECTOR4D{ PM_Replicate4D(0.0) };
	}

	inline PMMATRIXD::PMMATRIXD(
		double __11, double __12, double __13, double __14,
		double __21, double __22, double __23, double __24,
		double __31, double __32, double __33, double __34,
		double __41, double __42, double __43, double __44)
	{
		std::tie(
			_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44) =
			std::tie(
				__11, __12, __13, __14,
				__21, __22, __23, __24,
				__31, __32, __33, __34,
				__41, __42, __43, __44);
	}

	inline PMMATRIXD::PMMATRIXD(
		const PMVECTOR4D& v1,
		const PMVECTOR4D& v2,
		const PMVECTOR4D& v3,
		const PMVECTOR4D& v4,
		bool transform)
	{
		std::tie(ROW[0], ROW[1], ROW[2], ROW[3]) =
			std::tie(v1, v2, v3, v4);

		if (transform)
			TransposeMatrix(*this);
	}

	inline PMMATRIXD PMMATRIXD::operator*(const PMMATRIXD& next)
	{
		PMMATRIXD temp = *this;
		TransposeMatrix(temp);

		return PMMATRIXD{
			temp.ROW[0] * next,
			temp.ROW[1] * next,
			temp.ROW[2] * next,
			temp.ROW[3] * next
		};
	}

	inline PMMATRIXD PMMATRIXD::operator+(const PMMATRIXD& next)
	{
		PMMATRIXD result{
			ROW[0] + next.ROW[0],
			ROW[1] + next.ROW[1],
			ROW[2] + next.ROW[2],
			ROW[3] + next.ROW[3]
		};
		TransposeMatrix(result);
		return result;
	}

	inline PMMATRIXD PMMATRIXD::operator-(const PMMATRIXD& next)
	{
		PMMATRIXD result{
			ROW[0] - next.ROW[0],
			ROW[1] - next.ROW[1],
			ROW[2] - next.ROW[2],
			ROW[3] - next.ROW[3]
		};
		TransposeMatrix(result);
		return result;
	}

	inline PMVECTOR4D GetZeroVector4D()
	{
		return PMVECTOR4D{ PM_ZERO4D() };
	}

	inline PMVECTOR4D GetOneVector3D()
	{
		PMVECTOR4D vec = GetOneVector4D();
		vec.w = 0.0;
		return vec;
	}

	PMVECTOR4D GetZeroPoint3D()
	{
		PMVECTOR4D a_point = GetZeroVector4D();
		a_point.w = 1.0;
		return a_point;
	}

	inline PMVECTOR4D GetOneVector4D()
	{
		return PMVECTOR4D{ PM_ONE4D()};
	}

	PMVECTOR4D SetPoint3D(const double x, const double y, const double z)
	{
		return PMVECTOR4D{ x,y,z,1.0 };
	}

	inline double Length(const PMVECTOR4D& vec)
	{
		DoubleX4 square = PM_MUL(vec.VECTOR4D, vec.VECTOR4D);
		return sqrt(square.m256d_f64[0] + square.m256d_f64[1] + square.m256d_f64[2]);
	}

	inline PMVECTOR4D Normalize(const PMVECTOR4D& vec)
	{
		//only vector needs normalize,vector's w is always 0,while point's w is always 1
		return PMVECTOR4D{ PM_DIV(vec.VECTOR4D, PM_Replicate4D(Length(vec))) };
	}

	inline PMVECTOR4D Cross(const PMVECTOR4D& first, const PMVECTOR4D& last)
	{
		/*
			y1 z2-z1 y2,z1 x2-x1 z2,x1 y2- y1 x2

			y1 z2,z1 x2,x1 y2
			z1 y2,x1 z2,y1 x2

			y z x * z x y
			-
			z x y * y z x
		*/

		DoubleX4 f1 = PM_LOAD(first.y, first.z, first.x, 0.0);
		DoubleX4 f2 = PM_LOAD(first.z, first.x, first.y, 0.0);
		DoubleX4 l1 = PM_LOAD(last.z, last.x, last.y, 0.0);
		DoubleX4 l2 = PM_LOAD(last.y, last.z, last.x, 0.0);

		return PMVECTOR4D{ PM_SUB(PM_MUL(f1, l1), PM_MUL(f2, l2)) };
	}

	inline double Dot(const PMVECTOR4D& first, const PMVECTOR4D& last)
	{
		DoubleX4 dot = PM_MUL(first.VECTOR4D, last.VECTOR4D);
		return dot.m256d_f64[0] + dot.m256d_f64[1] + dot.m256d_f64[2];
	}

	inline PMMATRIXD MakeIdentityMatrix()
	{
		return PMMATRIXD{
			1.0,	0.0,	0.0,	0.0,
			0.0,	1.0,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0
		};
	}

	inline PMMATRIXD MakeScaleMatrix(const PMVECTOR4D& ScaleVec)
	{
		DoubleX4 Zero = PM_ZERO4D();
		return PMMATRIXD{
			PMVECTOR4D{PM_Blend_abbb(ScaleVec.VECTOR4D, Zero)},
			PMVECTOR4D{PM_Blend_babb(ScaleVec.VECTOR4D, Zero)},
			PMVECTOR4D{PM_Blend_bbab(ScaleVec.VECTOR4D, Zero)},
			GetOneVector4D(),
			false};
	}

	inline PMMATRIXD MakeInvScaleMatrix(const PMVECTOR4D& ScaleVec)
	{
		PMVECTOR4D InvScalVec = GetOneVector4D() / ScaleVec;
		DoubleX4 Zero = PM_ZERO4D();

		return PMMATRIXD{
			PMVECTOR4D{PM_Blend_abbb(InvScalVec.VECTOR4D, Zero)},
			PMVECTOR4D{PM_Blend_babb(InvScalVec.VECTOR4D, Zero)},
			PMVECTOR4D{PM_Blend_bbab(InvScalVec.VECTOR4D, Zero)},
			GetOneVector4D(),
			false };
	}

	inline PMMATRIXD MakeRotationMatrixX(const double radX)
	{
		double SIN = sin(radX);
		double COS = cos(radX);
		return PMMATRIXD{
			1.0,	0.0,	0.0,	0.0,
			0.0,	COS,	SIN,	0.0,
			0.0,	-SIN,	COS,	0.0,
			0.0,	0.0,	0.0,	1.0
		};
	}

	inline PMMATRIXD MakeInvRotationMatrixX(const double radX)
	{
		double SIN = sin(radX);
		double COS = cos(radX);
		return PMMATRIXD{
			1.0,	0.0,	0.0,	0.0,
			0.0,	COS,	-SIN,	0.0,
			0.0,	SIN,	COS,	0.0,
			0.0,	0.0,	0.0,	1.0
		};
	}

	inline double degreeToRadian(double deg)
	{
		return deg * PM_DegToRad;
	}

	inline double radianToDegree(double rad)
	{
		return rad * PM_RadToDeg;
	}

	inline PMMATRIXD MakeRotationMatrixY(const double radY)
	{
		double SIN = sin(radY);
		double COS = cos(radY);
		return PMMATRIXD{
			COS,	0.0,	-SIN,	0.0,
			0.0,	1.0,	0.0,	0.0,
			SIN,	0.0,	COS,	0.0,
			0.0,	0.0,	0.0,	1.0
		};
	}

	inline PMMATRIXD MakeInvRotationMatrixY(const double radY)
	{
		double SIN = sin(radY);
		double COS = cos(radY);
		return PMMATRIXD{
			COS,	0.0,	SIN,	0.0,
			0.0,	1.0,	0.0,	0.0,
			-SIN,	0.0,	COS,	0.0,
			0.0,	0.0,	0.0,	1.0
		};
	}

	inline PMMATRIXD MakeRotationMatrixZ(const double radZ)
	{
		double SIN = sin(radZ);
		double COS = cos(radZ);
		return PMMATRIXD{
			COS,	SIN,	0.0,	0.0,
			-SIN,	COS,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0
		};
	}

	inline PMMATRIXD MakeInvRotationMatrixZ(const double radZ)
	{
		double SIN = sin(radZ);
		double COS = cos(radZ);
		return PMMATRIXD{
			COS,	-SIN,	0.0,	0.0,
			SIN,	COS,	0.0,	0.0,
			0.0,	0.0,	1.0,	0.0,
			0.0,	0.0,	0.0,	1.0
		};
	}

	PMMATRIXD MakeScaleMatrix(const double num)
	{
		DoubleX4 N = PM_Replicate4D(num);
		DoubleX4 Zero = PM_ZERO4D();

		return PMMATRIXD{
		PMVECTOR4D{PM_Blend_abbb(N, Zero)},
		PMVECTOR4D{PM_Blend_babb(N, Zero)},
		PMVECTOR4D{PM_Blend_bbab(N, Zero)},
		GetOneVector4D(),
		false};
	}

	PMMATRIXD MakeRotationMatrix(const PMVECTOR4D& rotationVec, const double rad)
	{
		double SIN = sin(rad);
		double COS = cos(rad);
		PMMATRIXD C = MakeScaleMatrix(COS);

		PMMATRIXD R{
			PMVECTOR4D{0.0,				rotationVec.z,	-rotationVec.y,	0.0}*SIN,
			PMVECTOR4D{rotationVec.z,	0.0,			rotationVec.x,	0.0}*SIN,
			PMVECTOR4D{rotationVec.y,	rotationVec.x,	0.0,			0.0}*SIN,
			GetZeroVector4D()
		};

		PMMATRIXD XYZ{
			rotationVec * rotationVec.x * (1.0 - COS),
			rotationVec * rotationVec.y * (1.0 - COS),
			rotationVec * rotationVec.z * (1.0 - COS),
			GetZeroVector4D()
		};

		return C + R + XYZ;
	}

	PMMATRIXD MakeInvRotationMatrix(const PMVECTOR4D& rotationVec, const double rad)
	{
		PMMATRIXD Roatation = MakeRotationMatrix(rotationVec, rad);
		TransposeMatrix(Roatation);
		return Roatation;
	}

	inline PMMATRIXD MakeTranslationMatrix(const PMVECTOR4D& TranslationVec)
	{
		PMMATRIXD I = MakeIdentityMatrix();
		I.ROW[3] = TranslationVec;
		TransposeMatrix(I);
		return I;
	}

	inline void TransposeMatrix(_Inout_ PMMATRIXD& matrix)
	{
		auto&& [R1, R2, R3, R4] = matrix.ROW;//C++17 结构化绑定

		DoubleX4 t1 = PM_Swap_2301(R1.VECTOR4D);// 将第一行数据进行交换得到a2,a3,a0,a1
		DoubleX4 t2 = PM_Swap_2301(R2.VECTOR4D);// 将第二行数据进行交换得到b2,b3,b0,b1
		DoubleX4 t3 = PM_Swap_2301(R3.VECTOR4D);// 将第三行数据进行交换得到c2,c3,c0,c1
		DoubleX4 t4 = PM_Swap_2301(R4.VECTOR4D);// 将第四行数据进行交换得到d2,d3,d0,d1

		DoubleX4 t5 = PM_Blend_aabb(R1.VECTOR4D, t3);// a0,a1,c0,c1 合并原序列第一行和重排序列第三行得到
		DoubleX4 t6 = PM_Blend_aabb(R2.VECTOR4D, t4);// b0,b1,d0,d1
		DoubleX4 t7 = PM_Blend_aabb(t1,R3.VECTOR4D);// a2,a3,c2,c3
		DoubleX4 t8 = PM_Blend_aabb(t2,R4.VECTOR4D);// b2,b3,d2,d3

		R1 = PMVECTOR4D{ PM_Combine1133(t5, t6) };// a0,b0,c0,d0 生成转置子块，并写入对应位置
		R2 = PMVECTOR4D{ PM_Combine2244(t5, t6) };// a1,b1,c1,d1
		R3 = PMVECTOR4D{ PM_Combine1133(t7, t8) };// a2,b2,c2,d2
		R4 = PMVECTOR4D{ PM_Combine2244(t7, t8) };// a3,b3,c3,d3
	}

	inline PMMATRIXD GetTransposeMatrix(const PMMATRIXD& matrix)
	{
		PMMATRIXD target = matrix;
		TransposeMatrix(target);
		return target;
	}

	inline PMMATRIXD MakeViewMatrix(const PMVECTOR4D& EyePos, const PMVECTOR4D& FocusPos, const PMVECTOR4D& Up)
	{
		PMVECTOR4D w = Normalize(FocusPos - EyePos);// w = (T-Q)/(|T-Q|)
		PMVECTOR4D u = Normalize(Cross(Up, w));		// u = (jxw)/(|jxw|)
		PMVECTOR4D v = Cross(w, u);					// v = w x u,don't need normalize 

		PMVECTOR4D InvTranslate = -1.0 * EyePos;
		InvTranslate.w = 1.0;
		PMMATRIXD translate = MakeTranslationMatrix(InvTranslate);

		PMMATRIXD InvRotaion = PMMATRIXD{ u,v,w };  // 内部以转置存储所以无需操作
		return translate * InvRotaion;
	}

	PMMATRIXD MakePerspectProjMatrix(const double verticalFOV, const double aspectR, const double nearZ, const double farZ)
	{
		double COT = 1.0 / tan(verticalFOV / 2.0);// 1/tan(a/2)
		double A = farZ / (farZ - nearZ);
		double B = -nearZ * A;
		return PMMATRIXD{
			COT / aspectR,	0.0,	0.0,	0.0,
			0.0,			COT,	0.0,	0.0,
			0.0,			0.0,	A,		1.0,
			0.0,			0.0,	B,		0.0
		};
	}
}
#endif