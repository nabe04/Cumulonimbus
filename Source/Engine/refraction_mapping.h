#pragma once

#include "imgui.h"

#include "shader.h"

namespace shader
{
	struct M_Refraction
	{// (b6)
		DirectX::XMFLOAT4 refractive_index;	// ã¸ê‹ó¶(xê¨ï™ÇÃÇ›édól)(0.0 Å` 1.0)

		void EditParam()
		{
			ImGui::DragFloat("RefractieIndex", &refractive_index.x, 0.005f, 0.0f, 1.0f);
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