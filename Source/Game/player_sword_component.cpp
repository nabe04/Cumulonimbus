#include "player_sword_component.h"

namespace cumulonimbus::component
{
	void PlayerSwordComponent::Start()
	{

	}

	void PlayerSwordComponent::RenderImGui()
	{
		if(GetRegistry()->CollapsingHeader<PlayerSwordComponent>(GetEntity(),"Player Sword"))
		{

		}
	}

	void PlayerSwordComponent::RegistryApplyDamage(const mapping::rename_type::Entity& ent, const std::function<void(u_int)>& func)
	{
		damage_event.RegistryEvent(ent, func);
	}

	void PlayerSwordComponent::OnAttacked(const collision::HitResult& hit_result)
	{

	}

} // cumulonimbus::component