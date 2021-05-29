#pragma once

#include <memory>
#include <wrl.h>
#include <cereal/cereal.hpp>

#include "shader.h"

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