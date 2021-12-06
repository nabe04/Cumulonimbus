#pragma once
#include "weapon_component.h"
#include "event.h"

namespace cumulonimbus
{
	namespace collision
	{
		struct HitResult;
	} // collision
} // cumulonimbus

namespace cumulonimbus::component
{
	class PlayerSwordComponent final : public WeaponComponent
	{
	public:
		explicit PlayerSwordComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PlayerSwordComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const PlayerSwordComponent& copy_comp); // for prefab
		explicit PlayerSwordComponent(mapping::component_tag::ComponentTag tag);
		explicit PlayerSwordComponent()  = default; // for cereal
		~PlayerSwordComponent() override = default;

		void Start() override;
		void GameUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void RegistryApplyDamage(const mapping::rename_type::Entity& ent, const std::function<void(u_int)>& func);
	private:
		system::Event<void, u_int> damage_event{};

		void OnHit(const collision::HitResult& hit_result) override;
	};
} // cumulonimbus::component