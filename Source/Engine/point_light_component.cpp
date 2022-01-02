#include "point_light_component.h"

#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::PointLightComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::LightComponent, cumulonimbus::component::PointLightComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::PointLightComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void PointLightComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<LightComponent>(this),

			CEREAL_NVP(point_light)
		);
	}

	template <class Archive>
	void PointLightComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<LightComponent>(this),

			CEREAL_NVP(point_light)
		);
	}

	PointLightComponent::PointLightComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent)
		:LightComponent{ registry,ent }
	{

	}

	PointLightComponent::PointLightComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const PointLightComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void PointLightComponent::Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent)
	{
	}

	void PointLightComponent::CommonUpdate(const float dt)
	{
		LightComponent::CommonUpdate(dt);
	}

	void PointLightComponent::PostCommonUpdate(float dt)
	{
		point_light.p_light_position  = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();
		point_light.p_light_color	  = DirectX::SimpleMath::Vector3{ light_color.x,light_color.y,light_color.z };
		point_light.p_light_intensity = light_intensity;
	}

	void PointLightComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<PointLightComponent>(GetEntity(), "Point Light"))
		{
			ImChangeParameter();
			ImGui::DragFloat("Range", &point_light.p_light_range, 0.1f, 0.0f, FLT_MAX);
		}
	}

	void PointLightComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}
} // cumulonimbus::component
