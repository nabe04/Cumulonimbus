#define PIN_USE_TEXCOORD0

#include "globals.hlsli"
#include "lighting_pbr.hlsli"
#include "functions_utility.hlsli"
#include "functions_lighting.hlsli"
#include "../Source/Engine/shader_asset_mapping.h"
#include "../Source/Engine/cbuffer_lights.h"

SamplerState defaultSampler : register(s0);
float4 main(const PS_Input pin) : SV_TARGET
{
	// GBufferで書き出したテクスチャの色算出
    float4 albedo      = texture_base_color.Sample(defaultSampler, pin.texcoord0);
    const float4 normal_map  = texture_normal.Sample(defaultSampler, pin.texcoord0);
    const float metalness    = texture_mro.Sample(defaultSampler, pin.texcoord0).r;
    const float roughness    = texture_mro.Sample(defaultSampler, pin.texcoord0).g;
    const float occlusion    = texture_mro.Sample(defaultSampler, pin.texcoord0).b;
    const float depth        = texture_depth.Sample(defaultSampler, pin.texcoord0).r;

    BRDFData brdf_data = (BRDFData) 0;
    float alpha = albedo.a;

    InitializeBRDFData(albedo.rgb, metalness, roughness, alpha, brdf_data);

    BRDFData brdf_data_clear_coat = (BRDFData) 0;
    half4 shadow_mask = half4(1, 1, 1, 1);

    const Light main_light = GetMainLight();
    float3 main_light_direction = d_light_direction;

    const float3 pow_ws = DepthToWPosition(pin.texcoord0, depth, camera_inv_view_projection_matrix);
    const float3 normal_ws = normalize(2.0 * normal_map.rgb - 1.0);
    const float3 view_direction_ws = camera_position - pow_ws;

	// メインのPBR処理
    float3 color = LightingPhysicallyBased(brdf_data, brdf_data_clear_coat,
                                           main_light,
                                           normal_ws, view_direction_ws,
                                           float(1.0), true);

    return float4(color, albedo.a);
}