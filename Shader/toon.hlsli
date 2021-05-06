//------------------------------------------
//  Data Format
//------------------------------------------
struct VS_IN
{
    float4 position     : POSITION;
    float3 normal       : NORMAL;
    float2 texcoord     : TEXCOORD;
    float4 boneWeights  : WEIGHTS;
    uint4  boneIndices  : BONES;
};

struct VS_OUT
{ // Vertex shader output
    float4 position     : SV_POSITION;
    float3 normal       : NORMAL;
    float2 texcoord     : TEXCOORD;
    float4 color        : COLOR;
    float3 eye_vector   : EYE_VECTOR;
};

cbuffer CB_Toon : register(b6)
{
    int   tones;
    float reflectance;
    float power;
    float padding;
}