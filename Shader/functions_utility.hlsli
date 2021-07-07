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
 * brief     : ガンマ補正の計算
 * color     : ガンマ補正をかけたい色(テクスチャ)
 * gamma_val : 補正をかけたい累乗値
 */
inline float3 CalcGammaCorrection(float3 color,float gamma_val)
{
    return pow(abs(color), 1.0f / gamma_val);
}

/*
 * brier     : デガンマ補正の計算
 * color     : ガンマ補正をかけたい色(テクスチャ)
 * gamma_val : 補正をかけたい累乗値
 */
inline float3 CalcDeGammaCorrection(float3 color,float de_gamma_val)
{
    return pow(abs(color), de_gamma_val);
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

/*
 * brief      : 接空間(タンジェントスペース)とノーマルマップから法線を算出
 * normal_map : ノーマルマップテクスチャ
 */
inline float3 CalcNormalFromTangentSpace(const float3 tangent,const float3 binormal,const float3 normal,float3 normal_map)
{
	// 接空間軸
    const float3 vx = normalize(tangent);
    const float3 vy = normalize(binormal);
    const float3 vz = normalize(normal);

	// 接空間行列
    const float3x3 mat = { { vx }, { vy }, { vz } };
    normal_map = normal_map * 2.0f - 1.0f;

	// 法線マップからワールドへ変換
    return normal_map = normalize(mul(normal_map, mat));
}

#endif // FUNCTIONS_UTILITY_SHF