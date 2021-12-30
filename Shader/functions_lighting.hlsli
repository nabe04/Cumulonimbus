#ifndef FUNCTIONS_LIGHTING_SHF
#define FUNCTIONS_LIGHTING_SHF

#include "macros.hlsli"
#include "params_utility.hlsli"

//--------------------------------------
// Diffuse reflecton function
//--------------------------------------
// N : 法線(正規化済み)
// L : 入射ベクトル(正規化済み)
// C : 入射光(色・強さ)
// K : 反射率
//---------------------------------------
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L);
    D = max(0, D);
    return K * C * D;
}

//--------------------------------------------
//	鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝度)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//ハーフベクトル
    float3 H = normalize(-L + E);

    float3 S = dot(H, N);
    S = max(0, S);
    S = pow(S, Power);
    S = S * K * C;
    return S;
}

//-------------------------------------
//  Phong's specular reflection function
//-------------------------------------
// N     : Normals (normalized)
// L     : Incident vector (normalized)
// E     : Eye vector
// S     : Strength of reflected light in the mirror
// K     : reflection coefficient
// power : power
//--------------------------------------
float3 PhongSpecular(float3 N, float3 L, float3 E, float S, float K, float power)
{
    float3 R = -E + 2 * dot(N, E) * N;

    float3 D = max(0, dot(-L, R));

    return S * K * pow(D, power);
}



//***********************************************
//
//  Distribution function
//
//***********************************************

//---------------------------------------------
// Beckmann distribution function
//---------------------------------------------
// m  : Roughness
// NH : dot (Normal * Harf vector)
//---------------------------------------------
float Beckmann(float m, float NH)
{
    float NH2 = NH * NH;

    return exp(-(1 - NH2) / (NH2 * m * m)) / (4 * m * m * NH2 * NH2);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float NdfGGX(const float cos_Lh, const float roughness)
{
    const float alpha = roughness * roughness;
    const float alpha_sq = alpha * alpha;

    const float denom = (cos_Lh * cos_Lh) * (alpha_sq - 1.0) + 1.0;
    return alpha_sq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float GaSchlickG1(const float cos_theta, const float k)
{
    return cos_theta / (cos_theta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float GaSchlickGGX(const float cos_li, const float cos_lo, const float roughness)
{
    const float r = roughness + 1.0;
    const float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return GaSchlickG1(cos_li, k) * GaSchlickG1(cos_lo, k);
}

// Shlick's approximation of the Fresnel factor.
float3 FresnelSchlick(const float3 F0, const float cos_theta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}

// Returns number of mipmap levels for specular IBL environment map.
uint QuerySpecularTextureLevels(const TextureCube specular_tex)
{
    uint width, height, levels;
    specular_tex.GetDimensions(0, width, height, levels);
    return levels;
}

#endif // FUNCTIONS_LIGHTING_SHF