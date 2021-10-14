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
		{// ��ԏ�̐e�K�w����
			if(first_child.empty())
			{
				first_child  = selected_ent;
				next_sibling = {};
				back_sibling = {};
			}
		}
		else
		{// �q�K�w����
			const auto& parent_first_child		= registry->GetComponent<HierarchyComponent>(parent_ent).GetFirstChild();
			const auto& parent_first_child_next = registry->GetComponent<HierarchyComponent>(parent_first_child).GetNextSibling();
			if(parent_first_child_next.empty())
			{// �e�K�w��first_child�����̌Z��K�w(next)�������Ă��Ȃ��ꍇ
				// �e�K�w��first_child��̃G���e�B�e�B��next�Ɍ��݂̃G���e�B�e�B(selected_ent)���Z�b�g
				registry->GetComponent<HierarchyComponent>(parent_first_child).SetNext(selected_ent);
				// ���݂̃G���e�B�e�B(selected_ent)��back�ɐe�K�w��first_child�G���e�B�e�B���Z�b�g
				back_sibling = parent_first_child;
			}
			else
			{// �e�K�w��first_child�����̌Z��K�w(next)�������Ă���ꍇ
				// �Z��K�w�p�G���e�B�e�B(�e�K�w��first_child������next�̎w���G���e�B�e�B����n�߂�)
				mapping::rename_type::Entity sibling_hierarchy_ent = parent_first_child_next;
				while(true)
				{
					auto& sibling_hierarchy_comp = registry->GetComponent<HierarchyComponent>(sibling_hierarchy_ent);
					if(sibling_hierarchy_comp.GetNextSibling().empty())
					{// �Z��K�w�̎��̊K�w(next)���o�^����Ă��Ȃ� -> ���i�K�̌Z��K�w�̒��ň�Ԗ��[�̊K�w
						// �Z��K�w�̎��̌Z��K�w(next)�Ɍ��݂̃G���e�B�e�B���Z�b�g
						sibling_hierarchy_comp.SetNext(selected_ent);
						// ���݂̃G���e�B�e�B�̑O�̌Z��K�w(back)�Ɉ�O�̌Z��K�w(sibling_hierarchy_comp)���Z�b�g
						// -> ���̊K�w(�G���e�B�e�B)�����[�̌Z��K�w�ɂȂ�
						back_sibling = sibling_hierarchy_ent;
						break;
					}
					sibling_hierarchy_ent = sibling_hierarchy_comp.GetNextSibling();
				}
			}
		}
	}

} // cumulonimbus::component
