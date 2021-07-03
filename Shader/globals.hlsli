#ifndef SHADER_GLOBALS_HF
#define SHADER_GLOBALS_HF

#include "../Source/Engine/shader_interop.h"
#include "../Source/Engine/shader_interop_renderer.h"

struct VS_Input
{
#if defined(VIN_USE_LOCAL_POSITION)
    float4 position      : POSITION;
#endif // VIN_USE_LOCAL_POSITION

#if defined(VIN_USE_NORMAL)
    float3 normal        : NORMAL;
#endif // VIN_USE_NORMAL

#if defined(VIN_USE_TEXCOORD0)
    float2 texcoord0      : TEXCOORD;
#endif // VIN_USE_TEXCOORD0

#if defined(VIN_USE_BONE_WEIGHTS)
    float4 bone_weights   : WEIGHTS;
#endif // VIN_USE_BONE_WEIGHTS

#if defined(VIN_USE_BONE_INDICES)
    uint4  bone_indices   : BONES;
#endif // VIN_USE_BONE_INDICES

#if defined(VIN_USE_COLOR)
    float4 color : COLOR;
#endif // VIN_USE_COLOR

#if defined(VIN_USE_WORLD_TRANSFORM_MATRIX)
    float4x4 world_transform_matrix : WORLD_TRANSFORM_MATRIX;
#endif // VIN_USE_WORLD_TRANSFORM_MATRIX
};

struct PS_Input
{
    float4 position : SV_POSITION;
#if defined(PIN_USE_W_POSITION)
    float4 w_position : W_POSITION;
#endif // PIN_USE_W_POSITION

#if defined(PIN_USE_WV_POSITION)
	float4 wv_position : WV_POSITION;
#endif // PIN_USE_WV_POSITION

#if defined(PIN_USE_WVP_POSITION)
    float4 wvp_position : WVP_POSITION;
#endif // PIN_USE_WVP_POSITION

#if defined(PIN_USE_NORMAL)
    float3 normal : NORMAL;
#endif // PIN_USE_NORMAL

#if defined(PIN_USE_TANGENT)
    float3 tangent : TANGENT;
#endif // PIN_USE_TANGENT_VEC

#if defined(PIN_USE_BINORMAL)
	float3 binorma : BI_NORMAL;
#endif // PIN_USE_BINORMAL

#if defined(PIN_USE_COLOR)
    float4 color : COLOR;
#endif // PIN_USE_COLOR

#if defined(PIN_USE_TEX_POSITION)
	float3 tex_position : TEX_POSITION;
#endif // PIN_USE_TEX_POSITION

#if defined(PIN_USE_TEXCOORD0)
    float2 texcoord0 : TEXCOORD0;
#endif // PIN_USE_TEXCOORD0
};

typedef PS_Input VS_OutPut;

// for geometry buffer(gbuffer)
struct PS_Output
{
    float4 albedo_color : SV_TARGET0;
    float4 normal       : SV_TARGET1;
    float4 position     : SV_TARGET2;
};


TEXTURE2D(texture_depth     , float4, TexSlot_Depth);
TEXTURE2D(texture_base_color, float4, TexSlot_BaseColorMap);
TEXTURE2D(texture_normal    , float4, TexSlot_NormalMap);
TEXTURE2D(texture_position  , float4, TexSlot_Position);
TEXTURECUBE(texture_cube    , float4, TexSlot_SkyMap);

inline float GetScreenWidth()  { return screen_width;  }
inline float GetScreenHeight() { return screen_height; }

#endif // SHADER_GLOBALS_HF
