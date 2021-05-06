#define USE_VOUT_COLOR

#include "general.hlsli"

float4 main(PS_Input pin) : SV_TARGET
{
    return pin.color;
    //return float4(1,0,0,1);
}