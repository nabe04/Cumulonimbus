#include "hierarchy_component.h"

#include "transform_component.h"

#include <ImGuizmo/ImGuizmo.h>

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

	void HierarchyComponent::CommonUpdate(const float dt)
	{
		using namespace DirectX::SimpleMath;
		auto rotation_m = Matrix::CreateRotationY(DirectX::XMConvertToRadians(90));
		auto inv_m = rotation_m.Invert();


	}

	void HierarchyComponent::PreCommonUpdate(float dt)
	{

	}

	void HierarchyComponent::Load(ecs::Registry* registry)
	{

	}

	void HierarchyComponent::SetParentEntity(
		ecs::Registry* const registry,
		const mapping::rename_type::Entity& parent_ent)
	{
		//-- 兄弟階層の再設定 --//
		if (parent_entity != parent_ent)
		{// セットしようとしている親エンティティと保持している親エンティティが
		 // 違った場合親が変わったので前後の兄弟階層のエンティティの位置の変更する
			if (back_sibling.empty())
			{// 前に兄弟階層が存在しない
				if (!next_sibling.empty())
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
					if (const auto& selected_parent_ent = registry->GetComponent<HierarchyComponent>(GetEntity()).GetParentEntity();
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
		if (parent_entity.empty())
		{// 一番上の親階層処理
			if (first_child.empty())
			{
				next_sibling = {};
				back_sibling = {};
			}
		}
		else
		{// 子階層処理
			if (const auto& parent_first_child = registry->GetComponent<HierarchyComponent>(parent_ent).GetFirstChild();
				parent_first_child.empty())
			{// 現エンティティの親階層に子階層ののエンティティが存在しない時
				registry->GetComponent<HierarchyComponent>(parent_ent).first_child = GetEntity();
			}
			else
			{// 現エンティティの親階層に子階層ののエンティティが存在する時
				if (const auto& parent_first_child_next = registry->GetComponent<HierarchyComponent>(parent_first_child).GetNextSibling();
					parent_first_child_next.empty())
				{// 親階層のfirst_childが次の兄弟階層(next)を持っていない場合
					// 親階層のfirst_child先のエンティティのnextに現在のエンティティ(selected_ent)をセット
					registry->GetComponent<HierarchyComponent>(parent_first_child).next_sibling = GetEntity();
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
							sibling_hierarchy_comp.next_sibling = GetEntity();
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

		//-- 位置の再計算 --//
		if(!parent_entity.empty())
		{// 親を持っていれば

			std::function<DirectX::SimpleMath::Vector3(const mapping::rename_type::Entity& ent)> RecalculationScale = [&](const mapping::rename_type::Entity& ent) -> DirectX::SimpleMath::Vector3
			{
				HierarchyComponent& hierarchy_comp = GetRegistry()->GetComponent<HierarchyComponent>(ent);
				if (hierarchy_comp.GetParentEntity().empty())
					return GetRegistry()->GetComponent<TransformComponent>(hierarchy_comp.GetEntity()).GetScale();

				return GetRegistry()->GetComponent<TransformComponent>(ent).GetScale() / RecalculationScale(hierarchy_comp.GetParentEntity());
			};

			std::function<DirectX::SimpleMath::Vector3(const mapping::rename_type::Entity& ent)> RecalculationRotation = [&](const mapping::rename_type::Entity& ent) -> DirectX::SimpleMath::Vector3
			{
				HierarchyComponent& hierarchy_comp = GetRegistry()->GetComponent<HierarchyComponent>(ent);
				if (hierarchy_comp.GetParentEntity().empty())
					return GetRegistry()->GetComponent<TransformComponent>(hierarchy_comp.GetEntity()).GetWorldRotation();

				return GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorldRotation() - RecalculationRotation(hierarchy_comp.GetParentEntity());
			};

			DirectX::SimpleMath::Matrix m{ registry->GetComponent<TransformComponent>(parent_entity).GetWorld4x4().Invert() * GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorld4x4()};
			DirectX::SimpleMath::Vector3 t;
			DirectX::SimpleMath::Vector3 s;
			DirectX::SimpleMath::Vector3 r;

			DirectX::XMVECTOR t_v, s_v, r_v;
			DirectX::XMMatrixDecompose(&s_v, &r_v, &t_v, m);
			ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&m), reinterpret_cast<float*>(&t), reinterpret_cast<float*>(&r), reinterpret_cast<float*>(&s));
			//DirectX::SimpleMath::Quaternion::R
			//DirectX::SimpleMath::Matrix::CreateFromQuaternion()

			TransformComponent& parent_transform_comp = registry->GetComponent<TransformComponent>(parent_entity);
			TransformComponent& my_transform_comp	  = registry->GetComponent<TransformComponent>(GetEntity());

			//const DirectX::SimpleMath::Matrix&  inv_parent_world_mat = parent_transform_comp.GetWorld4x4().Invert();
			//const DirectX::SimpleMath::Vector3& parent_scale		 = parent_transform_comp.GetScale();
			//const DirectX::SimpleMath::Vector3& my_scale			 = my_transform_comp.GetScale();
			//const DirectX::SimpleMath::Vector3& parent_angle		 = parent_transform_comp.GetWorldRotation();
			//const DirectX::SimpleMath::Vector3& my_angle			 = my_transform_comp.GetWorldRotation();

			//const DirectX::SimpleMath::Matrix& result_mat		= inv_parent_world_mat * my_transform_comp.GetWorld4x4();
			//const DirectX::SimpleMath::Vector3& parents_scale	= RecalculationScale(GetEntity());
			//const DirectX::SimpleMath::Vector3& parent_rotation = RecalculationRotation(GetEntity());
			//const DirectX::SimpleMath::Vector3& parents_angle;

			my_transform_comp.SetPosition(t);
			my_transform_comp.SetScale(s);
			//my_transform_comp.SetScale(DirectX::SimpleMath::Vector3{ result_mat._11 ,
			//														 result_mat._22 ,
			//														 result_mat._33});
			my_transform_comp.SetWorldRotation(r);

			//registry->GetComponent<TransformComponent>(GetEntity()).SetWorld4x4(parent_world_mat.Invert()* my_world_mat);
		}
	}

	void HierarchyComponent::ActivateDirtyFlg()
	{
		is_dirty = true;
		// 次の兄弟階層がなければ処理を抜ける
		if (!next_sibling.empty())
			GetRegistry()->GetComponent<HierarchyComponent>(next_sibling).ActivateDirtyFlg();
		if (!first_child.empty())
			GetRegistry()->GetComponent<HierarchyComponent>(first_child).ActivateDirtyFlg();

		return;
	}

	void HierarchyComponent::DeactivateDirtyFlg()
	{
		is_dirty = false;
	}

	DirectX::SimpleMath::Vector3 HierarchyComponent::GetParentsScale(const mapping::rename_type::Entity& ent) const
	{
		HierarchyComponent& hierarchy_comp = GetRegistry()->GetComponent<HierarchyComponent>(ent);
		if (hierarchy_comp.GetParentEntity().empty())
			return GetRegistry()->GetComponent<TransformComponent>(hierarchy_comp.GetEntity()).GetScale();

		auto scale = GetParentsScale(hierarchy_comp.GetParentEntity());
		return GetRegistry()->GetComponent<TransformComponent>(ent).GetScale() / scale;
	}

} // cumulonimbus::component
