// For VS input
#define USE_LOCAL_POSITION
#define USE_VIN_NORMAL
#define USE_BONE_WEIGHTS
#define USE_BONE_INDICES
// For VS output
#define USE_NDC_POSITION

#include "general.hlsli"
#include "shadow_map.hlsli"


VS_OutPut main(VS_Input vin)
{
    VS_OutPut vs_out = (VS_OutPut) 0;

    float3 world_pos = { 0, 0, 0 };
    float3 normal = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
		// boneWeight[0] == boneWeight.x
        // 多分元の頂点にボーンオフセット行列を掛けて位置を算出している
        // それぞれのウェイトの影響度を加算
        world_pos += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        normal += (vin.boneWeights[i] * mul(float4(vin.normal, 0), boneTransforms[vin.boneIndices[i]])).xyz;
    }
    const float4 wvp_pos = mul(float4(world_pos, 1.0f), light_orthographic_view_projection_matrix); // World coordinate transformation

    vs_out.position     = wvp_pos;
    vs_out.ndc_position = wvp_pos;

    return vs_out;
}