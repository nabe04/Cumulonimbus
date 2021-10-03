#ifndef BILLBOARD
#define BILLBOARD

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <imgui.h>

#include "shader.h"
#include "shader_asset.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(BillboardCB, CBSlot_Billboard)
{
	float4x4 billboard_world_transform_matrix;
	float4x4 billboard_scaling_matrix;
	float4x4 billboard_rotation_matrix;
	float4x4 billboard_translation_matrix;

	float2 billboard_offset;
	float2 billboard_padding;

#ifdef __cplusplus
	template <typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(billboard_world_transform_matrix),
			CEREAL_NVP(billboard_scaling_matrix),
			CEREAL_NVP(billboard_rotation_matrix),
			CEREAL_NVP(billboard_translation_matrix),
			CEREAL_NVP(billboard_offset)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus

	namespace cumulonimbus
	{
		namespace shader_system
		{
			class BillboardShader final : public Shader
			{
			public:
				explicit BillboardShader();
				~BillboardShader() override = default;
			};
		} // shader_system

		namespace shader_asset
		{
			class BillboardAsset final : public ShaderAsset
			{
			public:
				explicit BillboardAsset();
				~BillboardAsset() override = default;

				void BindCBuffer()   override;
				void UnbindCBuffer() override;
				void RenderImGui()   override;

				[[nodiscard]]
				buffer::ConstantBuffer<BillboardCB>* GetCBuffer() const { return cb_billboard.get(); }

				template<class Archive>
				void serialize(Archive&& archive)
				{
					archive(
						cereal::base_class<ShaderAsset>(this),
						CEREAL_NVP(cb_billboard)
					);
				}
			private:
				std::unique_ptr<buffer::ConstantBuffer<BillboardCB>> cb_billboard{};
			};
		} // shader_asset

	} // cumulonimbus

	CEREAL_REGISTER_TYPE(cumulonimbus::shader_asset::BillboardAsset)
	CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::shader_asset::ShaderAsset, cumulonimbus::shader_asset::BillboardAsset)

#endif // __cplusplus

#endif // BILLBOARD
