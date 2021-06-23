// PS_Input(VS_Output)
#define PIN_USE_WVP_POSITION
#define PIN_USE_NORMAL
#define PIN_USE_COLOR
#define PIN_USE_TEXCOORD0     // 読み込んだテクスチャのUV値

#include "globals.hlsli"
#include "../Source/Engine/single_color.h"

float4 main(PS_Input pin) : SV_TARGET
{
    return float4(pin.color.rgb, 1);
}