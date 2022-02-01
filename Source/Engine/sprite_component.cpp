#include "sprite_component.h"

#include <cereal/types/array.hpp>

#include "arithmetic.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "locator.h"
#include "texture_loader.h"
#include "standard_sprite.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::SpriteComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::SpriteComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::SpriteComponent, 1)
CEREAL_CLASS_VERSION(cumulonimbus::component::SpriteComponent::Texcoord, 0)

namespace
{
	// ImGui�`�掞��PivotType(enum class)���當����ꗗ���擾����ׂɎg�p
	EnumStateMap<cumulonimbus::render::PivotType> pivot_preset{};
	// ImGui�`�掞��CollisionTag(enum class)���當����ꗗ���擾����ׂɎg�p
	//EnumStateMap<cumulonimbus::collision::CollisionTag> collision_tag{};
}


namespace cumulonimbus::component
{
	template <class Archive>
	void SpriteComponent::Texcoord::load(Archive&& archive, const uint32_t version)
	{
		archive(
			CEREAL_NVP(left_top),
			CEREAL_NVP(left_bottom),
			CEREAL_NVP(right_top),
			CEREAL_NVP(right_bottom)
		);
	}

	template <class Archive>
	void SpriteComponent::Texcoord::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			CEREAL_NVP(left_top),
			CEREAL_NVP(left_bottom),
			CEREAL_NVP(right_top),
			CEREAL_NVP(right_bottom)
		);
	}

	template <class Archive>
	void SpriteComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(texture_id),
			CEREAL_NVP(shader_asset_manager),
			CEREAL_NVP(graphics_state),
			CEREAL_NVP(pivot_type),
			CEREAL_NVP(vertices)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(cb_sprite)
			);
		}
	}

	template <class Archive>
	void SpriteComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(texture_id),
			CEREAL_NVP(shader_asset_manager),
			CEREAL_NVP(graphics_state),
			CEREAL_NVP(pivot_type),
			CEREAL_NVP(vertices)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(cb_sprite)
			);
		}
	}



	SpriteComponent::SpriteComponent()
		:ComponentBase{}
	{

	}

	SpriteComponent::SpriteComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent)
		: ComponentBase{ registry,ent }
	{
		if (!cb_sprite.get())
			cb_sprite = std::make_unique<buffer::ConstantBuffer<SpriteCB>>(locator::Locator::GetDx11Device()->device.Get());
		cb_sprite->CreateCBuffer(locator::Locator::GetDx11Device()->device.Get());

		// �����e�N�X�`���̃Z�b�g(�_�~�[�e�N�X�`��)
		asset::TextureLoader* texture_loader = locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>();
		const auto& texture = texture_loader->GetTexture({});
		Initialize(static_cast<float>(texture.GetWidth()), static_cast<float>(texture.GetHeight()));
	}

	SpriteComponent::SpriteComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const SpriteComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	SpriteComponent::SpriteComponent(const mapping::component_tag::ComponentTag tag)
		:ComponentBase{ tag }
	{

	}

	SpriteComponent::SpriteComponent(const SpriteComponent& other)
		: ComponentBase{ other },
		 texture_id{other.texture_id},
		 graphics_state{other.graphics_state},
		 pivot_type{other.pivot_type},
		 vertices{other.vertices}
	{
		if (!cb_sprite.get())
			cb_sprite = std::make_unique<buffer::ConstantBuffer<SpriteCB>>(locator::Locator::GetDx11Device()->device.Get());
		cb_sprite->CreateCBuffer(locator::Locator::GetDx11Device()->device.Get());

		asset::TextureLoader* texture_loader = locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>();
		const auto& texture = texture_loader->GetTexture(texture_id);
		Initialize(static_cast<float>(texture.GetWidth()), static_cast<float>(texture.GetHeight()));
	}

	SpriteComponent& SpriteComponent::operator=(const SpriteComponent& other)
	{
		if(this == &other)
		{
			return *this;
		}

		texture_id		= other.texture_id;
		graphics_state	= other.graphics_state;
		pivot_type		= other.pivot_type;
		vertices		= other.vertices;

		if (!cb_sprite.get())
			cb_sprite = std::make_unique<buffer::ConstantBuffer<SpriteCB>>(locator::Locator::GetDx11Device()->device.Get());
		cb_sprite->CreateCBuffer(locator::Locator::GetDx11Device()->device.Get());


		asset::TextureLoader* texture_loader = locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>();
		const auto& texture = texture_loader->GetTexture(texture_id);
		Initialize(static_cast<float>(texture.GetWidth()), static_cast<float>(texture.GetHeight()));

		return *this;
	}

	void SpriteComponent::Initialize(const float width, const float height)
	{
		vertices.at(0).texcoord = { .0f,.0f };
		vertices.at(1).texcoord = { 1.f,.0f };
		vertices.at(2).texcoord = { .0f,1.f };
		vertices.at(3).texcoord = { 1.f,1.f };

		shader_asset_manager.SetCurrentShaderAsset<shader_asset::StandardSpriteAsset>();
		CreateVertexBuffer();
		ResizeTexture({ width,height });
		SetPivotType(render::PivotType::Center); // �s�{�b�g�̃f�t�H���g�ݒ�(PivotType::Center)
	}

	void SpriteComponent::CommonUpdate(const float dt)
	{
		ConvertScreenToNDC();
		auto& cb_data = shader_asset_manager.GetShaderAsset<shader_asset::StandardSpriteAsset>()->GetCBuffer()->GetData();
	}

	void SpriteComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);

		// �ۑ�����Ă���NDC��ԏ�̍��W���猻�݂̃X�N���[����ԏ�̍��W���Z�o����
		asset::TextureLoader* texture_loader = locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>();
		const auto& texture = texture_loader->GetTexture(texture_id);
		Initialize(static_cast<float>(texture.GetWidth()), static_cast<float>(texture.GetHeight()));
	}

	void SpriteComponent::RenderImGui()
	{
		if (ImGui::CollapsingHeader("Sprite Component", ImGuiTreeNodeFlags_DefaultOpen))
		{
			asset::AssetManager& asset_manager = *locator::Locator::GetAssetManager();
			// �e�N�X�`���̑I��
			asset_manager.GetLoader<asset::TextureLoader>()->SelectableTexture(asset_manager, texture_id);
			// �s�{�b�g�̐ݒ�
			if (std::string current_name = nameof::nameof_enum(pivot_type).data();
				helper::imgui::Combo("Pivot", current_name, pivot_preset.GetStateNames()))
			{
				pivot_type = pivot_preset.GetStateMap().at(current_name);
				SetPivotType(pivot_type);
			}
			auto& cbuff_data = cb_sprite->GetData();
			// Tiling�̐ݒ�
			ImGui::DragFloat2("Tiling", &cbuff_data.sprite_tiling.x, 0.1f, FLT_MIN, FLT_MAX);
			// Offset�̐ݒ�
			ImGui::DragFloat2("Offset", &cbuff_data.sprite_offset.x, 0.1f, FLT_MIN, FLT_MAX);
			// Texcoord�̕ύX
			ImGui::Text("Texcoord");
			ImGui::DragFloat2("Left Top"    , &texcoord.left_top.x    , 0.01f, .0f, 1.f);
			ImGui::DragFloat2("Left Bottom" , &texcoord.left_bottom.x , 0.01f, .0f, 1.f);
			ImGui::DragFloat2("Right Top"   , &texcoord.right_top.x   , 0.01f, .0f, 1.f);
			ImGui::DragFloat2("Right Bottom", &texcoord.right_bottom.x, 0.01f, .0f, 1.f);
		}
	}

	void SpriteComponent::ResizeTexture(const DirectX::SimpleMath::Vector2& size)
	{
		const Window* window = locator::Locator::GetWindow();
		const float win_half_width  = window->Width() / 2.f;
		const float win_half_height = window->Height() / 2.f;
		auto convert_ndc = [&](const DirectX::SimpleMath::Vector2& tex_size)
		{
			const DirectX::SimpleMath::Vector2 screen_pos = arithmetic::ConvertScreenToNDC(tex_size, static_cast<float>(window->Width()), static_cast<float>(window->Height()));
			return DirectX::SimpleMath::Vector4{ screen_pos.x,screen_pos.y,.0f,1.f };
		};

		// ndc��ԏ�ł̃e�N�X�`���T�C�Y
		const DirectX::SimpleMath::Vector2 ndc_size = arithmetic::ConvertScreenToNDC(size, static_cast<float>(window->Width()), static_cast<float>(window->Height()));

		vertices.at(0).position = convert_ndc({ win_half_width - size.x,  win_half_height - size.y });	// ����
		vertices.at(1).position = convert_ndc({ win_half_width + size.x,  win_half_height - size.y });	// �E��
		vertices.at(2).position = convert_ndc({ win_half_width - size.x,  win_half_height + size.y });	// ����
		vertices.at(3).position = convert_ndc({ win_half_width + size.x,  win_half_height + size.y });	// �E��

		locator::Locator::GetDx11Device()->immediate_context->UpdateSubresource(vertex_buffer.Get(), 0, nullptr, vertices.data(), 0, 0);
	}

	void SpriteComponent::SetPivotType(const render::PivotType type)
	{
		asset::TextureLoader* texture_loader = locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>();
		const auto& texture = texture_loader->GetTexture(texture_id);
		const DirectX::SimpleMath::Vector2 texture_size = DirectX::SimpleMath::Vector2{ static_cast<float>(texture.GetWidth()),static_cast<float>(texture.GetHeight()) };
		const Window* window = locator::Locator::GetWindow();
		const float win_half_width  = window->Width() / 2.f;
		const float win_half_height = window->Height() / 2.f;
		const DirectX::SimpleMath::Vector2 tex_size = arithmetic::ConvertScreenToNDC({win_half_width + static_cast<float>(texture.GetWidth()),
																					  win_half_height + static_cast<float>(texture.GetHeight()) },
																					  static_cast<float>(window->Width()),
																					  static_cast<float>(window->Height()));
		auto& cb_data = shader_asset_manager.GetShaderAsset<shader_asset::StandardSpriteAsset>()->GetCBuffer()->GetData();
		pivot_type = type;

		switch(pivot_type)
		{
		case render::PivotType::Center:
			pivot = DirectX::SimpleMath::Vector2{ texture_size.x / 2.f,texture_size.y / 2.f };
			break;
		case render::PivotType::BottomCenter:
			pivot = DirectX::SimpleMath::Vector2{ texture_size.x / 2.f,texture_size.y };
			break;
		case render::PivotType::TopCenter:
			pivot = DirectX::SimpleMath::Vector2{ texture_size.x / 2.f,.0f };
			break;
		case render::PivotType::LeftTop:
			pivot = DirectX::SimpleMath::Vector2{ .0f,.0f };
			break;
		case render::PivotType::LeftBottom:
			pivot = DirectX::SimpleMath::Vector2{ .0f , texture_size.y };
			break;
		case render::PivotType::RightTop:
			pivot = DirectX::SimpleMath::Vector2{ texture_size.x ,.0f };
			break;
		case render::PivotType::RightBottom:
			pivot = DirectX::SimpleMath::Vector2{ texture_size.x, texture_size.y };
			break;
		case render::PivotType::End:
			assert(!"Type does not exist(SpriteComponent::SetPivotType)");
			break;
		default:
			assert(!"Type does not exist(SpriteComponent::SetPivotType)");
			break;
		}
	}

	void SpriteComponent::SetTexcoord(const render::TexcoordType type, const DirectX::SimpleMath::Vector2& tex_pos)
	{
		DirectX::SimpleMath::Vector2 pos{};
		pos.x = arithmetic::Clamp(tex_pos.x, .0f, 1.f);
		pos.y = arithmetic::Clamp(tex_pos.y, .0f, 1.f);

		if(type == render::TexcoordType::LeftTop)
		{
			texcoord.left_top = pos;
		}
		else if(type == render::TexcoordType::LeftBottom)
		{
			texcoord.left_bottom = pos;
		}
		else if(type == render::TexcoordType::RightTop)
		{
			texcoord.right_top = pos;
		}
		else
		{
			texcoord.right_bottom = pos;
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

		const HRESULT hr = locator::Locator::GetDx11Device()->device->CreateBuffer(&bd, &initData, vertex_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateBuffer Error (Texture2DComponent class)");
	}

	void SpriteComponent::ConvertScreenToNDC() const
	{
		const DirectX::SimpleMath::Vector3 world_pos = GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).GetPosition();
		const Window& window = *locator::Locator::GetWindow();
		const DirectX::SimpleMath::Vector2 ndc_pos = arithmetic::ConvertScreenToNDC(world_pos, static_cast<float>(window.Width()), static_cast<float>(window.Height()));
		//GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).SetPosition({ ndc_pos.x,ndc_pos.y,.0f });
	}
} // cumulonimbus::component
