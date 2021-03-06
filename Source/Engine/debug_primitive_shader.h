#ifndef DEGUB_PRIMITIVE_SHADER_H
#define DEGUB_PRIMITIVE_SHADER_H

#ifdef __cplusplus

#include "shader.h"
#include "shader_asset.h"

#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(DebugPrimitiveCB, CBSlot_DebugPrimitive)
{
	float4x4  primitive_world_transform_mat;
	float4	  color;

#ifdef __cplusplus
	template <class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(color)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus

namespace cumulonimbus::shader_system
{
	class DebugPrimitiveShader final : public Shader
	{
	public:
		explicit DebugPrimitiveShader();
		~DebugPrimitiveShader() = default;
	};
} // cumulonimbus::shader_system

#endif // __cplusplus

#endif // DEGUB_PRIMITIVE_SHADER_H