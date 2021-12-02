#pragma once
#include "actor3d_component.h"
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
	class WeaponComponent : public Actor3DComponent
	{
	public:
		void Load(ecs::Registry* registry) override;

		// ‘¼‚ÌƒRƒŠƒWƒ‡ƒ“‚Æ‚ ‚½‚Á‚½‚Æ‚«
		virtual void OnHit(const collision::HitResult& hit_result);

		//void RegistryHitEvent(const mapping::rename_type::Entity& ent,const std::function<>)

	protected:
		system::Event<void, u_int> hit_event{};
	};
} // cumulonimbus::component
