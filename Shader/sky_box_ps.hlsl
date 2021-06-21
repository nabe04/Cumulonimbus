// PS_Input(VS_Output)
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL

#include "globals.hlsli"

SamplerState default_sampler : register(s0);

float4 main(PS_Input pin) : SV_TARGET
{
    float3 view_vec = pin.w_position.xyz;
    float3 ref = reflect(view_vec, pin.normal);
    ref = normalize(ref);

    return texture_cube.Sample(default_sampler, ref);
}