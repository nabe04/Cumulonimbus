//------------------------------------------
//  Data Format
//------------------------------------------
struct VS_IN
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 boneWeights : WEIGHTS;
    uint4 boneIndices : BONES;
};


struct VS_OUT
{ // Vertex shader output
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 eyeVector : EYE_VECTOR;
};

cbuffer CB_Environment : register(b6)
{
    //float4 lightDir;    // Light directon
    //float4 ambient;     // ambient color
    //float4 eyePos;      // Point of view
    float  roughness;   // roughness
    float  padding[3];
};
