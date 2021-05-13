#define USE_NDC_POSITION

#include "general.hlsli"
#include "shadow_map.hlsli"

float4 main(PS_Input pin) : SV_TARGET
{
    // http://marupeke296.com/DXG_No55_WhatIsW.html
    // Z値を算出
    // semantic : POSITION
    float4 color;
    color.r = pin.ndc_position.z;  // pin.ndc_position.w; // 深度値
    //color.r = 0.5;
    color.g = pow(color.r, 2.f);
   // color.g = 0.5f;
    color.b = 0;
    color.a = 1;

    return color;
}