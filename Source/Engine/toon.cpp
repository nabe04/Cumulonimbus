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
			cb_toon = std::make_unique<buffer::ConstantBuffer<ToonCB>>(locator::Locator::GetDx11Device()->device.Get());
		}

		void ToonAsset::BindCBuffer()
		{
			cb_toon->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Toon);
		}

		void ToonAsset::UnbindCBuffer()
		{
			cb_toon->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
		}

		void ToonAsset::RenderImGui()
		{
			ImGui::SliderInt("Tone"					 , &cb_toon->data.toon_tones		, 1		, 32);
			ImGui::DragFloat("Reflection coefficient", &cb_toon->data.toon_reflectance	, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Power"				 , &cb_toon->data.toon_power		, 0.5f, 1.0f, 100.0f);
		}
	}
}