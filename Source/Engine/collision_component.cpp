#include "collision_component.h"

#include "scene.h"
#include "cereal_helper.h"

namespace cumulonimbus::component
{
	CollisionComponent::CollisionComponent(ecs::Registry* registry,
		mapping::rename_type::Entity ent, CollisionTag tag)
		:ComponentBase{ registry,ent }
	{

	}

	void CollisionComponent::SetCollisionTag(CollisionTag tag)
	{
		collision_tag = tag;
	}

	CollisionTag CollisionComponent::GetCollisionTag() const
	{
		return collision_tag;
	}

	collision::HitResult& CollisionComponent::GetHitResult()
	{
		return hit_result;
	}

} // cumulonimbus::component