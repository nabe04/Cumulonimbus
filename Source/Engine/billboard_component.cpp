#include "billboard_component.h"

#include "billboard.h"
#include "locator.h"

namespace cumulonimbus::component
{
	BillboardComponent::BillboardComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:SpriteComponent{ registry,ent }
	{
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
		auto* billboard_asset = shader_asset_manager->GetShaderAsset<shader_asset::BillboardAsset>();
		//billboard_asset->GetCBuffer()->data.billboard_translation_matrix =
	}

	void BillboardComponent::PostCommonUpdate(float dt)
	{
		auto* billboard_asset = shader_asset_manager->GetShaderAsset<shader_asset::BillboardAsset>();
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
