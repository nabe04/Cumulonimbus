#include "general.hlsli"
#include "shadow_map.hlsli"

//*******************************************
//  Txture
//*******************************************
Texture2D depth_texture : register(t0);
SamplerState linear_sampler : register(s0);
float4 main(PS_IN pin) : SV_TARGET
{
    float bias = 0.001f;
    float3 shadow_color;
    shadow_color.rgb = 0.6f;

    return float4(GetShadow(depth_texture, linear_sampler, pin.texture_position, shadow_color, bias).zzz, 1);
}