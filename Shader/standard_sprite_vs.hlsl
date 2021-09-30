#define VIN_USE_LOCAL_POSITION
#define VIN_USE_TEXCOORD0
#define PIN_USE_TEXCOORD0

#include "globals.hlsli"

PS_Input main(VS_Input vin)
{
    PS_Input vout = (PS_Input) 0;

    vout.position   = mul(transform_matrix, vin.position);
    vout.texcoord0  = vin.texcoord0;
    return vout;
}