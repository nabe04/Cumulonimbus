#ifndef METAL_H
#define METAL_H

#ifdef __cplusplus

#include "shader.h"
#include "shader_asset.h"

#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(MetalCB, CBSlot_Metal)
{
	float  metal_roughness;
	float3 metal_padding;

#ifdef __cplusplus
	template <typename Archive>
	void serialize(Archive && archive)
	{
		archive(
			CEREAL_NVP(metal_roughness)
		);
	}
#endif // __cplusplus
};


#ifdef __cplusplus

namespace cumulonimbus
{
	namespace shader_system
	{
		class MetalShader final : public Shader
		{
		public:
			explicit MetalShader();
			~MetalShader() override = default;
		};
	}

	namespace shader_asset
	{
		class MetalAsset final : public ShaderAsset
		{
		public:
			explicit MetalAsset(u_int gbuff_shader_slot);
			explicit MetalAsset()  = default;
			~MetalAsset() override = default;

			void BindCBuffer()   override;
			void UnbindCBuffer() override;
			void RenderImGui()   override;

			template <typename Archive>
			void serialize(Archive&& archive)
			{
				archive(
					cereal::base_class<ShaderAsset>(this),
					CEREAL_NVP(cb_metal)
				);
			}

		private:
			std::unique_ptr<buffer::ConstantBuffer<MetalCB>> cb_metal{nullptr};
		};
	}
}

CEREAL_REGISTER_TYPE(cumulonimbus::shader_asset::MetalAsset)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::shader_asset::ShaderAsset, cumulonimbus::shader_asset::MetalAsset)

namespace shader
{
	struct M_Metal
	{// Supplemental Settings (b6)
		FLOAT	 roughness = 0.5f;
		FLOAT	 padding[3] = {};

		void EditParam()
		{
			//ImGui::ColorEdit4("Ambient", (float*)&ambient);
			ImGui::DragFloat("Roughness", (float*)&roughness, 0.01f, 0.0f, 1.0f);
		}

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::make_nvp("roughness", roughness)
			);
		}
	};

	class Metal final :public Shader<M_Metal>
	{
	private:
		const char* vs_name = "./Shader/cso/cook_torrance_vs.cso";
		const char* ps_name = "./Shader/cso/cook_torrance_ps.cso";

	public:
		Metal(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}


#endif // __cplusplus

#endif // METAL_H
