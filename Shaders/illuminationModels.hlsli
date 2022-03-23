#include"LitMatStruct.hlsli"
#define PI 3.14159265359f
/*
	目前最好的ToneMapping算法，Academy Color Encoding System（ACES）
	是Hdr到Ldr的映射算法，普通的显示器只能显示8bit,也就是颜色要限制在[0.0,1.0] * 255中
	要保留画面细节，就是把颜色映射到[0,1]，并且考虑如何映射能满足实际需求
*/
float3 ACESToneMapping(float3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

/*	
	菲涅尔方程，用于计算反射
	石里克给出了菲涅耳反射率的近似值
	R0 = ( (n1-n2)/(n1+n2) )^2, 其中n1,n2分别是光在两种材质的折射率，
	比如空气的折射率约等于1，则式子可以简化为R0 = ( (n-1)/(n+1) )^2
*/
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

	return reflectPercent;
}

/*
	线性衰减因子的计算方法
		distance：距离光源的距离
		falloffStart：开始衰减的距离（未到达这个距离前保持最大强度）
		falloffEnd：大于这个距离将不会受到光照（即衰减因子为0）
	Return：衰减因子=（end -distance）/(end-start)
*/
float CalcAttenuation(float distance, float falloffStart, float falloffEnd)
{
	/* 线性衰减 */
	return saturate((falloffEnd - distance) / (falloffEnd - falloffStart));
}

/*	
	Blinn-Phong光照模型
	Phong和Blinn-Phong是计算镜面反射光的两种光照模型，两者仅有很小的不同之处

	phong模型计算反射向量:dot(V,R) shininess
	入射光 L，表面法向量 N，反射向量 R
	已知	R = L + V
	又有	V = 2*N   * (-N * L)//N是单位向量，所以可以算出L在-N方向的分量
	则有	R = L + (2*N   * (-N * L)) = L - 2*N*(N*L)
	//==============================================
	Phong模型中计算反射光线的向量是一件相对比较耗时的任务，
	因此Blinn-Phong对这一点进行了改进。
	
	表面法向量 N，光入射方向L 和视点方向V 的中间向量 H
	用该式计算镜面反射光符合基本规律，当视点方向和反射光线方向一致时，
	计算得到的H与N平行 dot(N,H)取得最大；当视点方向V偏离反射方向时，H也偏离N
	同时H的计算比起反射向量R的计算简单的多，H的计算仅仅需要一次加法

	一般地，使用Blinn-Phong模型时要得到相同强度的镜面光，镜面系数需要为Phong模型的2-4倍
	例如Phong模型的镜面高光系数设置为8，可以设置Blinn-Phong模型的系数为32
*/

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	/* 粗糙度因子里的m值 */
	const float m = mat.Shininess * 256.0f;
	/* 半角向量,视线和光照方向(与实际光线方向相反)的中间向量 */
	float3 halfVec = normalize(toEye + lightVec);
	//8/8->256+8 / 8
	float specularEffect = (m + 8.0f)/8;
	float roughnessFactor = specularEffect * pow(max(dot(halfVec, normal), 0.0f), m);

	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);
	/* 镜面反射反照率=菲涅尔因子*粗糙度因子 */
	float3 specAlbedo = fresnelFactor * roughnessFactor;

	/* 
		光照可以超过[0,1]范围
		LDR：specAlbedo = specAlbedo / (specAlbedo + 1.0f);
		return(漫反射+高光反射)=(总的反照率)*入射光量
	*/
	return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

float3 Phong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	const float m = mat.Shininess * 256.0f;

	float3 halfVec = normalize(toEye + lightVec);
 	/* 此时需要光线方向为由光源指出 */
	float3 reflectVec  = normalize(reflect(-lightVec, normal));
	//36/36->256+36/36
	float specularEffect = (m + 36.0f)/36;
	float roughnessFactor = specularEffect*pow(max(dot(reflectVec, normal), 0.0f), m);

	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, normal, lightVec);
	float3 specAlbedo = fresnelFactor * roughnessFactor;

	/* 
		光照可以超过[0,1]范围
		LDR：specAlbedo = specAlbedo / (specAlbedo + 1.0f);
	*/
	return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

/*	
	Cook-Torrance 反射模型BRDF
	Rbd = Kd*Rd + Ks*Rs
	其中Kd+Ks = 1.0，Rd，Rs与材质相关,Rd 是f lambert Rs 是f Cook-Torrance
	金属：对于金属来说所有的折射光都会被吸收，所以完美的金属漫反射 Rd = 0 只有镜面反射 Rs
	Rd = 物体固有色/PI

	计算镜面反射的公式 Rs = (D(h)F(l,h)G(l,v,h)) / (4*(n.l)(n.v))
	其中
	D：微平面在平面上的分布函数
	G：计算微平面由于互相遮挡而产生的衰减
	F：菲涅尔项
	D、G两项都用到了 粗糙度roughness
*/
/*	==========================
	微表面分布函数D
	假设表面的宏观法向量为n,这个函数计算所有微平面中法向量与宏观法向量n一致的微平面的比例
	
	Trowbridge-Reitz GGX 模型：
	D(h) = (a^2) / (PI( (n.h)^2 * (a^2 -1) + 1)^2)
	其中：n为表面的宏观法向量，h入射光和观察方向的中间向量即半角向量，a为粗糙度参数
*/
//float3 normal, float3 halfVec, float roughness
float DistributionGGX(float3 N, float3 H, float a)
{     
	float a2     = a*a;     
	float NdotH  = max(dot(N, H), 0.0f);     
	float NdotH2 = NdotH*NdotH;      
    
	float denom  = (NdotH2 * (a2 - 1.0f) + 1.0f);     

	return a2 / (PI * denom * denom);
}
/*	==========================
	G：几何遮挡因子（Geometric Occlusion Factor）
	
	微平面理论认为，由于微平面的不规则排列，反射光有一定比率会被物体自身的微平面遮挡而衰减，这就是 G 项
	用Schlick 模型结合 Smith 模型计算此项，公式为：
	
	k = (a + 1)^2/8
	
	G1(v) = (n.v) / ((n.v)(1-k)+k)
	
	G(l,v,h)= G1(1) * G1(v)
*/
inline float GeometrySchlickGGX(float NdotV, float k)
{
    float denom = NdotV * (1.0f - k) + k;

    return NdotV / denom;
}

float Geometry_Smith_SchlickGGX(float3 N, float3 V, float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggxv = GeometrySchlickGGX(NdotV, k);
    float ggxl = GeometrySchlickGGX(NdotL, k);

    return ggxv * ggxl;
}
/*	==========================
	金属以外的物体表面都会具有菲涅尔效果
	也就是上面的SchlickFresnel
*/
/* 这是UE4的菲涅尔算法,看用户是否采用 */
float3 Fresnel_UE4(float3 toEye,float3 halfVec, float3 F0)
{
	return F0 + (float3(1, 1, 1) - F0) * pow(2, ((-5.55473f) * dot(toEye,halfVec) - 6.98316f) * dot(toEye, halfVec));
}

float3 CookTorranceBRDF(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, AdvancedMaterial mat)
{
	float3 halfVec = normalize(toEye + lightVec);

	float NDF = DistributionGGX(normal, halfVec, mat.roughness);       
	float G   = Geometry_Smith_SchlickGGX(normal, toEye, lightVec, mat.roughness);
	float3 F = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

	float3 nominator    = NDF * G * F;
	/* 加0.001为了避免出现除零错误 */
	float denominator = 4.0f * max(dot(normal, toEye), 0.0f) * max(dot(normal, lightVec), 0.0f) + 0.001f;
	float3 specular     = nominator / denominator;

	// ===========
	/* 表示光能中被反射的能量的比例 */
	float3 kS = F;
	/* 被折射的，因为金属不会折射光线，因此不会有漫反射 Kd会变为0 */
	float3 kD = float3(1.0f,1.0f,1.0f) - kS;
	/* 设置金属度 */
	kD *= 1.0f - mat.metallic;


	float NdotL = max(dot(normal, lightVec), 0.0f);
	return (kD * mat.DiffuseAlbedo.xyz / PI + specular) * lightStrength * NdotL;
}
