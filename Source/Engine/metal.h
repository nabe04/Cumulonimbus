#pragma once
#include <cereal/cereal.hpp>

#include "shader.h"

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
