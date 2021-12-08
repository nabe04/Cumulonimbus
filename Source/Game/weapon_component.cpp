#include "weapon_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::WeaponComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::Actor3DComponent, cumulonimbus::component::WeaponComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::WeaponComponent, 0);

namespace cumulonimbus::component
{
	template <class Archive>
	void WeaponComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<Actor3DComponent>(this)
		);
	}

	template <class Archive>
	void WeaponComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<Actor3DComponent>(this)
		);
	}

	WeaponComponent::WeaponComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:Actor3DComponent{ registry,ent }
	{
	}

	WeaponComponent::WeaponComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const WeaponComponent& copy_comp)
		: Actor3DComponent{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	WeaponComponent::WeaponComponent(const mapping::component_tag::ComponentTag tag)
		:Actor3DComponent{ tag }
	{
	}

	void WeaponComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}
} // cumulonimbus::component
