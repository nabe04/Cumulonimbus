// VS_Input
#define VIN_USE_LOCAL_POSITION
#define VIN_USE_TEXCOORD0
// PS_Input(VS_Output)
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"

PS_Input main(VS_Input vin)
{
    PS_Input vout = (PS_Input) 0;

    float3 world_pos     = { 0, 0, 0 };
    world_pos            = mul(transform_matrix, vin.position).xyz;
    const float4 wvp_pos = mul(camera_view_projection_matrix, float4(world_pos, 1.0f)); // World coordinate transformation

    vout.position  = wvp_pos;
    vout.texcoord0 = vin.texcoord0;

    return vout;
}