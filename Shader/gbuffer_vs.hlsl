// VS_Input
#define VIN_USE_LOCAL_POSITION
#define VIN_USE_NORMAL
#define VIN_USE_TEXCOORD0
#define VIN_USE_BONE_WEIGHTS
#define VIN_USE_BONE_INDICES
// PS_Input(VS_Output)
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_TANGENT
#define PIN_USE_BINORMAL
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"

VS_OutPut main(VS_Input vin)
{
    VS_OutPut vout = (VS_OutPut) 0;

    float3 world_pos = { 0, 0, 0 };
    float3 normal    = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
		// boneWeight[0] == boneWeight.x
        // 多分元の頂点にボーンオフセット行列を掛けて位置を算出している
        // それぞれのウェイトの影響度を加算
        world_pos += (vin.bone_weights[i] * mul(bone_transforms[vin.bone_indices[i]], vin.position)).xyz;
        normal    += (vin.bone_weights[i] * mul(bone_transforms[vin.bone_indices[i]], float4(vin.normal, 0))).xyz;
    }

    const float4 wvp_pos = mul(camera_view_projection_matrix, float4(world_pos, 1.0f)); // World coordinate transformation

    vout.position   = wvp_pos;
    vout.w_position = float4(world_pos, 1.0f);
    vout.normal     = normal;
    vout.texcoord0  = vin.texcoord0;

	 // 接空間行列
    float3 vy = { 0.0f, 1.0f, 0.001f }; // 仮ベクトル
    float3 vz = normal;
    float3 vx = { 0.0f, 0.0f, 0.0f };
    vx = cross(vy, vz);
    vx = normalize(vx);
    vy = cross(vz, vx);
    vy = normalize(vy);
    vout.tangent  = vx;
	vout.binormal = vy;

	return vout;
}