#ifndef SHADER_INTEROP_RENDERER_H
#define SHADER_INTEROP_RENDERER_H
#include "shader_interop.h"

#define MaxBones 128 // ボーンの最大数

struct ShaderMaterial
{
	float4 base_color;
	float4 emissive_color;

	float roughness;
	float reflectance;
	float metalness;
	float padding;
};

// 共通のコンスタントバッファ
// slot 0 (b0)
CBuffer(FrameCB, CBSlot_Renderer_Frame)
{
	float2 screen_width_height;
	float2 padding;
};

// slot1 (b1)
CBuffer(CameraCB, CBSlot_Camera)
{
	float4x4 camera_view_matrix;
	float4x4 camera_view_projection_matrix;

	float3	 camera_position;
	float	 camera_distance_from_origin;

	float3	 camera_at;
	float	 camera_near_z;

	float3	 camera_up;
	float	 camera_far_z;
};

// slot2 (b2)
CBuffer(LightCB, CBSlot_Light)
{
	float3	light_position;
	float	orthographic_view_width;

	float3	light_direction;
	float	orthographic_view_height;

	float3	light_color;
	float	orthographic_near_z;

	float	orthographic_far_z;
	float3	padding;

	float4x4 light_view_matrix;

	float4x4 light_perspective_projection_matrix;
	float4x4 light_perspective_view_projection_matrix;

	float4x4 light_orthographic_projection_matrix;
	float4x4 light_orthographic_view_projection_matrix;
};

// slot3 (b3)
CBuffer(TransformCB, CBSlot_Transform)
{
	float4x4 bone_transforms[MaxBones];
	float4x4 transform_matrix;
};

// slot4 (b4)
CBuffer(MaterialCB, CBSlot_Material)
{
	ShaderMaterial material;
};


#endif // SHADER_INTEROP_RENDERER_H
