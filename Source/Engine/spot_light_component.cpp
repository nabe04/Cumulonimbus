#include "spot_light_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::SpotLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::LightComponent, cumulonimbus::component::SpotLightComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::SpotLightComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void SpotLightComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<LightComponent>(this),

			CEREAL_NVP(spot_light)
		);
	}

	template <class Archive>
	void SpotLightComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<LightComponent>(this),

			CEREAL_NVP(spot_light)
		);
	}

	SpotLightComponent::SpotLightComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent)
		:LightComponent{ registry,ent }
	{
	}

	SpotLightComponent::SpotLightComponent(
		ecs::Registry* registry,
		mapping::rename_type::Entity ent,
		const SpotLightComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void SpotLightComponent::CommonUpdate(float dt)
	{
	}

	void SpotLightComponent::PostCommonUpdate(float dt)
	{
	}

	void SpotLightComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<SpotLightComponent>(GetEntity(), "Spot Light"))
		{
			ImChangeParameter();
		}
	}

	void SpotLightComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}
} // cumulonimbus::component
