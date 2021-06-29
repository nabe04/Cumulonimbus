#ifndef SHADER_GLOBALS_HF
#define SHADER_GLOBALS_HF

#include "../Source/Engine/shader_interop.h"
#include "../Source/Engine/shader_interop_renderer.h"

struct VS_Input
{
#if defined(VIN_USE_LOCAL_POSITION)
    float4 position      : POSITION;
#endif
#if defined(VIN_USE_NORMAL)
    float3 normal        : NORMAL;
#endif
#if defined(VIN_USE_TEXCOORD0)
    float2 texcoord0      : TEXCOORD;
#endif
#if defined(VIN_USE_BONE_WEIGHTS)
    float4 bone_weights   : WEIGHTS;
#endif
#if defined(VIN_USE_BONE_INDICES)
    uint4  bone_indices   : BONES;
#endif
#if defined(VIN_USE_COLOR)
    float4 color : COLOR;
#endif
#if defined(VIN_USE_WORLD_TRANSFORM_MATRIX)
    float4x4 world_transform_matrix : WORLD_TRANSFORM_MATRIX;
#endif
};

struct PS_Input
{
    float4 position : SV_POSITION;
#if defined(PIN_USE_W_POSITION)
    float4 w_position : W_POSITION;
#endif
#if defined(PIN_USE_WV_POSITION)
	float4 wv_position : WV_POSITION;
#endif
#if defined(PIN_USE_WVP_POSITION)
    float4 wvp_position : WVP_POSITION;
#endif
#if defined(PIN_USE_NORMAL)
    float3 normal : NORMAL;
#endif
#if defined(PIN_USE_COLOR)
    float4 color : COLOR;
#endif
#if defined(PIN_USE_TEX_POSITION)
	float3 tex_position : TEX_POSITION;
#endif
#if defined(PIN_USE_TEXCOORD0)
    float2 texcoord0 : TEXCOORD0;
#endif
};

typedef PS_Input VS_OutPut;


TEXTURE2D(texture_depth     , float4, TexSlot_Depth);
TEXTURE2D(texture_base_color, float4, TexSlot_BaseColorMap);
TEXTURECUBE(texture_cube, float4, TexSlot_SkyMap);
TEXTURE2D(texture_normal, float4, TexSlot_NormalMap);

inline float GetScreenWidth()  { return screen_width;  }
inline float GetScreenHeight() { return screen_height; }

#endif // SHADER_GLOBALS_HF
