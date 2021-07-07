// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_TANGENT
#define PIN_USE_BINORMAL
#define PIN_USE_COLOR
#define PIN_USE_TEXCOORD0     // 読み込んだテクスチャのUV値

#include "globals.hlsli"
#include "functions_utility.hlsli"
#include "functions_lighting.hlsli"
#include "../Source/Engine/phong.h"

SamplerState default_sampler : register(s0);

PS_Output main(PS_Input pin) : SV_TARGET
{
    PS_Output pout = (PS_Output) 0;

    float3 eye_vector = light_position.xyz - pin.wvp_position.xyz;

    float4 color = texture_base_color.Sample(default_sampler, pin.texcoord0);
    float3 N = normalize(pin.normal); // Normal
    float3 E = normalize(eye_vector); // Eye vector
    float3 L = normalize(light_direction.xyz); // Light direction
    float3 reflectance = { 1.0f, 1.0f, 1.0f };

    float3 A = light_color;

    float3 D = Diffuse(N, L, pin.color.xyz, reflectance);

    float3 F = PhongSpecular(N, L, E, phong_specular, phong_ref_coefficient, phong_pow_value);

    color *= pin.color * float4(A + D + F, 1.0f);

    const float4 normal_map = texture_normal.Sample(default_sampler, pin.texcoord0);
    float3 normal = CalcNormalFromTangentSpace(pin.tangent, pin.binormal, pin.normal, normal_map.xyz);

    pout.color      = color;
    pout.normal     = float4(normal, 1.0f);
    pout.position   = pin.w_position;

    return pout;
}