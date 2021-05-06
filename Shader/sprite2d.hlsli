//------------------------------------------
//  Data Format
//------------------------------------------
struct VS_IN
{
    float2 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer CBMesh : register(b1)
{
    float4 m_Color;
    float4 m_Ka;
    float4 m_Kd;
    float4 m_Ks;
};

