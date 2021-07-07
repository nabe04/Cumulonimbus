#define PIN_USE_TEXCOORD0

#include "globals.hlsli"
#include "functions_utility.hlsli"
#include "functions_lighting.hlsli"
#include "../Source/Engine/shader_asset_mapping.h"

SamplerState defaultSampler : register(s0);
float4 main(PS_Input pin) : SV_TARGET
{
	// GBufferで書き出したテクスチャの色算出
    float4    albedo_color = texture_base_color.Sample(defaultSampler, pin.texcoord0);
    const float4    normal_map   = texture_normal.Sample(defaultSampler, pin.texcoord0);
    const float4    position_map = texture_position.Sample(defaultSampler, pin.texcoord0);
    const int shader_slot  = texture_shader_slot.Sample(defaultSampler, pin.texcoord0);

    // eye_vec   : position_mapからカメラへのベクトル
    const float3 eye_vec   = normalize(position_map.xyz - camera_position);
    // light_vec : ライトの向きベクトル(法線方向に合わせていない)
    const float3 light_vec = normalize(light_direction);

    float3 diffuse = Diffuse(normal_map.xyz, light_vec, light_color, 1.0f);

    float3 phong_specular = BlinnPhongSpecular(normal_map.xyz , light_vec,
											   light_color.xyz, eye_vec,
											   float3(1.0f, 1.0f, 1.0f), 5);

    albedo_color *= float4(diffuse + phong_specular + light_color, 1.0f);

    return albedo_color;
}