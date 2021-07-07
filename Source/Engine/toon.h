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

namespace shader
{
	struct M_Toon
	{
		INT   tones			= 3;
		FLOAT reflectance	= 1;
		FLOAT power			= 1;
		FLOAT padding[1];

		void EditParam()
		{
			ImGui::SliderInt("Tone", &tones, 1, 32);
			ImGui::DragFloat("Reflection coefficient", &reflectance, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Power", &power, 0.5f, 1.0f, 100.0f);
		}

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::make_nvp("tones", tones),
				cereal::make_nvp("reflectance", reflectance),
				cereal::make_nvp("power", power)
			);
		}
	};

	struct CB_Toon
	{
		INT   tones = 3;
		FLOAT reflectance = 1;
		FLOAT power = 1;
		FLOAT padding{};

		void Edit();
	};

	class Toon final :public Shader<M_Toon>
	{
	private:
		const char* vs_name = "./Shader/cso/toon_vs.cso";
		const char* ps_name = "./Shader/cso/toon_ps.cso";
	public:
		explicit Toon(ID3D11Device* device);
		~Toon() override = default;

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
}

#endif // __cplusplus

#endif // TOON_H