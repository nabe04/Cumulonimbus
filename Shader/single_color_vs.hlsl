#include "general.hlsli"
#include "single_color.hlsli"

VS_OUT main( VS_IN vin )
{
    VS_OUT vout = (VS_OUT) 0;

    float3 worldPos = { 0, 0, 0 };
    float3 normal = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
		// boneWeight[0] == boneWeight.x
        // 多分元の頂点にボーンオフセット行列を掛けて位置を算出している
        // それぞれのウェイトの影響度を加算
        worldPos += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        normal += (vin.boneWeights[i] * mul(float4(vin.normal, 0), boneTransforms[vin.boneIndices[i]])).xyz;
    }

    float4 wvpPos = mul(float4(worldPos, 1.0f), viewProjection); // World coordinate transformation

    vout.position   = wvpPos;
    vout.normal     = normalize(normal);
    vout.texcoord   = vin.texcoord;
    vout.color      = m_Color;

    return vout;
}