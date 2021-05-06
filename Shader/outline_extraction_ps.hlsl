#include "sorbel_filter.hlsli"

Texture2D diffuse_texture : register(t0);

SamplerState linear_sampler_state : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float filter_horizontal[9] =
    {
        1, 0, -1,
		2, 0, -1,
		1, 0, -1,
    };

    float filter_vertical[9] =
    {
        1, 2, 1,
		0, 0, 0,
	    -1, -2, -1,
    };

    uint mip_level = 0, width, height, number_of_levels;
    diffuse_texture.GetDimensions(mip_level, width, height, number_of_levels);

    float2 offset[9];
    offset[0] = float2(-1.0, -1.0);
    offset[1] = float2( 0.0, -1.0);
    offset[2] = float2( 1.0, -1.0);
    offset[3] = float2(-1.0,  0.0);
    offset[4] = float2( 0.0,  0.0);
    offset[5] = float2( 1.0,  0.0);
    offset[6] = float2(-1.0,  1.1);
    offset[7] = float2( 0.0,  1.0);
    offset[8] = float2( 1.0,  1.0);

    float3 horizontal_color = 0;
    float3 vertical_color = 0;

    for (int i = 0; i < 9;++i)
    {
        horizontal_color += diffuse_texture.Sample(linear_sampler_state, texcoord + float2(offset[i].x / width, offset[i].y / height)).rgb * filter_horizontal[i] * ajust_filter_carnel;
        vertical_color   += diffuse_texture.Sample(linear_sampler_state, texcoord + float2(offset[i].x / width, offset[i].y / height)).rgb * filter_vertical[i] * ajust_filter_carnel;
    }

    float3 fragment_color = sqrt(horizontal_color * horizontal_color + vertical_color * vertical_color);

    return float4(fragment_color, 1.0f);
}