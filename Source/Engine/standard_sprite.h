#ifndef STANDARD_SPRITE
#define STANDARD_SPRITE

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <imgui.h>

#include "shader.h"
#include "shader_asset.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(SpriteCB, CBSlot_Sprite)
{
	float2 sprite_offset;
	float2 sprite_padding;

#ifdef __cplusplus
	template <typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(sprite_offset)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus

namespace cumulonimbus
{
	namespace shader_system
	{
		class StandardSpriteShader final : public Shader
		{
		public:
			explicit StandardSpriteShader();
			~StandardSpriteShader() override = default;
		};
	} // shader_system

	namespace shader_asset
	{
		class StandardSpriteAsset final : public ShaderAsset
		{
		public:
			explicit StandardSpriteAsset();
			~StandardSpriteAsset() override = default;

			void BindCBuffer()   override;
			void UnbindCBuffer() override;
			void RenderImGui()   override;

			[[nodiscard]]
			buffer::ConstantBuffer<SpriteCB>* GetCBuffer() const { return cb_sprite.get(); }

			template<class Archive>
			void serialize(Archive&& archive)
			{
				archive(
					cereal::base_class<ShaderAsset>(this),
					CEREAL_NVP(cb_sprite)
				);
			}
		private:
			std::unique_ptr<buffer::ConstantBuffer<SpriteCB>> cb_sprite{ nullptr };
		};
	} // shader_asset

} // cumulonimbus

CEREAL_REGISTER_TYPE(cumulonimbus::shader_asset::StandardSpriteAsset)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::shader_asset::ShaderAsset, cumulonimbus::shader_asset::StandardSpriteAsset)

#endif // __cplusplus

#endif // STANDARD_SPRITE
