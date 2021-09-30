#define VIN_USE_LOCAL_POSITION
#define VIN_USE_TEXCOORD0
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"

SamplerState default_sampler : register(s0);


float4 main(const PS_Input pin) : SV_TARGET
{
    float4 color = texture_base_color.Sample(default_sampler, pin.texcoord0);

    return color;
}