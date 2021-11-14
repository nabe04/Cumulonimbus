#include "standard_sprite.h"

#include <array>

#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		StandardSpriteShader::StandardSpriteShader()
			:Shader{}
		{
			// InputLayoutÇÃê›íË
			std::array<D3D11_INPUT_ELEMENT_DESC, 2 > layout =
			{
			 {
				{ "POSITION",	0 , DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",	0 , DXGI_FORMAT_R32G32_FLOAT ,		0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			 }
			};

			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::StandardSprite_VS().c_str(), layout.data(), layout.size());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::StandardSprite_PS().c_str());
		}

	} // shader_system

	namespace shader_asset
	{
		StandardSpriteAsset::StandardSpriteAsset()
			:ShaderAsset{}
		{
			cb_sprite = std::make_unique<buffer::ConstantBuffer<SpriteCB>>(locator::Locator::GetDx11Device()->device.Get());

			// èâä˙ê›íË
			cb_sprite->GetData().sprite_offset = DirectX::SimpleMath::Vector2{};
		}

		void StandardSpriteAsset::BindCBuffer()
		{
			cb_sprite->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Sprite);
		}

		void StandardSpriteAsset::UnbindCBuffer()
		{
			cb_sprite->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
		}

		void StandardSpriteAsset::RenderImGui()
		{

		}

	} // shader_asset
} // cumulonimbus
