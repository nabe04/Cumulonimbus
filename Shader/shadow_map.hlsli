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

// ワールド座標をライト座標空間に変換
float3 GetShadowTexture(float4x4 vp, float3 w_pos)
{
    // 正規化デバイス座標(NDC座標)
    float4 wvp_pos;
    wvp_pos.xyz = w_pos;
    wvp_pos.w = 1;
    wvp_pos = mul(vp, wvp_pos);
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
