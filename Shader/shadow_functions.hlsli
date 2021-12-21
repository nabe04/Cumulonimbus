
// �V���h�E�}�b�v����V���h�E��ԍ��W�ɕϊ���Z�l��r(���U�V���h�E�}�b�v)
float3 GetVarianceShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
{
	// �V���h�E�}�b�v����[�x�����o��
    float d  = st.Sample(ss, Tex.xy).r;
    float d2 = st.Sample(ss, Tex.xy).g;

    // ���U�̌v�Z
    float variance = max(Bias, d2 - pow(d, 2));

	// �m����̍ő�l�̎Z�o
    float p_max = saturate(variance / (variance + pow(Tex.z - d, 2)));

	// �e�̐F
    Scolor = lerp(Scolor, 1.0f, p_max);

    return p_max;// max(Tex.z < d, p_max);
}

// ���[���h���W�����C�g���W��Ԃɕϊ�
float3 GetShadowTexture(const row_major float4x4 vp, const float3 w_pos)
{
    // ���K���f�o�C�X���W(NDC���W)
    float4 wvp_pos;
    wvp_pos.xyz = w_pos;
    wvp_pos.w = 1;
    wvp_pos = mul(wvp_pos, vp);
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
