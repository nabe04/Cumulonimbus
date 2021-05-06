#include "toon.h"

#include "imgui.h"

namespace shader
{
	void CB_Toon::Edit()
	{
		ImGui::SliderInt("Tone", &tones, 1, 32);
		ImGui::DragFloat("Reflection coefficient", &reflectance, 0.005f, 0.0f, 1.0f);
		ImGui::DragFloat("Power", &power, 0.5f, 1.0f, 100.0f);
	}

	Toon::Toon(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
		pixel_shader  = std::make_unique<PixelShader>(device, ps_name);
	}

	void Toon::Activate(ID3D11DeviceContext* immediate_context)
	{
		material_data->Activate(immediate_context, 6, false, true);

		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void Toon::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);

		material_data->Deactivate(immediate_context);
	}
}