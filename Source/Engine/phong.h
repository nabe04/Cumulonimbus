#pragma once

#include <cereal/cereal.hpp>
#include <imgui.h>

#include "shader.h"

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
