// VS_Input
#define VIN_USE_LOCAL_POSITION
#define VIN_USE_NORMAL
#define VIN_USE_TEXCOORD0
#define VIN_USE_BONE_WEIGHTS
#define VIN_USE_BONE_INDICES
// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_WV_POSITION

#include "globals.hlsli"

VS_OutPut main(VS_Input vin)
{
    VS_OutPut vs_out = (VS_OutPut) 0;

  float3 world_pos = { 0, 0, 0 };
  float3 normal = { 0, 0, 0 };
  for (int i = 0; i < 4; i++)
  {
	////// boneWeight[0] == boneWeight.x
      // 多分元の頂点にボーンオフセット行列を掛けて位置を算出している
      // それぞれのウェイトの影響度を加算
      world_pos += (vin.bone_weights[i] * mul(bone_transforms[vin.bone_indices[i]], vin.position)).xyz;
      normal    += (vin.bone_weights[i] * mul(bone_transforms[vin.bone_indices[i]], float4(vin.normal, 0))).xyz;
  }

   const float4  wvp_pos = mul(light_orthographic_view_projection_matrix, float4(world_pos, 1.0f)); // World coordinate transformation
   const float4 wv_pos  = mul(light_view_matrix                        , float4(world_pos, 1.0f)); // ビュー空間までの変換(分散シャドウマップの比較などに使用)

   vs_out.position     = wvp_pos;
   vs_out.wvp_position = wvp_pos;
   vs_out.wv_position  = wv_pos;

    return vs_out;
}