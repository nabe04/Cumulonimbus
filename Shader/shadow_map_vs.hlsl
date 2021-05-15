#include "general.hlsli"
#include "shadow_map.hlsli"

PS_IN main(VS_IN vin)
{
    PS_IN vout = (PS_IN) 0;

    float3 worldPos = { 0, 0, 0 };
    float3 normal = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
		// boneWeight[0] == boneWeight.x
        // 多分元の頂点にボーンオフセット行列を掛けて位置を算出している
        // それぞれのウェイトの影響度を加算
        worldPos += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        normal   += (vin.boneWeights[i] * mul(float4(vin.normal, 0), boneTransforms[vin.boneIndices[i]])).xyz;
    }
    float4 wvpPos = mul(float4(worldPos, 1.0f), viewProjection); // World coordinate transformation

    vout.position           = wvpPos;
    vout.ndc_wvp_position   = wvpPos;
    vout.texture_position = GetShadowTexture(light_orthographic_projection_matrix, worldPos);

    vout.normal = normalize(normal);

    return vout;
}