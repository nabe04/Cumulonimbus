Texture2D convolution_texture : register(t0);
Texture2D backbuffer_texture  : register(t1);

SamplerState default_sampler  : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    float3 backbuffer_color = 0;
    float3 convolution_color = 0;

    backbuffer_color  = backbuffer_texture.Sample(default_sampler, texcoord).xyz;
    convolution_color = convolution_texture.Sample(default_sampler, texcoord).xyz;

    return float4( backbuffer_color + convolution_color, 1);
}