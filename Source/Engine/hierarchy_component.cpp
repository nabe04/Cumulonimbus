#include "hierarchy_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::HierarchyComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::HierarchyComponent)

namespace cumulonimbus::component
{
	template <class Archive>
	void HierarchyComponent::serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(parent_entity)
		);
	}

	HierarchyComponent::HierarchyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	HierarchyComponent::HierarchyComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const HierarchyComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	//HierarchyComponent::HierarchyComponent(const HierarchyComponent& other)
	//	:ComponentBase{other},
	//	 parent_entity{other.parent_entity}
	//{

	//}

	//HierarchyComponent& HierarchyComponent::operator=(const HierarchyComponent& other)
	//{
	//	if (this == &other)
	//	{
	//		return *this;
	//	}

	//	ComponentBase::operator=(other);
	//	parent_entity = other.parent_entity;

	//	return *this;
	//}

	void HierarchyComponent::CommonUpdate(float dt)
	{
	}

	void HierarchyComponent::Load(ecs::Registry* registry)
	{

	}

} // cumulonimbus::component
