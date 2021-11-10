#include "phong.h"

#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		PhongShader::PhongShader()
			:Shader{}
		{
			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::Phong_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::PhongMTR_PS().c_str());
		}
	} //shader_system

	namespace shader_asset
	{
		PhongAsset::PhongAsset()
			:ShaderAsset{}
		{
			cb_phong = std::make_unique<buffer::ConstantBuffer<PhongCB>>(locator::Locator::GetDx11Device()->device.Get());

			// ‰ŠúÝ’è
			cb_phong->GetData().phong_specular = 1.0f;
			cb_phong->GetData().phong_ref_coefficient = 1.0f;
			cb_phong->GetData().phong_pow_value = 1.0f;
		}

		void PhongAsset::BindCBuffer()
		{
			cb_phong->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Phong);
		}

		void PhongAsset::UnbindCBuffer()
		{
			cb_phong->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
		}

		void PhongAsset::RenderImGui()
		{
			ImGui::DragFloat("Specular"					, &cb_phong->GetData().phong_specular		, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Reflection coefficient"	, &cb_phong->GetData().phong_ref_coefficient	, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Power"					, &cb_phong->GetData().phong_pow_value		, 0.5f	, 1.0f, 100.0f);
		}
	} // shader_asset
} // cumulonimbus