#include "damageable_component.h"

#include "collision_component.h"

namespace cumulonimbus::component
{
	DamageableComponent::DamageableComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{registry,ent}
	{

	}

	DamageableComponent::DamageableComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const DamageableComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void DamageableComponent::Load(ecs::Registry* registry)
	{

	}

	void DamageableComponent::RegistryDamageEvent(const mapping::rename_type::UUID& ent, const std::function<void(const DamageData&, const collision::HitResult&)>& func)
	{
		damage_event.RegistryEvent(ent, func);
	}

	void DamageableComponent::OnDamaged(const mapping::rename_type::UUID& id, const DamageData& damage_data, const collision::HitResult& hit_result)
	{
		damage_event.Invoke(id, damage_data, hit_result);
	}
} // cumulonimbus::component
