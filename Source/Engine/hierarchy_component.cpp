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
		// Todo : Hierarchy 処理が完了すればこのブロック全てのコメントアウトを外す
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
		//-- 兄弟階層の再設定 --//
		if(parent_entity != parent_ent)
		{// セットしようとしている親エンティティと保持している親エンティティが
		 // 違った場合親が変わったので前後の兄弟階層のエンティティの位置の変更する
			if(back_sibling.empty())
			{// 前に兄弟階層が存在しない
				if(!next_sibling.empty())
				{// 後に兄弟階層が存在する
					auto& next_sibling_comp = registry->GetComponent<HierarchyComponent>(next_sibling);
					// 前の階層のクリア -> 自分が「前の階層」にあたるので必要無くなる
					next_sibling_comp.back_sibling.clear();
					auto& next_sibling_parent_comp = registry->GetComponent<HierarchyComponent>(next_sibling_comp.GetParentEntity());
					// この階層の親階層のfirst_childが自分の後のエンティティになる -> next_sibling_compのエンティティになる
					next_sibling_parent_comp.first_child = next_sibling_comp.GetEntity();
				}
				else
				{// 後に兄弟階層が存在しない -> 単一の子階層
					if (const auto& selected_parent_ent = registry->GetComponent<HierarchyComponent>(selected_ent).GetParentEntity();
						!selected_parent_ent.empty())
					{// 自身の親パスが空で無いなら -> その親パスのfirst_childを空にする -> 前にも後にも親の持つ子供がいないから
						auto& selected_parent_comp = registry->GetComponent<HierarchyComponent>(selected_parent_ent);
						selected_parent_comp.first_child = {};
					}
				}
			}
			else
			{// 前に兄弟階層が存在する
				if (next_sibling.empty())
				{// 後に兄弟階層が存在しない -> 兄弟階層の中で一番下の階層
					auto& back_sibling_comp = registry->GetComponent<HierarchyComponent>(back_sibling);
					// 前の兄弟会騒音next_siblingをクリア -> 親の階層が変わるので自分の情報(エンティティ)をクリア
					back_sibling_comp.next_sibling.clear();
				}
				else
				{// 後に兄弟階層が存在する -> backとnextに兄弟階層がいるので自分を抜いてbackとnextを繋げる
					auto& back_sibling_comp = registry->GetComponent<HierarchyComponent>(back_sibling);
					auto& next_sibling_comp = registry->GetComponent<HierarchyComponent>(next_sibling);

					back_sibling_comp.next_sibling = next_sibling_comp.GetEntity();
					next_sibling_comp.back_sibling = back_sibling_comp.GetEntity();
				}
			}

			// 自分のパスが元々いた兄弟階層と異なるため
			// back_siblingとnext_siblingを空にする
			back_sibling = {};
			next_sibling = {};
		}
		else
		{// 同じ兄弟階層に再度付けられたため設定し直す必要がない -> 早期リターン
			return;
		}

		//-- 新規設定 --//
		parent_entity = parent_ent;
		if(parent_entity.empty())
		{// 一番上の親階層処理
			if(first_child.empty())
			{
				next_sibling = {};
				back_sibling = {};
			}
		}
		else
		{// 子階層処理
			if(const auto& parent_first_child = registry->GetComponent<HierarchyComponent>(parent_ent).GetFirstChild();
				parent_first_child.empty())
			{// 現エンティティの親階層に子階層ののエンティティが存在しない時
				registry->GetComponent<HierarchyComponent>(parent_ent).first_child = selected_ent;
			}
			else
			{// 現エンティティの親階層に子階層ののエンティティが存在する時
				if (const auto& parent_first_child_next = registry->GetComponent<HierarchyComponent>(parent_first_child).GetNextSibling();
					parent_first_child_next.empty())
				{// 親階層のfirst_childが次の兄弟階層(next)を持っていない場合
					// 親階層のfirst_child先のエンティティのnextに現在のエンティティ(selected_ent)をセット
					registry->GetComponent<HierarchyComponent>(parent_first_child).next_sibling = selected_ent;
					// 現在のエンティティ(selected_ent)のbackに親階層のfirst_childエンティティをセット
					back_sibling = parent_first_child;
				}
				else
				{// 親階層のfirst_childが次の兄弟階層(next)を持っている場合
					// 兄弟階層用エンティティ(親階層のfirst_childが持つnextの指すエンティティから始める)
					mapping::rename_type::Entity sibling_hierarchy_ent = parent_first_child_next;
					while (true)
					{
						auto& sibling_hierarchy_comp = registry->GetComponent<HierarchyComponent>(sibling_hierarchy_ent);
						if (sibling_hierarchy_comp.GetNextSibling().empty())
						{// 兄弟階層の次の階層(next)が登録されていない -> 現段階の兄弟階層の中で一番末端の階層
							// 兄弟階層の次の兄弟階層(next)に現在のエンティティをセット
							sibling_hierarchy_comp.next_sibling = selected_ent;
							// 現在のエンティティの前の兄弟階層(back)に一つ前の兄弟階層(sibling_hierarchy_comp)をセット
							// -> この階層(エンティティ)が末端の兄弟階層になる
							back_sibling = sibling_hierarchy_ent;
							break;
						}
						// 兄弟階層で最後の階層ではなかったため次の兄弟階層に進めてループする
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
