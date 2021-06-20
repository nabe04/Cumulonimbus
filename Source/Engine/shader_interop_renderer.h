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
	float material_padding;
};

// 共通のコンスタントバッファ
// slot 0 (b0)
CBUFFER(FrameCB, CBSlot_Renderer_Frame)
{
	float	screen_width;
	float	screen_height;
	float2	renderer_frame_padding;
};

// slot1 (b1)
CBUFFER(CameraCB, CBSlot_Camera)
{
	float4x4 camera_view_matrix;
	float4x4 camera_projection_matrix;
	float4x4 camera_view_projection_matrix;

	float3	 camera_position;
	float	 camera_distance_from_origin;

	float3	 camera_at;
	float	 camera_near_z;

	float3	 camera_up;
	float	 camera_far_z;

	float3	 camera_right;
	float	 camera_fov;

	float3	 camera_front;
	float	 camera_aspect;

	float	 camera_width;
	float	 camera_height;
	float2	 camera_padding;
};

// slot2 (b2)
CBUFFER(LightCB, CBSlot_Light)
{
	float3	light_position;				// ライト位置
	float	orthographic_view_width;	// 平行投影 : 幅

	float3	light_direction;			// ライト向き
	float	orthographic_view_height;	// 平行投影 : 高さ

	float3	light_color;
	float	orthographic_near_z;		// 平行投影 : 最近値

	float	orthographic_far_z;			// 平行投影 : 最遠値
	float3	light_padding;

	float4x4 light_perspective_projection_matrix;		// プロジェクション行列(透視投影)
	float4x4 light_perspective_view_projection_matrix;	// ビュー・プロジェクション行列(透視投影)

	float4x4 light_orthographic_projection_matrix;		// プロジェクション行列(平行投影)
	float4x4 light_orthographic_view_projection_matrix;	// ビュー・プロジェクション行列(平行投影)
};

// slot3 (b3)
CBUFFER(TransformCB, CBSlot_Transform)
{
	float4x4 bone_transforms[MaxBones];
	float4x4 transform_matrix;
};

// slot4 (b4)
CBUFFER(MaterialCB, CBSlot_Material)
{
	ShaderMaterial material;
};


#endif // SHADER_INTEROP_RENDERER_H
