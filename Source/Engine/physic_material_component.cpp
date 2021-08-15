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

	void PhysicMaterialComponent::Save(const std::string& file_path)
	{
	}

	void PhysicMaterialComponent::Load(const std::string& file_path_and_name)
	{
	}
} // cumulonimbus::component
