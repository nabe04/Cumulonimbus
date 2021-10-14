#include "hierarchy_component.h"

#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::HierarchyComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::HierarchyComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::HierarchyComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void HierarchyComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(parent_entity)
		);
	}

	template <class Archive>
	void HierarchyComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(parent_entity)
		);
	}

	HierarchyComponent::HierarchyComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	HierarchyComponent::HierarchyComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent, const HierarchyComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void HierarchyComponent::CommonUpdate(float dt)
	{
		const DirectX::SimpleMath::Matrix parent_world_mat = GetRegistry()->GetComponent<component::TransformComponent>(parent_entity).GetWorld4x4();
		const DirectX::SimpleMath::Matrix child_world_mat = GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).GetWorld4x4();

		GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).SetWorld4x4(parent_world_mat * child_world_mat);
		auto world_mat = GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).GetWorld4x4();
		int a;
		a = 0;
	}

	void HierarchyComponent::PreCommonUpdate(float dt)
	{

	}

	void HierarchyComponent::Load(ecs::Registry* registry)
	{

	}

	void HierarchyComponent::RegistryFamily(
		ecs::Registry* registry,
		const mapping::rename_type::Entity& parent_ent,
		const mapping::rename_type::Entity& selected_ent)
	{
		parent_entity = parent_ent;
		if(parent_entity.empty())
		{// 一番上の親階層処理
			if(first_child.empty())
			{
				first_child  = selected_ent;
				next_sibling = {};
				back_sibling = {};
			}
		}
		else
		{// 子階層処理
			const auto& parent_first_child		= registry->GetComponent<HierarchyComponent>(parent_ent).GetFirstChild();
			const auto& parent_first_child_next = registry->GetComponent<HierarchyComponent>(parent_first_child).GetNextSibling();
			if(parent_first_child_next.empty())
			{// 親階層のfirst_childが次の兄弟階層(next)を持っていない場合
				// 親階層のfirst_child先のエンティティのnextに現在のエンティティ(selected_ent)をセット
				registry->GetComponent<HierarchyComponent>(parent_first_child).SetNext(selected_ent);
				// 現在のエンティティ(selected_ent)のbackに親階層のfirst_childエンティティをセット
				back_sibling = parent_first_child;
			}
			else
			{// 親階層のfirst_childが次の兄弟階層(next)を持っている場合
				// 兄弟階層用エンティティ(親階層のfirst_childが持つnextの指すエンティティから始める)
				mapping::rename_type::Entity sibling_hierarchy_ent = parent_first_child_next;
				while(true)
				{
					auto& sibling_hierarchy_comp = registry->GetComponent<HierarchyComponent>(sibling_hierarchy_ent);
					if(sibling_hierarchy_comp.GetNextSibling().empty())
					{// 兄弟階層の次の階層(next)が登録されていない -> 現段階の兄弟階層の中で一番末端の階層
						// 兄弟階層の次の兄弟階層(next)に現在のエンティティをセット
						sibling_hierarchy_comp.SetNext(selected_ent);
						// 現在のエンティティの前の兄弟階層(back)に一つ前の兄弟階層(sibling_hierarchy_comp)をセット
						// -> この階層(エンティティ)が末端の兄弟階層になる
						back_sibling = sibling_hierarchy_ent;
						break;
					}
					sibling_hierarchy_ent = sibling_hierarchy_comp.GetNextSibling();
				}
			}
		}
	}

} // cumulonimbus::component
