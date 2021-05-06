#include "general.hlsli"
#include "monochrome.hlsli"

Texture2D	 diffuse_texture : register(t0);
SamplerState diffuse_sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float3 monochrome_scale = 0;
    monochrome_scale.r = 0.298912;
    monochrome_scale.g = 0.586611;
    monochrome_scale.b = 0.114478;
    float4 fragment_color = diffuse_texture.Sample(diffuse_sampler, texcoord);

    float3 gray_color = dot(fragment_color.rgb, monochrome_scale);

    return float4(gray_color, 1);
}