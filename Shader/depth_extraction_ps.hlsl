#define USE_NDC_POSITION

#include "general.hlsli"
#include "shadow_map.hlsli"

float4 main(PS_Input pin) : SV_TARGET
{
    // http://marupeke296.com/DXG_No55_WhatIsW.html
    // ZílÇéZèo
    // semantic : POSITION
    float4 color = pin.ndc_position.z / pin.ndc_position.w;
    color.w = 1;

    return color;
}