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
		// SceneView�J�����ɑ΂��Ẵr���{�[�h

	}

	void BillboardComponent::GameUpdate(float dt)
	{
		// Game View�J����(���C���J����)�ɑ΂��Ẵr���{�[�h
	}
} // cumulonimbus::component
