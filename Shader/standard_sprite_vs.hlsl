#define VIN_USE_LOCAL_POSITION
#define VIN_USE_TEXCOORD0
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"
#include "../Source/Engine/standard_sprite.h"

PS_Input main(VS_Input vin)
{
    PS_Input vout = (PS_Input) 0;
    vin.position.xy += old_sprite_offset;
    float3 ndc_pos = mul(transform_matrix, vin.position).xyz;

    //float2 local = vin.position.xy + sprite_offset;
    //float3 world = mul(transform_matrix, float4(local, .0f, 1.f)).xyz;

    vout.position = float4(vin.position.xy, 0.0f, 1.f);
    vout.texcoord0  = vin.texcoord0;
    return vout;
}