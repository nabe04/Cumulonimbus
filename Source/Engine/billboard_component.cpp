#include "billboard_component.h"

#include "billboard.h"
#include "locator.h"
CEREAL_REGISTER_TYPE(cumulonimbus::component::BillboardComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::SpriteComponent, cumulonimbus::component::BillboardComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::BillboardComponent, 0);

namespace cumulonimbus::component
{
	template <class Archive>
	void BillboardComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<SpriteComponent>(this),
			CEREAL_NVP(rendering_target)
		);
	}

	template <class Archive>
	void BillboardComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<SpriteComponent>(this),
			CEREAL_NVP(rendering_target)
		);
	}

	BillboardComponent::BillboardComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:SpriteComponent{ registry,ent }
	{
		auto entity = GetEntity();
		int a{};
	}

	BillboardComponent::BillboardComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const BillboardComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	BillboardComponent::BillboardComponent()
		:SpriteComponent{}
	{

	}

	void BillboardComponent::CommonUpdate(const float dt)
	{
		auto* billboard_asset = shader_asset_manager.GetShaderAsset<shader_asset::BillboardAsset>();
		//billboard_asset->GetCBuffer()->data.billboard_translation_matrix =
	}

	void BillboardComponent::PostCommonUpdate(float dt)
	{
		auto* billboard_asset = shader_asset_manager.GetShaderAsset<shader_asset::BillboardAsset>();
		//billboard_asset->GetCBuffer()->data.billboard_translation_matrix =
	}

	void BillboardComponent::SceneUpdate(const float dt)
	{
		// SceneViewカメラに対してのビルボード

	}

	void BillboardComponent::GameUpdate(const float dt)
	{
		// Game Viewカメラ(メインカメラ)に対してのビルボード
	}
} // cumulonimbus::component
