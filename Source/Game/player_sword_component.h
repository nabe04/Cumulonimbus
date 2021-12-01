#pragma once
#include "component_base.h"
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
	class PlayerSwordComponent final : public ComponentBase
	{
	public:
		void Start() override;
		void GameUpdate(float dt) override;

		void RenderImGui() override;

		void RegistryApplyDamage(const mapping::rename_type::Entity& ent, const std::function<void(u_int)>& func);
	private:
		system::Event<void, u_int> damage_event{};

		void OnAttacked(const collision::HitResult& hit_result);
	};
} // cumulonimbus::component