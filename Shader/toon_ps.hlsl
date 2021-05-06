#include "general.hlsli"
#include "functions.hlsli"
#include "toon.hlsli"

Texture2D	 diffuse_map		 : register(t0);
SamplerState diffuse_map_sampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = diffuse_map.Sample(diffuse_map_sampler, pin.texcoord);
    float alpha          = diffuse_color.a;

    float3 A = m_Ka.xyz;
    float3 N = normalize(pin.normal);
    float3 L = normalize(lightDirection.xyz);
    float3 C = lightColor.xyz;
    float3 E = normalize(pin.eye_vector.xyz);
    float3 K = reflectance;

    float3 specular = BlinnPhongSpecular(N, L, C, -E, K, power);

    specular = smoothstep(0, 1, specular);

    float3 diffuse = Diffuse(N, L, C, K);

    diffuse = smoothstep(0, 1, diffuse);

    float3 irradiance = ceil(diffuse * tones) / tones;


    float rim       = pow(1.0 - clamp(dot(N, -E), 0.0, 1.0), 5.0);
    float rim_dotLE = pow(max(dot(E, L),0), 30.0);
    float rim_color = rim * rim_dotLE;

    float3 radiance = diffuse_color.rgb * pin.color.rgb * float3(irradiance + specular + A + rim_color);

    return float4(radiance, alpha);
}