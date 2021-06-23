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
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::Phong_PS().c_str());
		}
	}

	namespace shader_asset
	{
		PhongAsset::PhongAsset()
			:ShaderAsset{}
		{
			cb_phong = std::make_unique<buffer::ConstantBuffer<PhongCB>>(Locator::GetDx11Device()->device.Get());

			// ‰ŠúÝ’è
			cb_phong->data.phong_specular			= 1.0f;
			cb_phong->data.phong_ref_coefficient	= 1.0f;
			cb_phong->data.phong_pow_value			= 1.0f;
		}

		void PhongAsset::BindCBuffer()
		{
			cb_phong->Activate(Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Phong);
		}

		void PhongAsset::UnbindCBuffer()
		{
			cb_phong->Deactivate(Locator::GetDx11Device()->immediate_context.Get());
		}

		void PhongAsset::RenderImGui()
		{
			ImGui::DragFloat("Specular"					, &cb_phong->data.phong_specular		, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Reflection coefficient"	, &cb_phong->data.phong_ref_coefficient	, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("Power"					, &cb_phong->data.phong_pow_value		, 0.5f	, 1.0f, 100.0f);
		}


	}
}

namespace shader
{
	Phong::Phong(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
		pixel_shader  = std::make_unique<PixelShader>(device, ps_name);
	}

	void Phong::Activate(ID3D11DeviceContext* immediate_context)
	{
		material_data->Activate(immediate_context, 6);

		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void Phong::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);

		material_data->Deactivate(immediate_context);
	}
}

