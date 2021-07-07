// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_TANGENT
#define PIN_USE_BINORMAL
#define PIN_USE_COLOR
#define PIN_USE_TEX_POSITION  // ShadowMap比較用UV値
#define PIN_USE_TEXCOORD0     // 読み込んだテクスチャのUV値

#include "globals.hlsli"
#include "functions_lighting.hlsli"
#include "functions_utility.hlsli"
#include "../Source/Engine/metal.h"

SamplerState default_sampler : register(s0);

PS_Output main(PS_Input pin) : SV_TARGET
{
    PS_Output pout = (PS_Output) 0;
	
    const float3 eye_vector = light_position - pin.wvp_position.xyz;

    float3 L = light_direction.xyz; // Light Vector
    float3 N = normalize(pin.normal); // Normals (normalized)
    float3 H = normalize(-L + eye_vector); // Half Vector (Light vector and Eye vector)
    float3 E = normalize(eye_vector);

    //-- Each angle used in the calculation --//
    float NE = dot(N, E);
    float NH = dot(N, H);
    float EH = dot(E, H);
    float NL = dot(N, -L);
    float LH = dot(-L, H);

    // Beckmann distribution function value
    float D = Beckmann(metal_roughness, NH);

    // Geometric reduction ratio
    float G = min(1, min(2 * NH * NE / EH, 2 * NH * NL / EH));

    // Fresnel Equations
    float n = 20.0f; // 複素屈折率の実部
    float g = sqrt(n * n + LH * LH - 1);
    float gpc = g + LH;
    float gnc = g - LH;
    float cgpc = LH * gpc - 1;
    float cgnc = LH * gnc + 1;
    float F = 0.5f * gnc * gnc * (1 + cgpc * cgpc / (cgnc * cgnc)) / (gpc * gpc);

    float3 zero = float3(0, 0, 0);
    float4 f4_diffuse = pin.color * min(0.0f, float4(Diffuse(N, L, light_color.rgb, float3(1.0f, 1.0f, 1.0f)), 1.0f));
    f4_diffuse.w = 1.0f;

    const float4 normal_map = texture_normal.Sample(default_sampler, pin.texcoord0);
    float3 normal = CalcNormalFromTangentSpace(pin.tangent, pin.binormal, pin.normal, normal_map.xyz);

    pout.color      = f4_diffuse + float4(light_color, 1.0f) * max(0, F * D * G / NE);
    pout.normal     = float4(normal, 1.0f);
    pout.position   = pin.w_position;

    return pout;
}