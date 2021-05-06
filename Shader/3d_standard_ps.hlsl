#include "general.hlsli"
#include "3d_standard.hlsli"

//*******************************************
//  Txture
//*******************************************
Texture2D diffuseTexture : register(t0);
SamplerState defaultSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET0
{
    return diffuseTexture.Sample(defaultSampler, pin.texcoord) * pin.color;
}