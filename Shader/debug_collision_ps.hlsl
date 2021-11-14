// VS_Input
#define VIN_USE_LOCAL_POSITION
#define VIN_USE_NORMAL
#define VIN_USE_TEXCOORD0
#define VIN_USE_BONE_WEIGHTS
#define VIN_USE_BONE_INDICES
// PS_Input(VS_Output)

#include "globals.hlsli"
#include "diffuse.hlsli"
#include "../Source/Engine/debug_collision.h"


PS_Output main(PS_Input pin)
{
    PS_Output pout = (PS_Output) 0;
    pout.color = collider_color;
    return pout;
}