#define USE_LOCAL_POSITION
#define USE_TEXCOORD0
#define USE_VIN_COLOR

#include "general.hlsli"


PS_Input main(VS_Input vin)
{
    PS_Input vout = (PS_Input) 0;

    vout.position   = vin.position;
    vout.texcoord0  = vin.texcoord0;
    return vout;
}