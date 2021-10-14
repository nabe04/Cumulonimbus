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
		// Todo : Hierarchy ��������������΂��̃u���b�N�S�ẴR�����g�A�E�g���O��
		//const DirectX::SimpleMath::Matrix parent_world_mat = GetRegistry()->GetComponent<component::TransformComponent>(parent_entity).GetWorld4x4();
		//const DirectX::SimpleMath::Matrix child_world_mat = GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).GetWorld4x4();

		//GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).SetWorld4x4(parent_world_mat * child_world_mat);
		//auto world_mat = GetRegistry()->GetComponent<component::TransformComponent>(GetEntity()).GetWorld4x4();
		//int a;
		//a = 0;
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
		//-- �Z��K�w�̍Đݒ� --//
		if(parent_entity != parent_ent)
		{// �Z�b�g���悤�Ƃ��Ă���e�G���e�B�e�B�ƕێ����Ă���e�G���e�B�e�B��
		 // ������ꍇ�e���ς�����̂őO��̌Z��K�w�̃G���e�B�e�B�̈ʒu�̕ύX����
			if(back_sibling.empty())
			{// �O�ɌZ��K�w�����݂��Ȃ�
				if(!next_sibling.empty())
				{// ��ɌZ��K�w�����݂���
					auto& next_sibling_comp = registry->GetComponent<HierarchyComponent>(next_sibling);
					// �O�̊K�w�̃N���A -> �������u�O�̊K�w�v�ɂ�����̂ŕK�v�����Ȃ�
					next_sibling_comp.back_sibling.clear();
					auto& next_sibling_parent_comp = registry->GetComponent<HierarchyComponent>(next_sibling_comp.GetParentEntity());
					// ���̊K�w�̐e�K�w��first_child�������̌�̃G���e�B�e�B�ɂȂ� -> next_sibling_comp�̃G���e�B�e�B�ɂȂ�
					next_sibling_parent_comp.first_child = next_sibling_comp.GetEntity();
				}
				else
				{// ��ɌZ��K�w�����݂��Ȃ� -> �P��̎q�K�w
					if (const auto& selected_parent_ent = registry->GetComponent<HierarchyComponent>(selected_ent).GetParentEntity();
						!selected_parent_ent.empty())
					{// ���g�̐e�p�X����Ŗ����Ȃ� -> ���̐e�p�X��first_child����ɂ��� -> �O�ɂ���ɂ��e�̎��q�������Ȃ�����
						auto& selected_parent_comp = registry->GetComponent<HierarchyComponent>(selected_parent_ent);
						selected_parent_comp.first_child = {};
					}
				}
			}
			else
			{// �O�ɌZ��K�w�����݂���
				if (next_sibling.empty())
				{// ��ɌZ��K�w�����݂��Ȃ� -> �Z��K�w�̒��ň�ԉ��̊K�w
					auto& back_sibling_comp = registry->GetComponent<HierarchyComponent>(back_sibling);
					// �O�̌Z����next_sibling���N���A -> �e�̊K�w���ς��̂Ŏ����̏��(�G���e�B�e�B)���N���A
					back_sibling_comp.next_sibling.clear();
				}
				else
				{// ��ɌZ��K�w�����݂��� -> back��next�ɌZ��K�w������̂Ŏ����𔲂���back��next���q����
					auto& back_sibling_comp = registry->GetComponent<HierarchyComponent>(back_sibling);
					auto& next_sibling_comp = registry->GetComponent<HierarchyComponent>(next_sibling);

					back_sibling_comp.next_sibling = next_sibling_comp.GetEntity();
					next_sibling_comp.back_sibling = back_sibling_comp.GetEntity();
				}
			}

			// �����̃p�X�����X�����Z��K�w�ƈقȂ邽��
			// back_sibling��next_sibling����ɂ���
			back_sibling = {};
			next_sibling = {};
		}
		else
		{// �����Z��K�w�ɍēx�t����ꂽ���ߐݒ肵�����K�v���Ȃ� -> �������^�[��
			return;
		}

		//-- �V�K�ݒ� --//
		parent_entity = parent_ent;
		if(parent_entity.empty())
		{// ��ԏ�̐e�K�w����
			if(first_child.empty())
			{
				next_sibling = {};
				back_sibling = {};
			}
		}
		else
		{// �q�K�w����
			if(const auto& parent_first_child = registry->GetComponent<HierarchyComponent>(parent_ent).GetFirstChild();
				parent_first_child.empty())
			{// ���G���e�B�e�B�̐e�K�w�Ɏq�K�w�̂̃G���e�B�e�B�����݂��Ȃ���
				registry->GetComponent<HierarchyComponent>(parent_ent).first_child = selected_ent;
			}
			else
			{// ���G���e�B�e�B�̐e�K�w�Ɏq�K�w�̂̃G���e�B�e�B�����݂��鎞
				if (const auto& parent_first_child_next = registry->GetComponent<HierarchyComponent>(parent_first_child).GetNextSibling();
					parent_first_child_next.empty())
				{// �e�K�w��first_child�����̌Z��K�w(next)�������Ă��Ȃ��ꍇ
					// �e�K�w��first_child��̃G���e�B�e�B��next�Ɍ��݂̃G���e�B�e�B(selected_ent)���Z�b�g
					registry->GetComponent<HierarchyComponent>(parent_first_child).next_sibling = selected_ent;
					// ���݂̃G���e�B�e�B(selected_ent)��back�ɐe�K�w��first_child�G���e�B�e�B���Z�b�g
					back_sibling = parent_first_child;
				}
				else
				{// �e�K�w��first_child�����̌Z��K�w(next)�������Ă���ꍇ
					// �Z��K�w�p�G���e�B�e�B(�e�K�w��first_child������next�̎w���G���e�B�e�B����n�߂�)
					mapping::rename_type::Entity sibling_hierarchy_ent = parent_first_child_next;
					while (true)
					{
						auto& sibling_hierarchy_comp = registry->GetComponent<HierarchyComponent>(sibling_hierarchy_ent);
						if (sibling_hierarchy_comp.GetNextSibling().empty())
						{// �Z��K�w�̎��̊K�w(next)���o�^����Ă��Ȃ� -> ���i�K�̌Z��K�w�̒��ň�Ԗ��[�̊K�w
							// �Z��K�w�̎��̌Z��K�w(next)�Ɍ��݂̃G���e�B�e�B���Z�b�g
							sibling_hierarchy_comp.next_sibling = selected_ent;
							// ���݂̃G���e�B�e�B�̑O�̌Z��K�w(back)�Ɉ�O�̌Z��K�w(sibling_hierarchy_comp)���Z�b�g
							// -> ���̊K�w(�G���e�B�e�B)�����[�̌Z��K�w�ɂȂ�
							back_sibling = sibling_hierarchy_ent;
							break;
						}
						// �Z��K�w�ōŌ�̊K�w�ł͂Ȃ��������ߎ��̌Z��K�w�ɐi�߂ă��[�v����
						sibling_hierarchy_ent = sibling_hierarchy_comp.GetNextSibling();
					}
				}
			}
		}
	}

	void HierarchyComponent::SetParentEntity(
		ecs::Registry* const registry,
		const mapping::rename_type::Entity& parent)
	{
		RegistryFamily(registry, parent, GetEntity());
	}

} // cumulonimbus::component
