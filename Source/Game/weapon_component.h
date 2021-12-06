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
		explicit WeaponComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit WeaponComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const WeaponComponent& copy_comp); // for prefab
		explicit WeaponComponent(mapping::component_tag::ComponentTag tag);
		explicit WeaponComponent()  = default; // for cereal
		~WeaponComponent() override = default;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

	protected:
		// ‘¼‚ÌƒRƒŠƒWƒ‡ƒ“‚Æ‚ ‚½‚Á‚½‚Æ‚«
		virtual void OnHit(const collision::HitResult& hit_result) {}
	};
} // cumulonimbus::component
