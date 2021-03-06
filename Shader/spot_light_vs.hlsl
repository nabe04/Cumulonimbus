#include "general.hlsli"
#include "spot_light.hlsli"

VS_OUT main(VS_IN vin)
{
    VS_OUT vout = (VS_OUT) 0;

    float3 worldPos = { 0, 0, 0 };
    float3 normal = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
		// boneWeight[0] == boneWeight.x
        worldPos += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        normal += (vin.boneWeights[i] * mul(float4(vin.normal, 0), boneTransforms[vin.boneIndices[i]])).xyz;
    }

    float4 wvpPos = mul(float4(worldPos, 1.0f), viewProjection); // World coordinate transformation

    vout.wvpPos         = wvpPos;
    vout.worldPos       = float4(worldPos, 1.f);
    vout.normal         = vin.normal;
    vout.texcoord       = vin.texcoord;
    vout.color          = m_Color;
    vout.eyeVector = light_position.xyz - vout.wvpPos.xyz;

    return vout;
}