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
#include "functions_utility.hlsli"
#include "functions_lighting.hlsli"
#include "shadow_functions.hlsli"

SamplerState default_sampler : register(s0);

PS_Output main(PS_Input pin)
{
    PS_Output pout = (PS_Output) 0;

    const float3 reflectance = float3(1.0f, 1.0f, 1.0f);

    float3 f3_diffuse = Diffuse(pin.normal.xyz, light_direction.xyz, pin.color.xyz, reflectance);

    const float4 normal_map = texture_normal.Sample(default_sampler, pin.texcoord0);
    float3 normal = CalcNormalFromTangentSpace(pin.tangent, pin.binormal, pin.normal, normal_map.xyz);

    float3 shadow_color = GetVarianceShadow(texture_depth, default_sampler, pin.tex_position, 0.6f, 0.000001f);

    float3 tex_color = texture_base_color.Sample(default_sampler, pin.texcoord0).xyz;
    tex_color = CalcGammaCorrection(tex_color, 2.2f);

    pout.color      = float4(tex_color, 1.0f) * float4(f3_diffuse, 1) * float4(shadow_color, 1);
    pout.normal     = float4(normal, 1.0f);
    pout.mro        = pin.w_position;

    return pout;
}