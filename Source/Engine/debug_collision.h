#ifndef DEBUG_COLLISION_H
#define DEBUG_COLLISION_H

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <imgui.h>

#include "shader.h"
#include "shader_asset.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(DebugCollisionCB, CBSlot_DebugCollision)
{
	float4 collider_color;


#ifdef __cplusplus
	template <typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(collider_color)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
namespace cumulonimbus
{
	namespace shader_system
	{
		class DebugCollisionShader final : public Shader
		{
		public:
			explicit DebugCollisionShader();
			~DebugCollisionShader() override = default;
		};
	} // shader_system

	namespace shader_asset
	{
		class DebugCollisionAsset final : public ShaderAsset
		{
		public:
			explicit DebugCollisionAsset();
			~DebugCollisionAsset() override = default;

			void BindCBuffer()	 override;
			void UnbindCBuffer() override;
			void RenderImGui()	 override {}

			template<typename Archive>
			void serialize(Archive&& archive)
			{
				archive(
					cereal::base_class<ShaderAsset>(this),
					CEREAL_NVP(cb_debug_collision)
				);
			}
		private:
			std::unique_ptr<buffer::ConstantBuffer<DebugCollisionCB>> cb_debug_collision{ nullptr };
		};
	} // shader_asset

} // cumulonimbus

CEREAL_REGISTER_TYPE(cumulonimbus::shader_asset::DebugCollisionAsset)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::shader_asset::ShaderAsset, cumulonimbus::shader_asset::DebugCollisionAsset)


#endif // __cplusplus

#endif // DEBUG_COLLISION_H
