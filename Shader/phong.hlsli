//------------------------------------------
//  Data Format
//------------------------------------------
//struct VS_IN
//{
//    float4 position : POSITION;
//    float3 normal : NORMAL;
//    float2 texcoord : TEXCOORD;
//    float4 boneWeights : WEIGHTS;
//    uint4 boneIndices : BONES;
//};

//struct VS_OUT
//{ // Vertex shader output
//    float4 position : SV_POSITION;
//    float3 normal : NORMAL;
//    float2 texcoord : TEXCOORD;
//    float4 color : COLOR;
//    float3 eyeVector : EYE_VECTOR;
//};

//------------------------------------------
//  Constant Buffer
//------------------------------------------
cbuffer CB_Environment : register(b6) // b0 and b1 have been used
{ // Expanded settings
    float  specular;        // Specular reflection
    float  refCoefficient;  // Reflection coefficient (.w is used by a "power" value)
    float  powValue;
    float  padding[1];
};
