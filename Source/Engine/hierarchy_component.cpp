#include "hierarchy_component.h"

#include "arithmetic.h"
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
		//-- ŒZ’íŠK‘w‚ÌÄÝ’è --//
		if (parent_entity != parent_ent)
		{// ƒZƒbƒg‚µ‚æ‚¤‚Æ‚µ‚Ä‚¢‚éeƒGƒ“ƒeƒBƒeƒB‚Æ•ÛŽ‚µ‚Ä‚¢‚éeƒGƒ“ƒeƒBƒeƒB‚ª
		 // ˆá‚Á‚½ê‡e‚ª•Ï‚í‚Á‚½‚Ì‚Å‘OŒã‚ÌŒZ’íŠK‘w‚ÌƒGƒ“ƒeƒBƒeƒB‚ÌˆÊ’u‚Ì•ÏX‚·‚é
			if (back_sibling.empty())
			{// ‘O‚ÉŒZ’íŠK‘w‚ª‘¶Ý‚µ‚È‚¢
				if (!next_sibling.empty())
				{// Œã‚ÉŒZ’íŠK‘w‚ª‘¶Ý‚·‚é
					auto& next_sibling_comp = registry->GetComponent<HierarchyComponent>(next_sibling);
					// ‘O‚ÌŠK‘w‚ÌƒNƒŠƒA -> Ž©•ª‚ªu‘O‚ÌŠK‘wv‚É‚ ‚½‚é‚Ì‚Å•K—v–³‚­‚È‚é
					next_sibling_comp.back_sibling.clear();
					auto& next_sibling_parent_comp = registry->GetComponent<HierarchyComponent>(next_sibling_comp.GetParentEntity());
					// ‚±‚ÌŠK‘w‚ÌeŠK‘w‚Ìfirst_child‚ªŽ©•ª‚ÌŒã‚ÌƒGƒ“ƒeƒBƒeƒB‚É‚È‚é -> next_sibling_comp‚ÌƒGƒ“ƒeƒBƒeƒB‚É‚È‚é
					next_sibling_parent_comp.first_child = next_sibling_comp.GetEntity();
				}
				else
				{// Œã‚ÉŒZ’íŠK‘w‚ª‘¶Ý‚µ‚È‚¢ -> ’Pˆê‚ÌŽqŠK‘w
					if (const auto& selected_parent_ent = registry->GetComponent<HierarchyComponent>(GetEntity()).GetParentEntity();
						!selected_parent_ent.empty())
					{// Ž©g‚ÌeƒpƒX‚ª‹ó‚Å–³‚¢‚È‚ç -> ‚»‚ÌeƒpƒX‚Ìfirst_child‚ð‹ó‚É‚·‚é -> ‘O‚É‚àŒã‚É‚àe‚ÌŽ‚ÂŽq‹Ÿ‚ª‚¢‚È‚¢‚©‚ç
						auto& selected_parent_comp = registry->GetComponent<HierarchyComponent>(selected_parent_ent);
						selected_parent_comp.first_child = {};
					}
				}
			}
			else
			{// ‘O‚ÉŒZ’íŠK‘w‚ª‘¶Ý‚·‚é
				if (next_sibling.empty())
				{// Œã‚ÉŒZ’íŠK‘w‚ª‘¶Ý‚µ‚È‚¢ -> ŒZ’íŠK‘w‚Ì’†‚Åˆê”Ô‰º‚ÌŠK‘w
					auto& back_sibling_comp = registry->GetComponent<HierarchyComponent>(back_sibling);
					// ‘O‚ÌŒZ’í‰ï‘›‰¹next_sibling‚ðƒNƒŠƒA -> e‚ÌŠK‘w‚ª•Ï‚í‚é‚Ì‚ÅŽ©•ª‚Ìî•ñ(ƒGƒ“ƒeƒBƒeƒB)‚ðƒNƒŠƒA
					back_sibling_comp.next_sibling.clear();
				}
				else
				{// Œã‚ÉŒZ’íŠK‘w‚ª‘¶Ý‚·‚é -> back‚Ænext‚ÉŒZ’íŠK‘w‚ª‚¢‚é‚Ì‚ÅŽ©•ª‚ð”²‚¢‚Äback‚Ænext‚ðŒq‚°‚é
					auto& back_sibling_comp = registry->GetComponent<HierarchyComponent>(back_sibling);
					auto& next_sibling_comp = registry->GetComponent<HierarchyComponent>(next_sibling);

					back_sibling_comp.next_sibling = next_sibling_comp.GetEntity();
					next_sibling_comp.back_sibling = back_sibling_comp.GetEntity();
				}
			}

			// Ž©•ª‚ÌƒpƒX‚ªŒ³X‚¢‚½ŒZ’íŠK‘w‚ÆˆÙ‚È‚é‚½‚ß
			// back_sibling‚Ænext_sibling‚ð‹ó‚É‚·‚é
			back_sibling = {};
			next_sibling = {};
		}
		else
		{// “¯‚¶ŒZ’íŠK‘w‚ÉÄ“x•t‚¯‚ç‚ê‚½‚½‚ßÝ’è‚µ’¼‚·•K—v‚ª‚È‚¢ -> ‘ŠúƒŠƒ^[ƒ“
			return;
		}

		//-- V‹KÝ’è --//
		if (parent_ent.empty())
		{// ˆê”Ôã‚ÌeŠK‘wˆ—
			if (first_child.empty())
			{
				next_sibling = {};
				back_sibling = {};
			}
		}
		else
		{// ŽqŠK‘wˆ—
			if (const auto& parent_first_child = registry->GetComponent<HierarchyComponent>(parent_ent).GetFirstChild();
				parent_first_child.empty())
			{// Œ»ƒGƒ“ƒeƒBƒeƒB‚ÌeŠK‘w‚ÉŽqŠK‘w‚Ì‚ÌƒGƒ“ƒeƒBƒeƒB‚ª‘¶Ý‚µ‚È‚¢Žž
				registry->GetComponent<HierarchyComponent>(parent_ent).first_child = GetEntity();
			}
			else
			{// Œ»ƒGƒ“ƒeƒBƒeƒB‚ÌeŠK‘w‚ÉŽqŠK‘w‚Ì‚ÌƒGƒ“ƒeƒBƒeƒB‚ª‘¶Ý‚·‚éŽž
				if (const auto& parent_first_child_next = registry->GetComponent<HierarchyComponent>(parent_first_child).GetNextSibling();
					parent_first_child_next.empty())
				{// eŠK‘w‚Ìfirst_child‚ªŽŸ‚ÌŒZ’íŠK‘w(next)‚ðŽ‚Á‚Ä‚¢‚È‚¢ê‡
					// eŠK‘w‚Ìfirst_childæ‚ÌƒGƒ“ƒeƒBƒeƒB‚Ìnext‚ÉŒ»Ý‚ÌƒGƒ“ƒeƒBƒeƒB(selected_ent)‚ðƒZƒbƒg
					registry->GetComponent<HierarchyComponent>(parent_first_child).next_sibling = GetEntity();
					// Œ»Ý‚ÌƒGƒ“ƒeƒBƒeƒB(selected_ent)‚Ìback‚ÉeŠK‘w‚Ìfirst_childƒGƒ“ƒeƒBƒeƒB‚ðƒZƒbƒg
					back_sibling = parent_first_child;
				}
				else
				{// eŠK‘w‚Ìfirst_child‚ªŽŸ‚ÌŒZ’íŠK‘w(next)‚ðŽ‚Á‚Ä‚¢‚éê‡
					// ŒZ’íŠK‘w—pƒGƒ“ƒeƒBƒeƒB(eŠK‘w‚Ìfirst_child‚ªŽ‚Ânext‚ÌŽw‚·ƒGƒ“ƒeƒBƒeƒB‚©‚çŽn‚ß‚é)
					mapping::rename_type::Entity sibling_hierarchy_ent = parent_first_child_next;
					while (true)
					{
						auto& sibling_hierarchy_comp = registry->GetComponent<HierarchyComponent>(sibling_hierarchy_ent);
						if (sibling_hierarchy_comp.GetNextSibling().empty())
						{// ŒZ’íŠK‘w‚ÌŽŸ‚ÌŠK‘w(next)‚ª“o˜^‚³‚ê‚Ä‚¢‚È‚¢ -> Œ»’iŠK‚ÌŒZ’íŠK‘w‚Ì’†‚Åˆê”Ô––’[‚ÌŠK‘w
							// ŒZ’íŠK‘w‚ÌŽŸ‚ÌŒZ’íŠK‘w(next)‚ÉŒ»Ý‚ÌƒGƒ“ƒeƒBƒeƒB‚ðƒZƒbƒg
							sibling_hierarchy_comp.next_sibling = GetEntity();
							// Œ»Ý‚ÌƒGƒ“ƒeƒBƒeƒB‚Ì‘O‚ÌŒZ’íŠK‘w(back)‚Éˆê‚Â‘O‚ÌŒZ’íŠK‘w(sibling_hierarchy_comp)‚ðƒZƒbƒg
							// -> ‚±‚ÌŠK‘w(ƒGƒ“ƒeƒBƒeƒB)‚ª––’[‚ÌŒZ’íŠK‘w‚É‚È‚é
							back_sibling = sibling_hierarchy_ent;
							break;
						}
						// ŒZ’íŠK‘w‚ÅÅŒã‚ÌŠK‘w‚Å‚Í‚È‚©‚Á‚½‚½‚ßŽŸ‚ÌŒZ’íŠK‘w‚Éi‚ß‚Äƒ‹[ƒv‚·‚é
						sibling_hierarchy_ent = sibling_hierarchy_comp.GetNextSibling();
					}
				}
			}
		}

		//-- ˆÊ’u‚ÌÄŒvŽZ --//
		if(!parent_ent.empty())
		{// Œ»ÝƒZƒbƒg‚µ‚æ‚¤‚Æ‚µ‚Ä‚¢‚ée‚ðŽ‚Á‚Ä‚¢‚ê‚Î
			TransformComponent& my_transform_comp = registry->GetComponent<TransformComponent>(GetEntity());
			const DirectX::SimpleMath::Matrix m{ registry->GetComponent<TransformComponent>(parent_ent).GetWorldMatrix().Invert() * GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorldMatrix()};
			DirectX::SimpleMath::Vector3    t{};
			DirectX::SimpleMath::Quaternion r{};
			DirectX::SimpleMath::Vector3    s{};

			if(arithmetic::DecomposeMatrix(t, r, s, m))
			{
				my_transform_comp.SetPosition(t);
				my_transform_comp.SetScale(s);
				my_transform_comp.SetRotation(r);
				const DirectX::SimpleMath::Vector3 euler = arithmetic::ConvertQuaternionToEuler(r);
				my_transform_comp.SetEulerAngles(euler);
			}
		}
		else
		{// ˆê”Ôã‚ÌeŠK‘w‚É‚È‚Á‚½‚Æ‚«
			if(!parent_entity.empty())
			{// ˆÈ‘O‚Ée‚ðŽ‚Á‚Ä‚¢‚ê‚Î
				TransformComponent& transform_comp	  = registry->GetComponent<TransformComponent>(GetEntity());
				const DirectX::SimpleMath::Matrix m{ transform_comp.GetWorldMatrix() };
				DirectX::SimpleMath::Vector3    t{};
				DirectX::SimpleMath::Quaternion r{};
				DirectX::SimpleMath::Vector3    s{};

				if (arithmetic::DecomposeMatrix(t, r, s, m))
				{
					transform_comp.SetPosition(t);
					transform_comp.SetScale(s);
					transform_comp.SetRotation(r);
					const DirectX::SimpleMath::Vector3 euler = arithmetic::ConvertQuaternionToEuler(r);
					transform_comp.SetEulerAngles(euler);
				}
			}

		}

		parent_entity = parent_ent;
	}

	void HierarchyComponent::ActivateDirtyFlg()
	{
		is_dirty = true;
		// ŽŸ‚ÌŒZ’íŠK‘w‚ª‚È‚¯‚ê‚Îˆ—‚ð”²‚¯‚é
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
