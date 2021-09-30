#include "billboard.h"

#include <array>

#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		BillboardShader::BillboardShader()
			:Shader{}
		{
			// Setting the InputLayout
			std::array<D3D11_INPUT_ELEMENT_DESC, 2 > layout =
			{
			 {
				{ "POSITION",	0 , DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",	0 , DXGI_FORMAT_R32G32_FLOAT ,		0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			 }
			};

			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::Billboard_VS().c_str(), layout.data(), layout.size());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::Billboard_PS().c_str());
		}
	} // shader_system

	namespace shader_asset
	{
		BillboardAsset::BillboardAsset()
			:ShaderAsset{}
		{
			cb_billboard = std::make_unique<buffer::ConstantBuffer<BillboardCB>>(locator::Locator::GetDx11Device()->device.Get());

			// ‰ŠúÝ’è
			cb_billboard->data.billboard_offset = DirectX::SimpleMath::Vector2{};
		}

		void BillboardAsset::BindCBuffer()
		{
			cb_billboard->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Billboard);
		}

		void BillboardAsset::UnbindCBuffer()
		{
			cb_billboard->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
		}

		void BillboardAsset::RenderImGui()
		{
		}
	} // shader_asset
} // cumulonimbus
