#include "globals.hlsli"
#include "functions_utility.hlsli"
#include "../Source/Engine/cbuffer_post_effect.h"

SamplerState default_sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    const float4 screen_color   = texture_post_process.Sample(default_sampler, texcoord);
    const float2 ndc_pos        = UvToNDC(texcoord);

	// ‰æ–Ê’†‰›‚©‚ç‚Ì‹——£
    const float len             = length(ndc_pos);

    float4 final_color = (float4) 0;

    if (len < distort_radius)
    {
        final_color = screen_color;
    }
	else
	{
        const float2 scroll_time = float2(distort_elapsed_time, 0);

        float2 scroll_direction = distort_scroll_direction;
        if (IsEqual(scroll_direction.x, 0) &&
            IsEqual(scroll_direction.y, 0))
        {
			scroll_direction = normalize(distort_scroll_direction);
        }

        //const float2 uv_scroll_val = texcoord + (scroll_time + scroll_direction) * distort_time_scale;
        const float2 uv_scroll_val =(scroll_time + scroll_direction) * distort_time_scale;

        const float2 noise_uv = float2(PerlinNoise(uv_scroll_val, texcoord * distort_noise_scale) * distort_noise_attenuation, 0) + texcoord;
       // const float2 noise_uv = float2(PerlinNoise(uv_scroll_val, distort_noise_scale) * distort_noise_attenuation, 0) + texcoord;

        final_color = texture_post_process.Sample(default_sampler, noise_uv + uv_scroll_val);

	}

    return final_color;
}