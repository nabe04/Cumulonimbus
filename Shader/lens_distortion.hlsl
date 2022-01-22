#include "globals.hlsli"
#include "functions_utility.hlsli"

SamplerState default_sampler : register(s0);
float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    const float4 screen_color = texture_post_process.Sample(default_sampler, texcoord);
    const float2 ndc_pos = UvToNDC(texcoord);

    const float aberration = pow(((length(ndc_pos))), 2.0f);

    const float3 lens_blur =  texture_post_process.Sample(default_sampler, texcoord - ndc_pos * aberration * 0.00625).rgb;
    const float3 lens_blur2 = texture_post_process.Sample(default_sampler, texcoord - ndc_pos * aberration * 0.0125).rgb;
    const float3 lens_blur3 = texture_post_process.Sample(default_sampler, texcoord - ndc_pos * aberration * 0.025).rgb;

	//float3 composite_color = lerp()

    const float gray_color = ToGrayColor(screen_color.rgb + lens_blur + lens_blur2) / 3.0f;

    float3 desaturate_color = gray_color;
    desaturate_color += float3(ToGrayColor(screen_color.rgb - lens_blur3).r * 2.0f, float2(0, 0));

    return float4(lerp(screen_color.rgb, desaturate_color, 0.5f), 1.0f);
}