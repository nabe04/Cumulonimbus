#ifndef PHONG_H
#define PHONG_H

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <imgui.h>

#include "shader.h"
#include "shader_asset.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(PhongCB, CBSlot_Phong)
{
	float phong_specular;
	float phong_ref_coefficient;
	float phong_pow_value;
	float phong_padding;

#ifdef __cplusplus
	template <typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(phong_specular),
			CEREAL_NVP(phong_ref_coefficient),
			CEREAL_NVP(phong_pow_value)
		);
	}
	
#endif // __cplusplus
};

#ifdef __cplusplus

namespace cumulonimbus
{
	namespace shader_system
	{
		class PhongShader final : public Shader
		{
		public:
			explicit PhongShader();
		};
	}

	namespace shader_asset
	{
		class PhongAsset final : public ShaderAsset
		{
		public:
			explicit PhongAsset();

			void BindCBuffer()	 override;
			void UnbindCBuffer() override;
			void RenderImGui()	 override;

			template <typename  Archive>
			void serialize(Archive&& archive)
			{
				archive(
					cereal::base_class<ShaderAsset>(this),
					CEREAL_NVP(cb_phong)
				);
			}
		private:
			std::unique_ptr<buffer::ConstantBuffer<PhongCB>> cb_phong{ nullptr };
		};
	}
}

CEREAL_REGISTER_TYPE(cumulonimbus::shader_asset::PhongAsset)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::shader_asset::ShaderAsset, cumulonimbus::shader_asset::PhongAsset)

namespace shader
{
	struct M_Phong
	{// Supplemental Settings (b6)
		FLOAT    specular			= 1.0f;		// Specular reflection
		FLOAT    ref_coefficient	= 1.0f;		// Reflection coefficient
		FLOAT	 pow_value			= 1.0f;		// Power value
		FLOAT    padding[1]			= {};

		void EditParam()
		{
			ImGui::DragFloat("Specular", &specular, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Reflection coefficient", &ref_coefficient, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Power", &pow_value, 0.5f, 1.0f, 100.0f);
		}

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::make_nvp("specular", specular),
				cereal::make_nvp("reflection coefficient", ref_coefficient),
				cereal::make_nvp("power value", pow_value));
		}
	};


	class Phong final :public Shader<M_Phong>
	{
	private:
		const char* vs_name = "./Shader/cso/phong_vs.cso";
		const char* ps_name = "./Shader/cso/phong_ps.cso";

	public:
		explicit Phong(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}

#endif // __cplusplus

#endif // PHONG_H
