// VS Input
#define USE_LOCAL_POSITION
#define USE_NORMAL
//#define USE_VIN_WORLD_TRANSFORM_MATRIX
// PS Input
#define USE_VOUT_COLOR

#include "general.hlsli"

cbuffer CB_Particle : register(b6)
{
    row_major float4x4  particle_transform;
    float4              particle_local_position;
    float4              particle_color;
};

VS_OutPut main(VS_Input vin)
{
    //float4 world_pos = mul(particle_local_position, particle_transform);
    //float4 wvp_pos = mul(float4(world_pos.xyz, 1.0f), viewProjection);

    float4 world_pos = mul(float4(vin.position.xyz, 1.0f), particle_transform);
    float4 wvp_pos   = mul(float4(world_pos.xyz, 1.0f), viewProjection);

    VS_OutPut pin = (VS_OutPut) 0;
    pin.position = wvp_pos;
    pin.color    = particle_color;

	return pin;
}