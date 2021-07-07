#include "metal.h"

#include "shader_filename_mapping.h"
#include "locator.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		MetalShader::MetalShader()
			: Shader{}
		{
			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::Metal_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::MetalMTR_PS().c_str());
		}

	}

	namespace shader_asset
	{
		MetalAsset::MetalAsset()
			: ShaderAsset{}
		{
			cb_metal = std::make_unique<buffer::ConstantBuffer<MetalCB>>(locator::Locator::GetDx11Device()->device.Get());
		}

		void MetalAsset::BindCBuffer()
		{
			cb_metal->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Metal);
		}

		void MetalAsset::UnbindCBuffer()
		{
			cb_metal->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
		}


		void MetalAsset::RenderImGui()
		{
			ImGui::DragFloat("Roughness", &cb_metal->data.metal_roughness, 0.01f, 0.0f, 1.0f);
		}

	}
}

namespace shader
{
	Metal::Metal(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
		pixel_shader = std::make_unique<PixelShader>(device, ps_name);
	}

	void Metal::Activate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);

		material_data->Activate(immediate_context, 6);
	}

	void Metal::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		material_data->Deactivate(immediate_context);

		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);
	}
}

