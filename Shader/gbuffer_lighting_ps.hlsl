#define PIN_USE_TEXCOORD0

#include "globals.hlsli"
SamplerState defaultSampler : register(s0);
float4 main(PS_Input pin) : SV_TARGET
{
    float4 albedo_color = texture_base_color.Sample(defaultSampler, pin.texcoord0);
    float4 normal_map   = texture_normal.Sample(defaultSampler, pin.texcoord0);
    float4 position_map = texture_position.Sample(defaultSampler, pin.texcoord0);
    float  shader_slot  = texture_shader_slot.Sample(defaultSampler, pin.texcoord0);

    return albedo_color;
}