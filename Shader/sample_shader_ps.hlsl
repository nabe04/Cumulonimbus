// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_COLOR
#define PIN_USE_TEX_POSITION   // ShadowMap��r�pUV�l
#define PIN_USE_TEXCOORD0      // �ǂݍ��񂾃e�N�X�`����UV�l

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