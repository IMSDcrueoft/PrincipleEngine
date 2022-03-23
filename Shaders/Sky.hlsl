// Include common HLSL code.
#include "Common.hlsli"

TextureCube gCubeMap : register(t0);

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// 使用局部顶点位置作为立方体贴图查找向量
	vout.PosL = vin.PosL;
	
	// 转换到世界空间
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	// 总是以相机为中心的天空
	posW.xyz += gEyePosW;

	// 设置 z = w 使得 z/w = 1（即，天穹总是在远平面上）
	vout.PosH = mul(posW, gViewProj).xyww;
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gCubeMap.Sample(gsamLinearWrap, pin.PosL);
}

