#ifndef SHADER_INTEROP_RENDERER_H
#define SHADER_INTEROP_RENDERER_H

#ifdef __cplusplus
	#include<string>

	#include <cereal/cereal.hpp>

	#include "cereal_helper.h"
#endif // __cplusplus

#include "shader_interop.h"

#define MaxBones 128 // ボーンの最大数

struct ShaderMaterial
{
	float4 base_color;
	float4 emissive_color;

	float roughness;
	float reflectance;
	float metalness;
	int   material_use_custom; // 0 : 既存のテクスチャを使用, 1 : 本構造体のパラメータを使用

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(base_color),
			CEREAL_NVP(emissive_color),
			CEREAL_NVP(roughness),
			CEREAL_NVP(reflectance),
			CEREAL_NVP(metalness)
		);
	}

#endif // __cplusplus
};

//#ifdef __cplusplus
//	struct MaterialFilename
//	{
//		std::string albedo_color_filename;
//		std::string emissive_color_filename;
//
//		std::string roughness_map_filename;
//		std::string reflectance_map_filename;
//		std::string metalness_map_filename;
//	};
//
//#endif // __cplusplus

// 共通のコンスタントバッファ
// slot 0 (b0)
CBUFFER(FrameCB, CBSlot_Renderer_Frame)
{
	float	screen_width;
	float	screen_height;
	float2	renderer_frame_padding;

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(screen_width),
			CEREAL_NVP(screen_height),
			CEREAL_NVP(renderer_frame_padding)
		);
	}

#endif // __cplusplus
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

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(camera_view_projection_matrix),
			CEREAL_NVP(camera_projection_matrix),
			CEREAL_NVP(camera_view_projection_matrix),

			CEREAL_NVP(camera_position),
			CEREAL_NVP(camera_distance_from_origin),

			CEREAL_NVP(camera_at),
			CEREAL_NVP(camera_near_z),

			CEREAL_NVP(camera_up),
			CEREAL_NVP(camera_far_z),

			CEREAL_NVP(camera_right),
			CEREAL_NVP(camera_fov),

			CEREAL_NVP(camera_front),
			CEREAL_NVP(camera_aspect),

			CEREAL_NVP(camera_width),
			CEREAL_NVP(camera_height)
		);
	}

#endif // __cplusplus
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

	float4x4 light_view_matrix;

	float4x4 light_perspective_projection_matrix;		// プロジェクション行列(透視投影)
	float4x4 light_perspective_view_projection_matrix;	// ビュー・プロジェクション行列(透視投影)

	float4x4 light_orthographic_projection_matrix;		// プロジェクション行列(平行投影)
	float4x4 light_orthographic_view_projection_matrix;	// ビュー・プロジェクション行列(平行投影)

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(light_position),
			CEREAL_NVP(orthographic_view_width),

			CEREAL_NVP(light_direction),
			CEREAL_NVP(orthographic_view_height),

			CEREAL_NVP(light_color),
			CEREAL_NVP(orthographic_near_z),

			CEREAL_NVP(orthographic_far_z),

			CEREAL_NVP(light_view_matrix),

			CEREAL_NVP(light_perspective_projection_matrix),
			CEREAL_NVP(light_perspective_view_projection_matrix),

			CEREAL_NVP(light_orthographic_projection_matrix),
			CEREAL_NVP(light_orthographic_view_projection_matrix)
		);
	}

#endif // __cplusplus
};

// slot3 (b3)
CBUFFER(TransformCB, CBSlot_Transform)
{
	float4x4 bone_transforms[MaxBones];
	float4x4 transform_matrix;

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(bone_transforms),
			CEREAL_NVP(transform_matrix)
		);
	}

#endif // __cplusplus
};

// slot4 (b4)
CBUFFER(MaterialCB, CBSlot_Material)
{
	ShaderMaterial material;

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(material)
		);
	}
#endif // __cplusplus
};

// slot5 (b5)
CBUFFER(ShaderSlotCB, CBSlot_ShaderSlot)
{
	int		shader_slot;
	float3	shader_slot_dummy;

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(shader_slot)
		);
	}
#endif // __cplusplus
};


#endif // SHADER_INTEROP_RENDERER_H
