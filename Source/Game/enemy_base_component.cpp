#include "enemy_base_component.h"

#include "arithmetic.h"

namespace cumulonimbus::component
{
	void EnemyBaseComponent::RangeTimerFloat::SetRandomVal()
	{
		random_val = arithmetic::RandomFloatInRange(min, max);
	}

	EnemyBaseComponent::EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:Actor3DComponent{ registry,ent }
	{
	}
} // cumulonimbus::component
