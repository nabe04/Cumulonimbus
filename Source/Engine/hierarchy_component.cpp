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
		{// ˆê”Ôã‚ÌeŠK‘wˆ—
			if(first_child.empty())
			{
				first_child  = selected_ent;
				next_sibling = {};
				back_sibling = {};
			}
		}
		else
		{// ŽqŠK‘wˆ—
			const auto& parent_first_child		= registry->GetComponent<HierarchyComponent>(parent_ent).GetFirstChild();
			const auto& parent_first_child_next = registry->GetComponent<HierarchyComponent>(parent_first_child).GetNextSibling();
			if(parent_first_child_next.empty())
			{// eŠK‘w‚Ìfirst_child‚ªŽŸ‚ÌŒZ’íŠK‘w(next)‚ðŽ‚Á‚Ä‚¢‚È‚¢ê‡
				// eŠK‘w‚Ìfirst_childæ‚ÌƒGƒ“ƒeƒBƒeƒB‚Ìnext‚ÉŒ»Ý‚ÌƒGƒ“ƒeƒBƒeƒB(selected_ent)‚ðƒZƒbƒg
				registry->GetComponent<HierarchyComponent>(parent_first_child).SetNext(selected_ent);
				// Œ»Ý‚ÌƒGƒ“ƒeƒBƒeƒB(selected_ent)‚Ìback‚ÉeŠK‘w‚Ìfirst_childƒGƒ“ƒeƒBƒeƒB‚ðƒZƒbƒg
				back_sibling = parent_first_child;
			}
			else
			{// eŠK‘w‚Ìfirst_child‚ªŽŸ‚ÌŒZ’íŠK‘w(next)‚ðŽ‚Á‚Ä‚¢‚éê‡
				// ŒZ’íŠK‘w—pƒGƒ“ƒeƒBƒeƒB(eŠK‘w‚Ìfirst_child‚ªŽ‚Ânext‚ÌŽw‚·ƒGƒ“ƒeƒBƒeƒB‚©‚çŽn‚ß‚é)
				mapping::rename_type::Entity sibling_hierarchy_ent = parent_first_child_next;
				while(true)
				{
					auto& sibling_hierarchy_comp = registry->GetComponent<HierarchyComponent>(sibling_hierarchy_ent);
					if(sibling_hierarchy_comp.GetNextSibling().empty())
					{// ŒZ’íŠK‘w‚ÌŽŸ‚ÌŠK‘w(next)‚ª“o˜^‚³‚ê‚Ä‚¢‚È‚¢ -> Œ»’iŠK‚ÌŒZ’íŠK‘w‚Ì’†‚Åˆê”Ô––’[‚ÌŠK‘w
						// ŒZ’íŠK‘w‚ÌŽŸ‚ÌŒZ’íŠK‘w(next)‚ÉŒ»Ý‚ÌƒGƒ“ƒeƒBƒeƒB‚ðƒZƒbƒg
						sibling_hierarchy_comp.SetNext(selected_ent);
						// Œ»Ý‚ÌƒGƒ“ƒeƒBƒeƒB‚Ì‘O‚ÌŒZ’íŠK‘w(back)‚Éˆê‚Â‘O‚ÌŒZ’íŠK‘w(sibling_hierarchy_comp)‚ðƒZƒbƒg
						// -> ‚±‚ÌŠK‘w(ƒGƒ“ƒeƒBƒeƒB)‚ª––’[‚ÌŒZ’íŠK‘w‚É‚È‚é
						back_sibling = sibling_hierarchy_ent;
						break;
					}
					sibling_hierarchy_ent = sibling_hierarchy_comp.GetNextSibling();
				}
			}
		}
	}

} // cumulonimbus::component
