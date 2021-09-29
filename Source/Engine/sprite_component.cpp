#include "sprite_component.h"

#include "arithmetic.h"
#include "locator.h"
#include "texture_loader.h"
#include "standard_sprite.h"

namespace cumulonimbus::component
{
	template <class Archive>
	void SpriteComponent::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(texture_id),
			CEREAL_NVP(graphics_state),
			CEREAL_NVP(pivot_type),
			CEREAL_NVP(vertices)
		);
	}

	SpriteComponent::SpriteComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const mapping::rename_type::UUID& tex_id)
		: ComponentBase{ registry,ent }
	{
		vertices.at(0).texcoord = { .0f,.0f };
		vertices.at(1).texcoord = { 1.f,.0f };
		vertices.at(2).texcoord = { .0f,1.f };
		vertices.at(3).texcoord = { 1.f,1.f };

		shader_asset = std::make_shared<shader_asset::ShaderAsset2DManager>();
		asset::TextureLoader* texture_loader = locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>();
		const auto& texture = texture_loader->GetTexture(tex_id);
		CreateVertexBuffer();
		ResizeTexture({ static_cast<float>(texture.GetWidth()),static_cast<float>(texture.GetHeight()) });
		SetPivotType(render::PivotType::Center); // ピボットのデフォルト設定(PivotType::Center)
	}

	void SpriteComponent::Load(ecs::Registry* registry)
	{

	}

	void SpriteComponent::ResizeTexture(const DirectX::SimpleMath::Vector2& size)
	{
		const Window* window = locator::Locator::GetWindow();
		const DirectX::SimpleMath::Vector2 screen_size = arithmetic::ConvertScreenToNDC(size, static_cast<float>(window->Width()), static_cast<float>(window->Height()));

		vertices.at(0).position = {			  .0f,   		 .0f,.0f,1.f };
		vertices.at(1).position = { screen_size.x,		     .0f,.0f,1.f };
		vertices.at(2).position = {			  .0f,-screen_size.y,.0f,1.f };
		vertices.at(3).position = { screen_size.x,-screen_size.y,.0f,1.f };

		locator::Locator::GetDx11Device()->immediate_context->UpdateSubresource(vertex_buffer.Get(), 0, nullptr, vertices.data(), 0, 0);
	}

	void SpriteComponent::SetPivotType(const render::PivotType pivot)
	{
		asset::TextureLoader* texture_loader = locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>();
		const auto& texture = texture_loader->GetTexture(texture_id);
		const Window* window = locator::Locator::GetWindow();
		const DirectX::SimpleMath::Vector2 screen_size = arithmetic::ConvertScreenToNDC({ static_cast<float>(texture.GetWidth()),static_cast<float>(texture.GetHeight()) },
																						  static_cast<float>(window->Width()), static_cast<float>(window->Height()));
		auto& cb_data = shader_asset->GetShaderAsset<shader_asset::StandardSpriteAsset>()->GetCBuffer()->data;
		pivot_type = pivot;

		switch(pivot_type)
		{
		case render::PivotType::Center:
			cb_data.sprite_offset = { -screen_size.x / 2.f,screen_size.y / 2.f };
			break;
		case render::PivotType::BottomCenter:
			cb_data.sprite_offset = { -screen_size.x / 2.f,screen_size.y };
			break;
		case render::PivotType::TopCenter:
			cb_data.sprite_offset = { screen_size.x / 2.f , .0f };
			break;
		case render::PivotType::LeftTop:
			cb_data.sprite_offset = { .0f,.0f };
			break;
		case render::PivotType::LeftBottom:
			cb_data.sprite_offset = { -screen_size.x , .0f };
			break;
		case render::PivotType::RightTop:
			cb_data.sprite_offset = { screen_size.x ,.0f };
			break;
		case render::PivotType::RightBottom:
			cb_data.sprite_offset = { screen_size.x,screen_size.y };
			break;
		case render::PivotType::End:
			assert(!"Type does not exist(SpriteComponent::SetPivotType)");
			break;
		default:
			assert(!"Type does not exist(SpriteComponent::SetPivotType)");
			break;
		}
	}

	void SpriteComponent::CreateVertexBuffer()
	{
		D3D11_BUFFER_DESC bd{};
		bd.Usage			= D3D11_USAGE_DEFAULT;
		bd.ByteWidth		= sizeof(shader::VertexSprite) * static_cast<u_int>(vertices.size());
		bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags	= 0;
		bd.MiscFlags		= 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = vertices.data();

		HRESULT hr = locator::Locator::GetDx11Device()->device->CreateBuffer(&bd, &initData, vertex_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateBuffer Error (Texture2DComponent class)");
	}
} // cumulonimbus::component
