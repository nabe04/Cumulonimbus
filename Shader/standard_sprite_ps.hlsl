#define USE_LOCAL_POSITION
#define USE_TEXCOORD0

#include "general.hlsli"

Texture2D    default_texture : register(t0);
SamplerState default_sampler : register(s0);


float4 main(PS_Input pin) : SV_TARGET
{
    float4 color = default_texture.Sample(default_sampler, pin.texcoord0);

    return color;
}