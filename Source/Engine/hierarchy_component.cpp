#include "hierarchy_component.h"

namespace cumulonimbus::component
{
	HierarchyComponent::HierarchyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	void HierarchyComponent::CommonUpdate(float dt)
	{
	}

	void HierarchyComponent::Load(ecs::Registry* registry)
	{

	}

} // cumulonimbus::component
