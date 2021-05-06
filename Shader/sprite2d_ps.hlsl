#include "sprite2d.hlsli"

//*******************************************
//  Texture
//*******************************************
Texture2D    diffuseTexture : register(t0);
SamplerState defaultSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = diffuseTexture.Sample(defaultSampler, pin.texcoord) * pin.color;

    return color * m_Color;
}