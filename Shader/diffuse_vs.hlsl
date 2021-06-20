// VS_Input
#define VIN_USE_LOCAL_POSITION
#define VIN_USE_NORMAL
#define VIN_USE_TEXCOORD0
#define VIN_USE_BONE_WEIGHTS
#define VIN_USE_BONE_INDICES
// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_COLOR
#define PIN_USE_TEX_POSITION  // ShadowMap��r�pUV�l
#define PIN_USE_TEXCOORD0     // �ǂݍ��񂾃e�N�X�`����UV�l

#include "globals.hlsli"
#include "diffuse.hlsli"
#include "shadow_map.hlsli"

VS_OutPut main(VS_Input vin)
{
    VS_OutPut vout = (VS_OutPut) 0;

    float3 world_pos = { 0, 0, 0 };
    float3 normal   = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
		// boneWeight[0] == boneWeight.x
        // �������̒��_�Ƀ{�[���I�t�Z�b�g�s����|���Ĉʒu���Z�o���Ă���
        // ���ꂼ��̃E�F�C�g�̉e���x�����Z
        world_pos += (vin.bone_weights[i] * mul(bone_transforms[vin.bone_indices[i]], vin.position)).xyz;
        normal    += (vin.bone_weights[i] * mul(bone_transforms[vin.bone_indices[i]], float4(vin.normal, 0))).xyz;
    }

    const float4 wvp_pos = mul(camera_view_projection_matrix, float4(world_pos, 1.0f)); // World coordinate transformation

    vout.position       = wvp_pos;
    vout.wvp_position   = wvp_pos;
    vout.normal         = normalize(normal);
    vout.color          = material.base_color;
    vout.tex_position   = GetShadowTexture(light_orthographic_view_projection_matrix, world_pos);
    vout.texcoord0      = vin.texcoord0;
    return vout;
}