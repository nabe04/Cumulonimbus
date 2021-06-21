// PS_Input(VS_Output)
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_COLOR

#include "globals.hlsli"

SamplerState default_sampler : register(s0);

float4 main(PS_Input pin) : SV_TARGET
{
    float3 ref = pin.w_position.xyz - light_position.xyz;
    ref = reflect(ref, pin.normal);
    ref = normalize(ref);

    return texture_cube.Sample(default_sampler, ref);
}