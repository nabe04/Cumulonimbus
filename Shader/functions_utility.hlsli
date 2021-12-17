#ifndef FUNCTIONS_UTILITY_SHF
#define FUNCTIONS_UTILITY_SHF

/*
 * brief : GBuffer���PS_Output::shader_slot�̐F���̌v�Z
 *         R8_UNORM�e�N�X�`���Ȃ̂ŃX���b�g�ԍ���0�`1��
 *         256�K���ɕϊ�����
 * slot  : CB_ShaderSlot::shader_slot�̒l
 */
inline float CalcGBuffShaderSlotVal(int slot)
{
	// 0.00392156f : 1 / 255
    return float(slot) * 0.00392156f;
}

/*
 * brief     : �K���}�␳�̌v�Z
 * color     : �K���}�␳�����������F(�e�N�X�`��)
 * gamma_val : �␳�����������ݏ�l
 */
inline float3 CalcGammaCorrection(float3 color,float gamma_val)
{
    return pow(abs(color), 1.0f / gamma_val);
}

/*
 * brier     : �f�K���}�␳�̌v�Z
 * color     : �K���}�␳�����������F(�e�N�X�`��)
 * gamma_val : �␳�����������ݏ�l
 */
inline float3 CalcDeGammaCorrection(float3 color,float de_gamma_val)
{
    return pow(abs(color), de_gamma_val);
}

/*
 * brief : float��epsilon�̒l���l���������l�֐�
 */
inline bool IsEqual(float l_value,float r_value)
{
    const int l_val = floor(l_value);
    const int r_val = floor(r_value);

	// epsilon == 1.192092896e-07F
    return (l_val - r_val) <= 1.192092896e-07F ? true : false;
}

/*
 * brief      : �ڋ��(�^���W�F���g�X�y�[�X)�ƃm�[�}���}�b�v����@�����Z�o
 * normal_map : �m�[�}���}�b�v�e�N�X�`��
 */
inline float3 CalcNormalFromTangentSpace(const float3 tangent,const float3 binormal,const float3 normal,float3 normal_map)
{
	// �ڋ�Ԏ�
    const float3 vx = normalize(tangent);
    const float3 vy = normalize(binormal);
    const float3 vz = normalize(normal);

	// �ڋ�ԍs��
    const float3x3 mat = { { vx }, { vy }, { vz } };
    normal_map = normal_map * 2.0f - 1.0f;

	// �@���}�b�v���烏�[���h�֕ϊ�
    return normal_map = normalize(mul(normal_map, mat));
}

/**
 * @brief : use_channel���1�ȏ�̒l�̃e�N�X�`���`�����l���l���擾����
 * @param use_channel : �g�p�������`�����l��(�g�p������������1�ȏ���i�[����)
 * @param tex : �擾����e�N�X�`��
 * @param ss : �T���v���[
 * @param uv_pos : UV���W
 */
inline float GetSingleTextureChannel(const float4 use_channel,const Texture2D tex, const SamplerState ss,const float2 uv_pos)
{
    const float4 col = tex.Sample(ss, uv_pos);

    if (use_channel.x >= 1.0f)
        return col.r;
    if (use_channel.y >= 1.0f)
        return col.g;
    if (use_channel.z >= 1.0f)
        return col.b;

    return col.a;
}

#endif // FUNCTIONS_UTILITY_SHF