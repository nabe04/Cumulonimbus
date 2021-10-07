#include "actor3d_component.h"

namespace cumulonimbus::component
{
	Actor3DComponent::Actor3DComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		: ComponentBase{ registry,ent }
	{

	}

	Actor3DComponent::Actor3DComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const Actor3DComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}
} // cumulonimbus::component
