#define SPOT_MAX 32

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
    float4 wvpPos : SV_POSITION;
    float4 worldPos : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 eyeVector : EYE_VECTOR;
};

struct SpotLight
{
    float index;
    float range; // Range of light
    float type; // Valid or invalid
    float innerCone;
    float outerCone;
    float dumy0;
    float dumy1;
    float dumy2;
    float4 position;
    float4 color;
    float4 direction;
};

cbuffer CB : register(b6)
{
    SpotLight spotLights[SPOT_MAX]; //スポットライト配列
};
