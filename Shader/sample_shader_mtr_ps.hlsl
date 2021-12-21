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

    float4 albedo_color = texture_base_color.Sample(default_sampler, pin.texcoord0);

    const float4 normal_map = texture_normal.Sample(default_sampler, pin.texcoord0);
    float3 normal = CalcNormalFromTangentSpace(pin.tangent, pin.binormal, pin.normal, normal_map.xyz);

    pout.color    = albedo_color;
    pout.normal   = float4(normal, 1.0f);
    pout.mro      = pin.w_position;

    return pout;
}