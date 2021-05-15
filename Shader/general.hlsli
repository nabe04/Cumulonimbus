//******************************************
//  Constant Buffer
//******************************************
#define MAX_BONES 128
cbuffer CBTranslation : register(b0)
{
    row_major float4x4 viewProjection;
    row_major float4x4 boneTransforms[MAX_BONES];
    row_major float4x4 transform_matrix;
};

cbuffer CBMesh : register(b1)
{
    float4 m_Color;
    float4 m_Ka;
    float4 m_Kd;
    float4 m_Ks;
};

cbuffer CBLight : register(b2)
{
    float4 lightDirection;
    float4 lightColor;
    float4 ambient;
    float4 eyePosition;
    float4 light_position;
    row_major float4x4 light_view_projection;
}

//cbuffer CBLight : register(b2)
//{
//    float4 light_position;
//    float4 light_direction;
//    float4 light_ambient;
//    float4 light_color;

//    row_major float4x4 view_matrix;

//    row_major float4x4 light_perspective_projection_matrix;
//    row_major float4x4 light_perspective_view_projection_matrix;

//    float light_orthographic_view_width;
//    float light_orthographic_view_height;
//    float light_orthographic_near_z;
//    float light_orthographic_far_z;
//    row_major float4x4 light_orthographic_projection_matrix;
//    row_major float4x4 light_orthographic_view_projection_matrix;
//}

struct VS_Input
{
#if defined(USE_LOCAL_POSITION)
    float4 position      : POSITION;
#endif
#if defined(USE_VIN_NORMAL)
    float3 normal        : NORMAL;
#endif
#if defined(USE_TEXCOORD0)
    float2 texcoord0      : TEXCOORD;
#endif
#if defined(USE_BONE_WEIGHTS)
    float4 boneWeights   : WEIGHTS;
#endif
#if defined(USE_BONE_INDICES)
    uint4  boneIndices   : BONES;
#endif
#if defined(USE_VIN_COLOR)
    float4 color : COLOR;
#endif
#if defined(USE_VIN_WORLD_TRANSFORM_MATRIX)
    row_major float4x4 world_transform_matrix : WORLD_TRANSFORM_MATRIX;
#endif
};

struct PS_Input
{
    float4 position : SV_POSITION;
#if defined(USE_WORLD_POSITION)
    float4 world_position : WORLD_POSITION;
#endif
#if defined(USE_NDC_POSITION)
    float4 ndc_position : NDC_POSITION;
#endif
#if defined(USE_TEXTURE_POSITION)
    float3 texture_coodinate : TEXTURE_POSITON;
#endif
#if defined(USE_VOUT_NORMAL)
    float3 normal : NORMAL;
#endif
#if defined(USE_VOUT_COLOR)
    float4 color : COLOR;
#endif
#if defined(USE_TEXCOORD0)
    float2 texcoord0 : TEXCOORD0;
#endif
};

typedef PS_Input VS_OutPut;

//#define USE_LOCAL_POSITION
//#define USE_WORLD_POSITION
//#define USE_NDC_POSITION
//#define USE_TEXTURE_POSITION
//#define USE_NORMAL
//#define USE_VIN_COLOR
//#define USE_VOUT_COLOR
//#define USE_TEXCOORD0
//#define USE_BONE_WEIGHTS
//
//#define USE_BONE_INDICES

//******************************************
//  Data Format
//******************************************
//struct VS_IN
//{
//    float4 position : POSITION;
//    float3 normal : NORMAL;
//    float2 texcoord : TEXCOORD;
//    float4 boneWeights : WEIGHTS;
//    uint4 boneIndices : BONES;
//};

//struct VS_OUT
//{
//    float4 position : SV_POSITION;
//    float3 normal : NORMAL;
//    float2 texcoord : TEXCOORD;
//    float4 color : COLOR;
//};


