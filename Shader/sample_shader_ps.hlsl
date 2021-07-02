// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_COLOR
#define PIN_USE_TEX_POSITION   // ShadowMap比較用UV値
#define PIN_USE_TEXCOORD0      // 読み込んだテクスチャのUV値

#include "globals.hlsli"
#include "functions.hlsli"
#include "shadow_functions.hlsli"

//*******************************************
//  Texture
//*******************************************
SamplerState default_sampler : register(s0);

float4 main(PS_Input pin) : SV_TARGET
{
    float4 albedo_color = texture_base_color.Sample(default_sampler, pin.texcoord0);

    //return normal_map_c;
    return albedo_color;
}