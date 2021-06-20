// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_WV_POSITION


#include "globals.hlsli"

float4 main(PS_Input pin) : SV_TARGET
{
    // http://marupeke296.com/DXG_No55_WhatIsW.html
    // Z値を算出
    // semantic : POSITION
    float4 color;
    //color.r = pin.world_view_position.z / pin.world_view_position.w; // wvp空間での深度値(0.0 ~ 1.0)
    color.r = pin.wvp_position.z / pin.wvp_position.w;
    color.g = color.r * color.r;
    color.b = 0;
    color.a = 1;

    return color;
}