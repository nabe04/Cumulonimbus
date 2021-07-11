#include "actor3d_component.h"

namespace cumulonimbus::component
{
	Actor3DComponent::Actor3DComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		: ComponentBase{ registry,ent }
	{

	}

} // cumulonimbus::component
