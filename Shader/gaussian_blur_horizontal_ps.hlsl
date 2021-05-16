#include "bloom.hlsli"

Texture2D hdr_color_map : register(t0);
SamplerState linear_border_sampler_state : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
	uint mip_level = 0, width, height, number_of_levels;
    hdr_color_map.GetDimensions(mip_level, width, height, number_of_levels);
	const float aspect_ratio = width / height;

#if 1
	//http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	const float offset[3] = { 0.0, 1.3846153846, 3.2307692308 };
	const float weight[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };

    float4 fragment_color = hdr_color_map.Sample(linear_border_sampler_state, texcoord) * weight[0];
	for (int i = 1; i < 3; i++)
	{
        fragment_color += hdr_color_map.Sample(linear_border_sampler_state, texcoord + float2(offset[i] / width, 0.0)) * weight[i];
        fragment_color += hdr_color_map.Sample(linear_border_sampler_state, texcoord - float2(offset[i] / width, 0.0)) * weight[i];
    }
#else
	//https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms
	const float offset[2] = { 0.53805, 2.06278 };
	const float weight[2] = { 0.44908, 0.05092 };
	float4 fragment_color = 0;
	for (int i = 0; i < 2; i++)
	{
		fragment_color += hdr_color_map.Sample(linear_border_sampler_state, texcoord + float2(offset[i], 0.0) / width) * weight[i];
		fragment_color += hdr_color_map.Sample(linear_border_sampler_state, texcoord - float2(offset[i], 0.0) / width) * weight[i];
	}
#endif
    return fragment_color;
}
