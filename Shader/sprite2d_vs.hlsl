#include "sprite2d.hlsli"

VS_OUT main( VS_IN vin )
{
    VS_OUT vout = (VS_OUT) 0;
    vout.position = float4(vin.position, 0.0f, 1.0f);
    vout.texcoord = vin.texcoord;
    vout.color    = vin.color;

    return vout;
}