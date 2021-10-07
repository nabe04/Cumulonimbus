#include "physic_material_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::PhysicMaterialComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::PhysicMaterialComponent)

namespace cumulonimbus::component
{
	template <class Archive>
	void PhysicMaterialComponent::serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(dynamic_friction),
			CEREAL_NVP(static_friction),
			CEREAL_NVP(bounciness),
			CEREAL_NVP(friction_combine),
			CEREAL_NVP(bounce_combine)
		);
	}

	PhysicMaterialComponent::PhysicMaterialComponent(
		ecs::Registry* registry,
		mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	PhysicMaterialComponent::PhysicMaterialComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const PhysicMaterialComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}


	void PhysicMaterialComponent::GameUpdate(float dt)
	{
	}

	void PhysicMaterialComponent::RenderImGui()
	{
	}

	void PhysicMaterialComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}
} // cumulonimbus::component
