#define PIN_USE_TEXCOORD0

#include "globals.hlsli"
#include "functions_utility.hlsli"
#include "functions_lighting.hlsli"
#include "../Source/Engine/shader_asset_mapping.h"

   // // eye_vec   : position_mapからカメラへのベクトル
   // const float3 eye_vec = normalize(w_pos - camera_position);
   // // light_vec : ライトの向きベクトル(法線方向に合わせていない)
   // const float3 light_vec = normalize(light_direction);

   // //float3 diffuse = Diffuse(normal_map.xyz, light_vec, light_color, 1.0f);

   // //float3 phong_specular = BlinnPhongSpecular(normal_map.xyz , light_vec,
			//	//							   light_color.xyz, eye_vec,
			//	//							   float3(1.0f, 1.0f, 1.0f), 5);

   //// albedo_color *= float4(diffuse + phong_specular + light_color, 1.0f);

   // albedo *= float4(light_color, 1.0f);

    //return albedo_color;

SamplerState defaultSampler : register(s0);
float4 main(const PS_Input pin) : SV_TARGET
{
	// GBufferで書き出したテクスチャの色算出
    const float4 albedo       = texture_base_color.Sample(defaultSampler, pin.texcoord0);
    const float4 normal_map   = texture_normal.Sample(defaultSampler, pin.texcoord0);
    const float  metalness    = texture_mro.Sample(defaultSampler, pin.texcoord0).r;
    const float  roughness    = texture_mro.Sample(defaultSampler, pin.texcoord0).g;
    const float  occlusion    = texture_mro.Sample(defaultSampler, pin.texcoord0).b;
    const float  depth        = texture_depth.Sample(defaultSampler, pin.texcoord0).r;

    const float3 w_pos = DepthToWPosition(pin.texcoord0, depth, camera_inv_view_projection_matrix);

    // Outgoing light direction (vector from world-space fragment position to the "eye").
    const float3 Lo = normalize(camera_position - w_pos);

    // Angle between surface normal and outgoing light direction.
	// Get current fragment's normal and transform to world space.
    const float3 N = normalize(2.0 * normal_map.rgb - 1.0);
    const float cos_lo = max(0.0, dot(N, Lo));

	// Specular reflection vector.
    const float3 Lr = 2.0 * cos_lo * N - Lo;

    // Fresnel reflectance at normal incidence (for metals use albedo color).
    const float3 F0 = lerp(Fdielectric, albedo.xyz, metalness);

    // Direct lighting calculation for analytical lights.
    float3 direct_lighting = 0.0;

    const float3 Li = -light_direction;
    //float3 Lradiance = lights[i].radiance;
    const float3 Lradiance = (float3) 1;

	// Half-vector between Li and Lo.
    const float3 Lh = normalize(Li + Lo);

	// Calculate angles between surface normal and various light vectors.
    const float cos_li = max(0.0, dot(N, Li));
    const float cos_lh = max(0.0, dot(N, Lh));

	// Calculate Fresnel term for direct lighting.
    const float3 F = FresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
	// Calculate normal distribution for specular BRDF.
    const float D  = NdfGGX(cos_lh, roughness);
	// Calculate geometric attenuation for specular BRDF.
    const float G  = GaSchlickGGX(cos_li, cos_lo, roughness);

    // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
	// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
	// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    const float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);

	// Lambert diffuse BRDF.
	// We don't scale by 1/PI for lighting & material units to be more convenient.
	// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    const float3 diffuse_BRDF = kd * albedo;

	// Cook-Torrance specular microfacet BRDF.
    const float3 specular_BRDF = (F * D * G) / max(Epsilon, 4.0 * cos_li * cos_lo);

	// Total contribution for this light.
    direct_lighting += (diffuse_BRDF + specular_BRDF) * Lradiance * cos_li;

    return float4(direct_lighting, albedo.a);
}