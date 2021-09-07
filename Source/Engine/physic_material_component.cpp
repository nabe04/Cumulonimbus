#include "physic_material_component.h"

namespace cumulonimbus::component
{
	PhysicMaterialComponent::PhysicMaterialComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	void PhysicMaterialComponent::Update(float dt)
	{
	}

	void PhysicMaterialComponent::RenderImGui()
	{
	}

	void PhysicMaterialComponent::Load(ecs::Registry* registry)
	{
	}
} // cumulonimbus::component
