#include "light_component.h"

#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::LightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::LightComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::LightComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void LightComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),

			CEREAL_NVP(light_direction),
			CEREAL_NVP(light_intensity),
			CEREAL_NVP(light_color)
		);
	}

	template <class Archive>
	void LightComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),

			CEREAL_NVP(light_direction),
			CEREAL_NVP(light_intensity),
			CEREAL_NVP(light_color)
		);
	}

	LightComponent::LightComponent(
		ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	LightComponent::LightComponent(
		ecs::Registry* registry, const mapping::rename_type::Entity ent,
		const LightComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void LightComponent::CommonUpdate(float dt)
	{
		// ƒ‰ƒCƒg‚ÌŒü‚«Ý’è
		light_direction = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetModelFront();
	}

	void LightComponent::RenderImGui()
	{

	}

	void LightComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void LightComponent::ImChangeParameter()
	{
		ImGui::ColorEdit4("Color", &light_color.x);
		ImGui::DragFloat("Intensity", &light_intensity, 0.1f, 0.0f, 150.0f);
	}

} // cumulonimbus::component
