#include "general.hlsli"
#include "phong.hlsli"
#include "functions.hlsli"

//*******************************************
//  Txture
//*******************************************
Texture2D diffuseTexture : register(t0);
SamplerState defaultSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color        = diffuseTexture.Sample(defaultSampler, pin.texcoord);
    float3 N            = normalize(pin.normal);    // Normal
    float3 E            = normalize(pin.eyeVector); // Eye vector
    float3 L            = normalize(lightDirection.xyz);  // Light direction
    float3 reflectance  = { 1.0f, 1.0f, 1.0f };

    float3 A = m_Ka.xyz;

    float3 D = Diffuse(N, L, pin.color.xyz, reflectance);

    float3 F = PhongSpecular(N, L, E, specular, refCoefficient, powValue);

    color *= pin.color * float4(A + D + F, 1.0f);
    return color;
}