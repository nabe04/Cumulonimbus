#include "single_color.h"

#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		SingleColorShader::SingleColorShader()
			:Shader{}
		{
			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::SingleColor_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::SampleShaderMTR_PS().c_str());
		}
	} // shader_system

	namespace shader_asset
	{
		SingleColorAsset::SingleColorAsset()
			:ShaderAsset{}
		{
			cb_single_color = std::make_unique<buffer::ConstantBuffer<SingleColorCB>>(locator::Locator::GetDx11Device()->device.Get());

			// 初期設定
			cb_single_color->data.single_color = { 1.f,1.f,1.f,1.f };
		}

		void SingleColorAsset::BindCBuffer()
		{
			cb_single_color->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_SingleColor);
		}

		void SingleColorAsset::UnbindCBuffer()
		{
			cb_single_color->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
		}

		void SingleColorAsset::RenderImGui()
		{
			ImGui::ColorEdit4("Color", (float*)&cb_single_color->data.single_color);
		}
	} // shader_asset
} // cumulonimbus