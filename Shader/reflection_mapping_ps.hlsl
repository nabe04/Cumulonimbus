#define USE_LOCAL_POSITION
#define USE_BONE_WEIGHTS
#define USE_BONE_INDICES
#define USE_VOUT_NORMAL
#define USE_TEXCOORD0
#define USE_WORLD_POSITION

#include "general.hlsli"

TextureCube texture_cube : register(t2);
SamplerState default_sampler : register(s0);

float4 main(PS_Input pin) : SV_TARGET
{
    float3 ref = pin.world_position.xyz - light_position.xyz;
    ref = reflect(ref, pin.normal);
    ref = normalize(ref);

    return texture_cube.Sample(default_sampler, ref);
}