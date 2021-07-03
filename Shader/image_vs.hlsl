#define PIN_USE_TEXCOORD0

#include "globals.hlsli"

VS_OutPut main(in uint vertex_id : SV_VERTEXID)
{
    VS_OutPut vout  = (VS_OutPut) 0;
    vout.texcoord0  = float2((vertex_id << 1) & 2, vertex_id & 2);
    vout.position   = float4(vout.texcoord0 * float2(2.0f, -2.0f) +
						     float2(-1.0f, 1.0f), 0.0f, 1.0f);

    return vout;
}