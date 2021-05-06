#include "single_color.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    return float4(single_color.rgb, 1);
}