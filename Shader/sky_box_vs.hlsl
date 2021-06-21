// VS_Input
#define VIN_USE_LOCAL_POSITION
#define VIN_USE_NORMAL
#define VIN_USE_TEXCOORD0
#define VIN_USE_BONE_WEIGHTS
#define VIN_USE_BONE_INDICES
// PS_Input(VS_Output)
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL

#include "globals.hlsli"

PS_Input main(VS_Input vin)
{
    PS_Input vout = (PS_Input) 0;

    float3 worldPos = { 0, 0, 0 };
    float3 normal = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
		// boneWeight[0] == boneWeight.x
        // 多分元の頂点にボーンオフセット行列を掛けて位置を算出している
        // それぞれのウェイトの影響度を加算
        worldPos += (vin.bone_weights[i] * mul(bone_transforms[vin.bone_indices[i]], vin.position)).xyz;
        normal   += (vin.bone_weights[i] * mul(bone_transforms[vin.bone_indices[i]], float4(vin.normal, 0))).xyz;
    }

    const float4 wvp_pos = mul(camera_view_projection_matrix, float4(worldPos, 0.0f)); // World coordinate transformation

    vout.position   = wvp_pos;
    vout.position.z = vout.position.w;
    vout.w_position = float4(worldPos, 1.0f);
    vout.normal     = normal;

    return vout;
}