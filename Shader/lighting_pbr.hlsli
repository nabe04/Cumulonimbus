#include "macros.hlsli"
#include "functions_utility.hlsli"
#include "../Source/Engine/shader_interop_renderer.h"
#include "../Source/Engine/cbuffer_lights.h"

/////////////////////////////////////////////
//      Helper functions for smoothness    //
/////////////////////////////////////////////

/**
 * @return : (1.0 - perceptual_roughness);
 */
float PerceptualRoughnessToSmoothness(const float perceptual_roughness)
{
    return (1.0 - perceptual_roughness);
}

/////////////////////////////////////////////
//      Helper functions for roughness     //
/////////////////////////////////////////////

/**
 * @return : perceptual_roughness * perceptual_roughness
 */
float PerceptualRoughnessToRoughness(const float perceptual_roughness)
{
    return perceptual_roughness * perceptual_roughness;
}

/**
 * @return : sqrt(roughness);
 */
float RoughnessToPerceptualRoughness(const float roughness)
{
    return sqrt(roughness);
}

/**
 * @return : 1.0 - sqrt(roughness);
 */
float RoughnessToPerceptualSmoothness(const float roughness)
{
    return 1.0 - sqrt(roughness);
}

/**
 * @return : (1.0 - perceptual_smoothness) * (1.0 - perceptual_smoothness);
 */
float PerceptualSmoothnessToRoughness(const float perceptual_smoothness)
{
    return (1.0 - perceptual_smoothness) * (1.0 - perceptual_smoothness);
}

/**
 * @return : (1.0 - perceptual_smoothness);
 */
float PerceptualSmoothnessToPerceptualRoughness(const float perceptual_smoothness)
{
    return (1.0 - perceptual_smoothness);
}

/////////////////////////////////////////////
//            Light Functions              //
/////////////////////////////////////////////

// Abstraction over Light shading data.
struct Light
{
    float3 direction;
    float3 color;
    float  distance_attenuation;
    float  shadow_attenuation;
};

Light GetMainLight()
{
    Light light = (Light)0;
    light.direction             = d_light_direction;
    light.distance_attenuation  = 1.0f;
    light.shadow_attenuation    = 1.0f;
    light.color                 = d_light_color;

    return light;
}

/////////////////////////////////////////////
//            BRDF Functions               //
/////////////////////////////////////////////


#define kDielectricSpec half4(0.04, 0.04, 0.04, 1.0 - 0.04) // standard dielectric reflectivity coef at incident angle (= 4%)

struct BRDFData
{
    float3 diffuse;
    float3 specular;
    float reflectivity;
    float perceptual_roughness;
    float roughness;
    float roughness2;
    float grazing_term;
    float normalization_term; // roughness * 4.0 + 2.0
    float roughness2_minus_one; // roughness^2 - 1.0
};

float OneMinusReflectivityMetallic(const float metallic)
{
    // We'll need oneMinusReflectivity, so
    //   1-reflectivity = 1-lerp(dielectricSpec, 1, metallic) = lerp(1-dielectricSpec, 0, metallic)
    // store (1-dielectricSpec) in kDielectricSpec.a, then
    //   1-reflectivity = lerp(alpha, 0, metallic) = alpha + metallic*(0 - alpha) =
    //                  = alpha - metallic * alpha
    const float one_minus_dielectric_spec = kDielectricSpec.a;
    return one_minus_dielectric_spec - metallic * one_minus_dielectric_spec;
}

void InitializeBRDFDataDirect(
        const float3 diffuse, const float3 specular ,
		const float reflectivity,
        const float one_minus_reflectivity,
		const float roughness,
        inout float alpha,
		out BRDFData out_brdf_data)
{
    out_brdf_data.diffuse               = diffuse;
    out_brdf_data.specular              = specular;
    out_brdf_data.reflectivity          = reflectivity;
    out_brdf_data.perceptual_roughness  = roughness;
    out_brdf_data.roughness             = max(PerceptualRoughnessToRoughness(out_brdf_data.perceptual_roughness), HALF_MIN_SQRT);
    out_brdf_data.roughness2            = max(out_brdf_data.roughness * out_brdf_data.roughness, HALF_MIN);
    out_brdf_data.grazing_term          = saturate(PerceptualRoughnessToSmoothness(roughness) + reflectivity);
	out_brdf_data.normalization_term    = out_brdf_data.roughness * 4.0f + 2.0f;
    out_brdf_data.roughness2_minus_one  = out_brdf_data.roughness2 - 1.0f;
}

void InitializeBRDFData(const float3 albedo, const float metallic, const float roughness, inout float alpha, out BRDFData out_brdf_data)
{
    const float one_minus_reflectivity  = OneMinusReflectivityMetallic(metallic);
    const float reflectivity            = 1.0f - one_minus_reflectivity;
    const float3 brdf_diffuse           = albedo * one_minus_reflectivity;
    const float3 brdf_specular          = lerp(kDielectricSpec.rbg, albedo, metallic);

    InitializeBRDFDataDirect(brdf_diffuse, brdf_specular, reflectivity, one_minus_reflectivity, roughness, alpha, out_brdf_data);
}

float DirectBRDFSpecular(
		const BRDFData brdf_data, const float3 normal_ws,
		const float3 light_direction_ws,const float3 view_direction_ws)
{
    const float3 half_dir = SafeNormalize(float3(light_direction_ws) + float3(view_direction_ws));

    const float NoH = saturate(dot(normal_ws, half_dir));
    const float LoH = saturate(dot(light_direction_ws, half_dir));

	// GGX Distribution multiplied by combined approximation of Visibility and Fresnel
    // BRDFspec = (D * V * F) / 4.0
    // D = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2
    // V * F = 1.0 / ( LoH^2 * (roughness + 0.5) )
    // See "Optimizing PBR for Mobile" from Siggraph 2015 moving mobile graphics course
    // https://community.arm.com/events/1155

    // Final BRDFspec = roughness^2 / ( NoH^2 * (roughness^2 - 1) + 1 )^2 * (LoH^2 * (roughness + 0.5) * 4.0)
    // We further optimize a few light invariant terms
    // brdfData.normalizationTerm = (roughness + 0.5) * 4.0 rewritten as roughness * 4.0 + 2.0 to a fit a MAD.
    const float d = NoH * NoH * brdf_data.roughness2_minus_one + 1.00001f;

    const float LoH2 = LoH * LoH;
    const float specular_term = brdf_data.roughness2 / ((d * d) * max(0.1h, LoH2) * brdf_data.normalization_term);

    return specular_term;
}

///////////////////////////////////
//       Lighting Function       //
///////////////////////////////////
float3 LightingPhysicallyBased(
		const BRDFData brdf_data, const BRDFData brdf_data_clear_coat,
		const float3 light_color, const float3 light_direction_ws,
		const float light_attenuation, const float3 normal_ws,
		const float3 view_direction_ws, const float clear_coat_mask,
		const bool specular_highlight_off)
{
    const float NdotL = saturate(dot(normal_ws, -light_direction_ws));
    //float3 dir = float3(0, 0, 1);
    //float3 dir2 = float3(-0.46934, -0.46438, -0.75105);
    //float NdotL = saturate(dot(dir2, dir));
    //float d = saturate(dot(float3(1, 0, 0), float3(1, 0, 0)));
    const float3 radiance = light_color * (light_attenuation * NdotL);

    float3 brdf = brdf_data.diffuse;

    brdf += brdf_data.specular * DirectBRDFSpecular(brdf_data, normal_ws, light_direction_ws, view_direction_ws);

    return brdf * radiance;
}


float3 LightingPhysicallyBased(
		const BRDFData brdf_data, const BRDFData brdf_data_clear_coat,
        const Light light, const float3 normal_ws,
        const float3 view_direction_ws, const float clear_coat_mask,
        const bool specular_highlight_off)
{
    return LightingPhysicallyBased(brdf_data, brdf_data_clear_coat, light.color, light.direction, light.distance_attenuation * light.shadow_attenuation, normal_ws, view_direction_ws, clear_coat_mask, specular_highlight_off);
}