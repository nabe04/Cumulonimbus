#include "enemy_base_component.h"

namespace cumulonimbus::component
{
	EnemyBaseComponent::EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:Actor3DComponent{ registry,ent }
	{
	}
} // cumulonimbus::component
