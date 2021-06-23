#include "toon.h"

#include <imgui.h>

#include "shader_filename_mapping.h"
#include "locator.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		ToonShader::ToonShader()
			:Shader{}
		{
			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::Toon_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::Toon_PS().c_str());
		}

	}

	namespace shader_asset
	{
		ToonAsset::ToonAsset()
			:ShaderAsset{}
		{
			cb_toon = std::make_unique<buffer::ConstantBuffer<ToonCB>>(Locator::GetDx11Device()->device.Get());
		}

		void ToonAsset::BindCBuffer()
		{
			cb_toon->Activate(Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Toon);
		}

		void ToonAsset::UnbindCBuffer()
		{
			cb_toon->Deactivate(Locator::GetDx11Device()->immediate_context.Get());
		}

		void ToonAsset::RenderImGui()
		{
			ImGui::SliderInt("Tone"					 , &cb_toon->data.toon_tones		, 1		, 32);
			ImGui::DragFloat("Reflection coefficient", &cb_toon->data.toon_reflectance	, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Power"				 , &cb_toon->data.toon_power		, 0.5f, 1.0f, 100.0f);
		}
	}
}

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