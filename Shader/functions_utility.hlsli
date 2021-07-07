#ifndef FUNCTIONS_UTILITY_SHF
#define FUNCTIONS_UTILITY_SHF

/*
 * brief : GBuffer上のPS_Output::shader_slotの色情報の計算
 *         R8_UNORMテクスチャなのでスロット番号を0～1の
 *         256階調に変換する
 * slot  : CB_ShaderSlot::shader_slotの値
 */
inline float CalcGBuffShaderSlotVal(int slot)
{
	// 0.00392156f : 1 / 255
    return float(slot) * 0.00392156f;
}

/*
 * brief : floatのepsilonの値を考慮した同値関数
 */
inline bool IsEqual(float l_value,float r_value)
{
    const int l_val = floor(l_value);
    const int r_val = floor(r_value);

	// epsilon == 1.192092896e-07F
    return (l_val - r_val) <= 1.192092896e-07F ? true : false;
}

#endif // FUNCTIONS_UTILITY_SHF