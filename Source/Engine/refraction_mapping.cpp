#include "refraction_mapping.h"

#include <imgui.h>

#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		RefractionMappingShader::RefractionMappingShader()
			:Shader{}
		{
			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::RefractionMapping_VS().c_str());
			pixel_shader = std::make_unique<shader_system::PixelShader>(ps::RefractionMapping_PS().c_str());
		}
	}

	namespace shader_asset
	{
		RefractionMappingAsset::RefractionMappingAsset(u_int gbuff_shader_slot)
			: ShaderAsset{ gbuff_shader_slot }
		{
			cb_refraction = std::make_unique<buffer::ConstantBuffer<RefractionCB>>(locator::Locator::GetDx11Device()->device.Get());

			// ‰ŠúÝ’è
			cb_refraction->data.refraction_refractive_index = 0.5f;
		}

		void RefractionMappingAsset::BindCBuffer()
		{
			cb_refraction->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_RefractionMapping);
		}

		void RefractionMappingAsset::UnbindCBuffer()
		{
			cb_refraction->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
		}

		void RefractionMappingAsset::RenderImGui()
		{
			ImGui::DragFloat("RefractieIndex", &cb_refraction->data.refraction_refractive_index, 0.005f, 0.0f, 1.0f);
		}
	}
}

namespace shader
{
	RefractionMapping::RefractionMapping(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
		pixel_shader = std::make_unique<PixelShader>(device, ps_name);
	}

	void RefractionMapping::Activate(ID3D11DeviceContext* immediate_context)
	{
		material_data->Activate(immediate_context, 6);

		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void RefractionMapping::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);

		material_data->Deactivate(immediate_context);
	}
}

