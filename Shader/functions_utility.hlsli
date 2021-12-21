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

/**
 * @brief : use_channel上で1以上の値のテクスチャチャンネル値を取得する。
 *          すべてのチャンネルが1未満の場合spare_param(第5引数)が使用される
 * @param use_channel : 使用したいチャンネル(使用したい成分に1以上を格納する)
 * @param tex : 取得するテクスチャ
 * @param ss : サンプラー
 * @param uv_pos : UV座標
 * @param spare_param : use_channelの値がすべて1未満の場合使用される値
 */
inline float GetSingleTextureChannel(
    const float4 use_channel, const Texture2D tex,
    const SamplerState ss,const float2 uv_pos,
    const float spare_param)
{
    const float4 col = tex.Sample(ss, uv_pos);
	// 返す色(早期リターンをした場合警告が出たため)
    float ret_col = 0.0f;

    if (use_channel.x >= 1.0f)
        ret_col = col.r;
    else if (use_channel.y >= 1.0f)
        ret_col = col.g;
    else if (use_channel.z >= 1.0f)
        ret_col = col.b;
    else if(use_channel.w >= 1.0f)
        ret_col = col.a;
	else
        ret_col = spare_param;

    return ret_col;
}

/**
 * @brief : 深度テクスチャからワールド空間上の座標を算出
 * @param texcoord : テスクチャ座標
 * @param depth : 深度値
 * @param inv_view_projection : ビュー・プロジェクション行列の逆行列
 */
inline float3 DepthToWPosition(
	float2 texcoord,
	const float depth,
	const float4x4 inv_view_projection)
{
	// 深度値からプロジェクション座標での位置を算出
    texcoord.y = -texcoord.y;
    float2 pos_xy = texcoord.xy * 2.0f - float2(1.0f, -1.0f);
    const float4 projected_position = float4(pos_xy, depth, 1.0f);

    const float4 position = mul(inv_view_projection, projected_position);

    return position.xyz / position.w;
}

#endif // FUNCTIONS_UTILITY_SHF