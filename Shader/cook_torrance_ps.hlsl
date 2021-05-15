#include "general.hlsli"
#include "functions.hlsli"
#include "cook_torrance.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 L = light_direction.xyz; // Light Vector
    float3 N = normalize(pin.normal);         // Normals (normalized)
    float3 H = normalize(-L + pin.eyeVector); // Half Vector (Light vector and Eye vector)
    float3 E = normalize(pin.eyeVector);

    //-- Each angle used in the calculation --//
    float NE = dot(N, E);
    float NH = dot(N, H);
    float EH = dot(E, H);
    float NL = dot(N, -L);
    float LH = dot(-L, H);

    // Beckmann distribution function value
    float D = Beckmann(roughness, NH);

    // Geometric reduction ratio
    float G = min(1, min(2 * NH * NE / EH, 2 * NH * NL / EH));

    // Fresnel Equations
    float n     = 20.0f; // ï°ëfã¸ê‹ó¶ÇÃé¿ïî
    float g     = sqrt(n * n + LH * LH - 1);
    float gpc   = g + LH;
    float gnc   = g - LH;
    float cgpc  = LH * gpc - 1;
    float cgnc  = LH * gnc + 1;
    float F = 0.5f * gnc * gnc * (1 + cgpc * cgpc / (cgnc * cgnc)) / (gpc * gpc);

    float3 zero = float3(0,0,0);
    float4 f4_diffuse = pin.color * min(0.0f, float4(Diffuse(N, L, light_ambient.rgb, float3(1.0f, 1.0f, 1.0f)), 1.0f));
    f4_diffuse.w = 1.0f;

    return f4_diffuse + light_ambient * max(0, F * D * G / NE);
}