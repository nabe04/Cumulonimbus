# ifndef TOON_H
#define  TOON_H

#ifdef __cplusplus
#include <memory>
#include <wrl.h>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "shader.h"
#include "shader_asset.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(ToonCB, CBSlot_Toon)
{
	int   toon_tones;
	float toon_reflectance;
	float toon_power;
	float toon_padding;

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(toon_tones),
			CEREAL_NVP(toon_reflectance),
			CEREAL_NVP(toon_power)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus

namespace cumulonimbus
{
	namespace shader_system
	{
		class ToonShader final : public Shader
		{
		public:
			explicit ToonShader();
		};
	}

	namespace shader_asset
	{
		class ToonAsset final : public ShaderAsset
		{
		public:
			explicit ToonAsset();
			~ToonAsset() override = default;

			void BindCBuffer()	 override;
			void UnbindCBuffer() override;
			void RenderImGui()	 override;

			template <typename Archive>
			void serialize(Archive&& archive)
			{
				archive(
					cereal::base_class<ShaderAsset>(this),
					CEREAL_NVP(cb_toon)
				);
			}

		private:
			std::unique_ptr<buffer::ConstantBuffer<ToonCB>> cb_toon{ nullptr };
		};

	}
}

CEREAL_REGISTER_TYPE(cumulonimbus::shader_asset::ToonAsset)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::shader_asset::ShaderAsset, cumulonimbus::shader_asset::ToonAsset)

#endif // __cplusplus

#endif // TOON_H