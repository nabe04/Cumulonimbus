// VS_Input
#define VIN_USE_LOCAL_POSITION
// PS_Input(VS_Output)

#include "globals.hlsli"
#include "../Source/Engine/debug_primitive_shader.h"

VS_OutPut main( VS_Input vin)
{
    VS_OutPut vout = (VS_OutPut) 0;

    float3 world_pos = { 0, 0, 0 };
    world_pos = mul(primitive_world_transform_mat, vin.position).xyz;

    vout.position = float4(world_pos, 1.0f);
    return vout;
}