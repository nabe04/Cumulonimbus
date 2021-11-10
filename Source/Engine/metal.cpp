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
			ImGui::DragFloat("Roughness", &cb_metal->GetData().metal_roughness, 0.01f, 0.0f, 1.0f);
		}

	}
}

