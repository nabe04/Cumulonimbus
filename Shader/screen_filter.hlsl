#include "globals.hlsli"
#include "functions_utility.hlsli"
#include "../Source/Engine/cbuffer_post_effect.h"

SamplerState default_sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    const float4 screen_color   = texture_post_process.Sample(default_sampler, texcoord);
    const float4 dissolve_color = texture_dissolve.Sample(default_sampler, texcoord);
    const float threshold       = ToGrayColor(dissolve_color.rgb);

	float4 final_color = screen_color;

    if (threshold < OneMinus(sfilter_dissolve_threshold))
	{// ディゾルブする前の色を返す
        if (sfilter_is_grey_scale >= 1)
        {
            final_color = ToGrayColor(screen_color.rgb);
        }
        else
        {
            final_color.rgb += sfilter_filter_color;
        }
    }


    return float4(final_color.rgb, 1.0f);
}