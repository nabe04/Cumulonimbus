#include "collision_component.h"

#include "scene.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::CollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::CollisionComponent)

namespace cumulonimbus::component
{
	template <class Archive>
	void CollisionComponent::serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(collision_tag)
		);
	}

	void CollisionComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	CollisionComponent::CollisionComponent(ecs::Registry* registry,
		mapping::rename_type::Entity ent, CollisionTag tag)
		:ComponentBase{ registry,ent }
	{

	}

	CollisionComponent::CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CollisionComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void CollisionComponent::SetCollisionTag(CollisionTag tag)
	{
		collision_tag = tag;
	}

	CollisionTag CollisionComponent::GetCollisionTag() const
	{
		return collision_tag;
	}
} // cumulonimbus::component