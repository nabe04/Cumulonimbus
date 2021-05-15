#define USE_NDC_POSITION
#define USE_LOCAL_POSITION
#define USE_VIN_NORMAL
#define USE_BONE_WEIGHTS
#define USE_BONE_INDICES

#include "general.hlsli"
#include "shadow_map.hlsli"


VS_OutPut main(VS_Input vin)
{
    PS_Input vout = (PS_Input) 0;

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
    const float4 wvpPos = mul(float4(worldPos, 1.0f), light_orthographic_view_projection_matrix); // World coordinate transformation

    vout.position     = wvpPos;
    vout.ndc_position = wvpPos;

    return vout;
}