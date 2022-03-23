//此文件储存贝塞尔方程的计算函数
//外壳着色器的点,可自定义
struct HullOut
{
	float3 PosL : POSITION;
};
//--------------------------------------------------
float2 BernsteinBasis1(float t)//这是一阶贝塞尔曲线
{
    float invT = 1.0f - t;	//1-t
    return float2( invT,
                     t );
}

float2 dBernsteinBasis1(float t)//这是一阶贝塞尔曲线求导
{
	return float2(-1.0f,1.0f);//d(1-t)/dt,dt/dt
}

//---------------------------------------------------
float3 BernsteinBasis2(float t)//这是二阶贝塞尔曲线
{
    float invT = 1.0f - t;	//1-t

    return float3( invT * invT,		//B 02 (1-t)^2
                   2.0f * t * invT,	//B 12 2t(1-t)
                      t * t);		//B 22 t^2
}

float3 dBernsteinBasis2(float t)//这是二阶贝塞尔曲线求导
{
    float invT = 1.0f - t;

    return float3( -2 * invT,
                    2 * invT - 2 * t,
                    2 * t );
}

//---------------------------------------------------
float4 BernsteinBasis3(float t)//这是三阶贝塞尔曲线
{
    float invT = 1.0f - t;

    return float4( invT * invT * invT,
                   3.0f * t * invT * invT,
                   3.0f * t * t * invT,
                      t * t * t );
}

float4 dBernsteinBasis3(float t)//这是三阶贝塞尔曲线求导
{
    float invT = 1.0f - t;

    return float4( -3 * invT * invT,
                    3 * invT * invT - 6 * t * invT,
                    6 *    t * invT - 3 * t * t,
                    3 *    t * t );
}
//---------------------------------------------------
float3 CubicBezierSumQuad3x3(const OutputPatch<HullOut, 9> bezpatch, float3 basisU, float3 basisV)
{
    float3 sum = float3(0.0f, 0.0f, 0.0f);
    sum  = basisV.x * (basisU.x*bezpatch[0].PosL  + basisU.y*bezpatch[1].PosL  + basisU.z*bezpatch[2].PosL);
    sum += basisV.y * (basisU.x*bezpatch[3].PosL  + basisU.y*bezpatch[4].PosL  + basisU.z*bezpatch[5].PosL);
    sum += basisV.z * (basisU.x*bezpatch[6].PosL  + basisU.y*bezpatch[7].PosL  + basisU.z*bezpatch[8].PosL);

    return sum;
}

float3 CubicBezierSumQuad4x4(const OutputPatch<HullOut, 16> bezpatch, float4 basisU, float4 basisV)
{
    float3 sum = float3(0.0f, 0.0f, 0.0f);
    sum  = basisV.x * (basisU.x*bezpatch[0].PosL  + basisU.y*bezpatch[1].PosL  + basisU.z*bezpatch[2].PosL  + basisU.w*bezpatch[3].PosL );
    sum += basisV.y * (basisU.x*bezpatch[4].PosL  + basisU.y*bezpatch[5].PosL  + basisU.z*bezpatch[6].PosL  + basisU.w*bezpatch[7].PosL );
    sum += basisV.z * (basisU.x*bezpatch[8].PosL  + basisU.y*bezpatch[9].PosL  + basisU.z*bezpatch[10].PosL + basisU.w*bezpatch[11].PosL);
    sum += basisV.w * (basisU.x*bezpatch[12].PosL + basisU.y*bezpatch[13].PosL + basisU.z*bezpatch[14].PosL + basisU.w*bezpatch[15].PosL);

    return sum;
}
//---------------------------------------------------
float3 CubicBezierSumTri3x3(const OutputPatch<HullOut, 6> bezpatch, float3 basisU3,float2 basisU2, float3 basisV)
{
    float3 sum = float3(0.0f, 0.0f, 0.0f);
    sum  = basisV.x * (basisU3.x*bezpatch[0].PosL  + basisU3.y*bezpatch[1].PosL  + basisU3.z*bezpatch[2].PosL);
    sum += basisV.y * (basisU2.x*bezpatch[3].PosL  + basisU2.y*bezpatch[4].PosL);
    sum += basisV.z * (   1.0f  *bezpatch[5].PosL);

    return sum;
}

float3 CubicBezierSumTri4x4(const OutputPatch<HullOut, 10> bezpatch, float4 basisU4,float3 basisU3,float2 basisU2, float4 basisV)
{
    float3 sum = float3(0.0f, 0.0f, 0.0f);
    sum  = basisV.x * (basisU4.x*bezpatch[0].PosL  + basisU4.y*bezpatch[1].PosL  + basisU4.z*bezpatch[2].PosL  + basisU4.w*bezpatch[3].PosL );
    sum += basisV.y * (basisU3.x*bezpatch[4].PosL  + basisU3.y*bezpatch[5].PosL  + basisU3.z*bezpatch[6].PosL);
    sum += basisV.z * (basisU2.x*bezpatch[7].PosL  + basisU2.y*bezpatch[8].PosL);
    sum += basisV.w * (   1.0f  *bezpatch[9].PosL);

    return sum;
}