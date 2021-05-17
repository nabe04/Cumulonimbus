#define USE_WORLD_VIEW_POSITION

#include "general.hlsli"

float4 main(PS_Input pin) : SV_TARGET
{
    // http://marupeke296.com/DXG_No55_WhatIsW.html
    // Z�l���Z�o
    // semantic : POSITION
    float4 color;
    color.r = pin.world_view_position.z / pin.world_view_position.w; // wvp��Ԃł̐[�x�l(0.0 ~ 1.0)
    color.r = 1;
    color.g = 1;
    //color.g = pow(color.r, 2.f);
    color.b = 0;
    color.a = 1;

    return color;
}