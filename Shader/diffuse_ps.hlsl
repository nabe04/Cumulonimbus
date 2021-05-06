#define USE_VOUT_NORMAL
#define USE_TEXCOORD0
#define USE_NDC_POSITION
#define USE_TEXTURE_POSITION
#define USE_VOUT_COLOR


#include "general.hlsli"
#include "functions.hlsli"
#include "diffuse.hlsli"
#include "shadow_map.hlsli"

//*******************************************
//  Texture
//*******************************************
Texture2D diffuseTexture : register(t0);
Texture2D depth_texture : register(t1);
SamplerState defaultSampler : register(s0);

float4 main(PS_Input pin) : SV_TARGET
{
   const  float3 reflectance = float3(1.0f, 1.0f, 1.0f);

    float3 f3_diffuse = Diffuse(pin.normal.xyz, lightDirection.xyz, pin.color.xyz, reflectance);

    float3 shadow_color = GetShadow(depth_texture, defaultSampler, pin.texture_coodinate, 0.6f,0.001f);

    return diffuseTexture.Sample(defaultSampler, pin.texcoord0) * float4(f3_diffuse, 1) * float4(shadow_color, 1);
}