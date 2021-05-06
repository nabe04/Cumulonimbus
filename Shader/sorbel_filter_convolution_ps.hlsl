Texture2D monochrome_texture			: register(t0);
Texture2D outline_extraction_texture	: register(t1);

SamplerState diffuse_sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float4 monochrome           = monochrome_texture.Sample(diffuse_sampler, texcoord);
    float4 outline_extraction   = outline_extraction_texture.Sample(diffuse_sampler, texcoord);

    float3 fragment_color = outline_extraction.rgb - monochrome.rgb;

    return float4(fragment_color, 1.0f);
    return float4(1 - fragment_color, 1.0f);
}