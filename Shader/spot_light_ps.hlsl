#include "general.hlsli"
#include "spot_light.hlsli"
#include "functions.hlsli"

//*******************************************
//  Txture
//*******************************************
Texture2D diffuseTexture : register(t0);
SamplerState defaultSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 L = normalize(light_color.xyz);
    float3 E = normalize(light_position.xyz - pin.worldPos.xyz);

    // Ambient
    float4 color    = diffuseTexture.Sample(defaultSampler, pin.texcoord);
    float3 C        = light_color.rgb;  //ì¸éÀåı(êFÇ∆ã≠Ç≥)

    // Normal
    float3 N = normalize(pin.normal); // Normal

    // Diffuse
    float3 Kd = float3(1, 1, 1);
    float3 D  = Diffuse(N, L, C, Kd);

    // Specular
    float3 Ks = float3(1, 1, 1);
    float3 S = BlinnPhongSpecular(N, L, C, E, Ks, 20);

    // Spot light
    float3 SL;
    float3 SC;
    float3 influence;
    float3 SD = (float3) 0;
    float3 SS = (float3) 0;
    for (int i = 0; i < SPOT_MAX;i++)
    {
        if(spotLights[i].type == 0)
            continue;

        SL = pin.worldPos.xyz - spotLights[i].position.xyz;
        float d = length(SL);

        float r = spotLights[i].range;
        if(d > r)
            continue;

        float3 SFront   = normalize(spotLights[i].direction.xyz);
        SL              = normalize(SL);
        float angle     = dot(SL, SFront);
        float area      = spotLights[i].innerCone - spotLights[i].outerCone;
        float influene2 = spotLights[i].innerCone - angle;
        influene2       = saturate(1.0 - influene2 / area);
        influence       = 1;
        SC              = spotLights[i].color.rgb;
        SD             += Diffuse(N, SL, SC, Kd) * influence * influence * influene2;
        SS             += BlinnPhongSpecular(N, SL, SC, E, Ks, 20) * influence * influence * influene2;
    }

    return color * pin.color * float4(m_Ka.rgb + D + S + SD + SS, 1.0f);
}