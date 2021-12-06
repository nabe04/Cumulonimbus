#pragma once
#include "weapon_component.h"

namespace cumulonimbus::component
{
	class SoldierSwordComponent final : public WeaponComponent
	{
	public:
		explicit SoldierSwordComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit SoldierSwordComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SoldierSwordComponent& copy_comp); // for prefab
		explicit SoldierSwordComponent(mapping::component_tag::ComponentTag tag);
		explicit SoldierSwordComponent() = default; // for cereal
		~SoldierSwordComponent() override = default;

		void Start() override;

		void RenderImGui() override;

		void Load(ecs::Registry * registry) override;

		template<class Archive>
		void save(Archive && archive, uint32_t version) const;

		template<class Archive>
		void load(Archive && archive, uint32_t version);

	private:
		void OnHit(const collision::HitResult& hit_result) override;
	};
} // cumulonimbus::component
