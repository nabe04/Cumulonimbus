struct VS_IN
{
    float4 position    : POSITION;
    float3 normal      : NORMAL;
    float2 texcoord    : TEXCOORD;
    float4 boneWeights : WEIGHTS;
    uint4  boneIndices : BONES;
};

struct PS_IN
{
    float4 position     : SV_POSITION;
    float4 ndc_wvp_position : NDC_WVP_POSITION;
    float3 texture_position : NDC_WORLD_POSITION;
    float3 normal       : NORMAL;
};

// ���[���h���W�����C�g���W��Ԃɕϊ�
float3 GetShadowTexture(float4x4 vp, float3 w_pos)
{
    // ���K���f�o�C�X���W(NDC���W)
    float4 wvp_pos;
    wvp_pos.xyz = w_pos;
    wvp_pos.w = 1;
    wvp_pos = mul(vp, wvp_pos);
    wvp_pos /= wvp_pos.w;

    // �e�N�X�`�����W
    wvp_pos.y = -wvp_pos.y;
    wvp_pos.xy = 0.5f * wvp_pos.xy + 0.5f;

    return wvp_pos.xyz;
}

// �V���[�h�E�}�b�v����V���h�E��ԍ��W�ɕϊ���Z�l��r
float3 GetShadow(Texture2D depth_tex, SamplerState ss, float3 obj_tex_pos, float3 shadow_color, float bias)
{
    // https://drive.google.com/file/d/14lz9TR7dmzqntr9LPXsxgceH7OfmXonG/view?usp=sharing
    // �V���h�E�}�b�v����[�x�����o��
    float d = depth_tex.Sample(ss, obj_tex_pos.xy).r;

    // �V���h�E�}�b�v�̐[�x�l��Object�̐[�x�l�̔�r
    shadow_color = (obj_tex_pos.z - d > bias) ? shadow_color : 1.0f;

    // �e�̋�����������
    return shadow_color;
}
