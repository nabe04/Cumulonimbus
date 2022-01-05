#include "spot_light_component.h"

#include "transform_component.h"

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
		LightComponent::CommonUpdate(dt);
	}

	void SpotLightComponent::PostCommonUpdate(float dt)
	{
		spot_light.s_light_position  = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();
		spot_light.s_light_direction = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetModelFront();
		spot_light.s_light_color	 = DirectX::SimpleMath::Vector4{ light_color.x,light_color.y,light_color.z ,1.0f };
		spot_light.s_light_intensity = light_intensity;
	}

	void SpotLightComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<SpotLightComponent>(GetEntity(), "Spot Light"))
		{
			ImChangeParameter();
			ImGui::DragFloat("Range", &spot_light.s_light_range, 0.1f, 0.0f, FLT_MAX);
			ImGui::DragFloat("Inner Cone", &spot_light.s_light_inner_cone, 0.01f, spot_light.s_light_outer_cone, 1.0f);
			ImGui::DragFloat("Outer Cone", &spot_light.s_light_outer_cone, 0.01f, 0.0f, spot_light.s_light_inner_cone);
		}
	}

	void SpotLightComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}
} // cumulonimbus::component
