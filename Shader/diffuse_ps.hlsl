// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_COLOR
#define PIN_USE_TEX_POSITION   // ShadowMap比較用UV値
#define PIN_USE_TEXCOORD0      // 読み込んだテクスチャのUV値

#include "globals.hlsli"
#include "functions.hlsli"
#include "diffuse.hlsli"
#include "shadow_functions.hlsli"

//*******************************************
//  Texture
//*******************************************
TEXTURE2D(texture_diffuse, float4, TexSlot_OnDemand0);
SamplerState default_sampler : register(s0);

float4 main(PS_Input pin) : SV_TARGET
{
   const  float3 reflectance = float3(1.0f, 1.0f, 1.0f);

    float3 f3_diffuse = Diffuse(pin.normal.xyz, light_direction.xyz, pin.color.xyz, reflectance);

    float3 shadow_color = GetVarianceShadow(texture_depth, default_sampler, pin.tex_position, 0.6f, 0.000001f);

    return texture_diffuse.Sample(default_sampler, pin.texcoord0) * float4(f3_diffuse, 1) * float4(shadow_color, 1);
}