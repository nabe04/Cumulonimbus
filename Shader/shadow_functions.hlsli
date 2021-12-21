
// シャドウマップからシャドウ空間座標に変換とZ値比較(分散シャドウマップ)
float3 GetVarianceShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
{
	// シャドウマップから深度を取り出す
    float d  = st.Sample(ss, Tex.xy).r;
    float d2 = st.Sample(ss, Tex.xy).g;

    // 分散の計算
    float variance = max(Bias, d2 - pow(d, 2));

	// 確率上の最大値の算出
    float p_max = saturate(variance / (variance + pow(Tex.z - d, 2)));

	// 影の色
    Scolor = lerp(Scolor, 1.0f, p_max);

    return p_max;// max(Tex.z < d, p_max);
}

// ワールド座標をライト座標空間に変換
float3 GetShadowTexture(const row_major float4x4 vp, const float3 w_pos)
{
    // 正規化デバイス座標(NDC座標)
    float4 wvp_pos;
    wvp_pos.xyz = w_pos;
    wvp_pos.w = 1;
    wvp_pos = mul(wvp_pos, vp);
    wvp_pos /= wvp_pos.w;

    // テクスチャ座標
    wvp_pos.y = -wvp_pos.y;
    wvp_pos.xy = 0.5f * wvp_pos.xy + 0.5f;

    return wvp_pos.xyz;
}

// シャードウマップからシャドウ空間座標に変換とZ値比較
float3 GetShadow(Texture2D depth_tex, SamplerState ss, float3 obj_tex_pos, float3 shadow_color, float bias)
{
    // https://drive.google.com/file/d/14lz9TR7dmzqntr9LPXsxgceH7OfmXonG/view?usp=sharing
    // シャドウマップから深度を取り出す
    float d = depth_tex.Sample(ss, obj_tex_pos.xy).r;

    // シャドウマップの深度値とObjectの深度値の比較
    shadow_color = (obj_tex_pos.z - d > bias) ? shadow_color : 1.0f;

    // 影の強さが分かる
    return shadow_color;
}
