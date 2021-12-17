// PS_Input(VS_Output)
#define PIN_USE_W_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_TANGENT
#define PIN_USE_BINORMAL
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"
#include "functions.hlsli"
#include "functions_utility.hlsli"
#include "../Source/Engine/material.h"

SamplerState default_sampler : register(s0);

PS_Output main(PS_Input pin)
{
    PS_Output pout = (PS_Output) 0;

	// 接空間軸
    const float3 vx = normalize(pin.tangent);
    const float3 vy = normalize(pin.binormal);
    const float3 vz = normalize(pin.normal);

	// 接空間行列
    const float3x3 v_mat = { { vx }, { vy }, { vz } };

    float3 normal = texture_normal.Sample(default_sampler, pin.texcoord0).xyz;
    normal = normal * 2.0f - 1.0f;

    // 法線マップからワールドへ変換
    normal = normalize(mul(normal, v_mat));

    const float4 color    = texture_base_color.Sample(default_sampler, pin.texcoord0);
    const float4 position = pin.w_position;

	// ラフネス値の取得
    float roughness = GetSingleTextureChannel(mat_use_roughness_channel, texture_roughness, default_sampler, pin.texcoord0);
	// メタルネス値の取得
    float metalness = GetSingleTextureChannel(mat_use_metalness_channel, texture_metalness, default_sampler, pin.texcoord0);
    // オクリュージョン値の取得 mat_use_occlusion_channel
    float occlusion = GetSingleTextureChannel(mat_use_occlusion_channel, texture_occlusion, default_sampler, pin.texcoord0);

    pout.color          = color;
    pout.mro = position;
    pout.normal         = float4(normal, 1.0f);

	return pout;
}