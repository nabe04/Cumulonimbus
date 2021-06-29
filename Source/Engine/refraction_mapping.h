#ifndef REFRACTION_MAPPING_H
#define REFRACTION_MAPPING_H

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <imgui.h>

#include "shader.h"
#include "shader_asset.h"
#include "cereal_helper.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(RefractionCB, CBSlot_RefractionMapping)
{
	float  refraction_refractive_index; // 屈折率
	float3 refraction_padding;

#ifdef __cplusplus
	template <typename Archive>
	void serialize(Archive && archive)
	{
		archive(CEREAL_NVP(refraction_refractive_index));
	}
#endif // __cplusplus
};

#ifdef __cplusplus

namespace cumulonimbus
{
	namespace shader_system
	{
		class RefractionMappingShader final : public Shader
		{
		public:
			explicit RefractionMappingShader();
		};
	}

	namespace shader_asset
	{
		class RefractionMappingAsset final : public ShaderAsset
		{
		public:
			explicit RefractionMappingAsset();

			void BindCBuffer()   override;
			void UnbindCBuffer() override;
			void RenderImGui()   override;

			template <typename Archive>
			void serialize(Archive&& archive)
			{
				archive(
					cereal::base_class<ShaderAsset>(this),
					CEREAL_NVP(cb_refraction)
				);
			}

		private:
			std::unique_ptr<buffer::ConstantBuffer<RefractionCB>> cb_refraction{ nullptr };
		};
	}
}

CEREAL_REGISTER_TYPE(cumulonimbus::shader_asset::RefractionMappingAsset)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::shader_asset::ShaderAsset, cumulonimbus::shader_asset::RefractionMappingAsset)

namespace shader
{
	struct M_Refraction
	{// (b6)
		DirectX::XMFLOAT4 refractive_index;	// 屈折率(x成分のみ仕様)(0.0 ～ 1.0)

		void EditParam()
		{
			ImGui::DragFloat("RefractieIndex", &refractive_index.x, 0.005f, 0.0f, 1.0f);
		}

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::make_nvp("refractive_index", refractive_index)
			);
		}
	};

	class RefractionMapping final :public Shader<M_Refraction>
	{
	private:
		const char* vs_name = "./Shader/cso/refraction_mapping_vs.cso";
		const char* ps_name = "./Shader/cso/refraction_mapping_ps.cso";

	public:
		explicit RefractionMapping(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}

#endif // __cplusplus

#endif // REFRACTION_MAPPING_H
