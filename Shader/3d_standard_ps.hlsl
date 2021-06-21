#define PIN_USE_COLOR
#define PIN_USE_TEXCOORD0
#define PIN_USE_NORMAL

#include "globals.hlsli"

//*******************************************
//  Txture
//*******************************************
SamplerState defaultSampler : register(s0);

float4 main(PS_Input pin) : SV_TARGET0
{
    return texture_base_color.Sample(defaultSampler, pin.texcoord0) * pin.color;
}