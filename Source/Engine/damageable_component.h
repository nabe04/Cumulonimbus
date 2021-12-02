#pragma once
#include <functional>

#include "event.h"
#include "component_base.h"

namespace cumulonimbus::component
{
	struct DamageData
	{
		u_int damage_amount{};
	};

	class DamageableComponent final : public ComponentBase
	{
	public:
		explicit DamageableComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit DamageableComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const DamageableComponent& damageable_comp);
		explicit DamageableComponent()  = default; // for cereal
		~DamageableComponent() override = default;

		void Load(ecs::Registry* registry) override;

		void RegistryDamageEvent(const mapping::rename_type::UUID& id, const std::function<void(const DamageData&)>& func);
		void OnDamaged(const mapping::rename_type::UUID& id, const DamageData& damage_data);
	
	private:
		system::Event<void, const DamageData&> damage_event{};
	};
} // cumulonimbus::component
