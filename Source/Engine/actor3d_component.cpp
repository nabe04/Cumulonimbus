#include "actor3d_component.h"

namespace cumulonimbus::component
{
	Actor3DComponent::Actor3DComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		: ComponentBase{ registry,ent }
	{

	}

	Actor3DComponent::Actor3DComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const Actor3DComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	Actor3DComponent::Actor3DComponent(const mapping::component_tag::ComponentTag tag)
		:ComponentBase{ tag }
	{

	}

} // cumulonimbus::component
