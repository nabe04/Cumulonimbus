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
 * brief : float��epsilon�̒l���l���������l�֐�
 */
inline bool IsEqual(float l_value,float r_value)
{
    const int l_val = floor(l_value);
    const int r_val = floor(r_value);

	// epsilon == 1.192092896e-07F
    return (l_val - r_val) <= 1.192092896e-07F ? true : false;
}

#endif // FUNCTIONS_UTILITY_SHF