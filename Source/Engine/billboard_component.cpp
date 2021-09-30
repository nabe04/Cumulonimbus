#include "billboard_component.h"

#include "locator.h"

namespace cumulonimbus::component
{
	BillboardComponent::BillboardComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:SpriteComponent{ registry,ent }
	{
	}

	BillboardComponent::BillboardComponent()
		:SpriteComponent{}
	{

	}

	void BillboardComponent::SceneUpdate(float dt)
	{
		// SceneViewカメラに対してのビルボード

	}

	void BillboardComponent::GameUpdate(float dt)
	{
		// Game Viewカメラ(メインカメラ)に対してのビルボード
	}
} // cumulonimbus::component
