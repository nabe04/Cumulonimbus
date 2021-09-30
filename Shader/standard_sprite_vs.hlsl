#define VIN_USE_LOCAL_POSITION
#define VIN_USE_TEXCOORD0
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"

PS_Input main(VS_Input vin)
{
    PS_Input vout = (PS_Input) 0;
    float3 world = mul(transform_matrix, vin.position).xyz;

    vout.position = float4(world, 1.f);
    vout.texcoord0  = vin.texcoord0;
    return vout;
}