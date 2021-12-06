#include "soldier_sword_component.h"
// components
#include "capsule_collison_component.h"
#include "damageable_component.h"
#include "hierarchy_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::SoldierSwordComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::WeaponComponent, cumulonimbus::component::SoldierSwordComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::SoldierSwordComponent, 0)


namespace cumulonimbus::component
{
	template <class Archive>
	void SoldierSwordComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<WeaponComponent>(this)
		);
	}

	template <class Archive>
	void SoldierSwordComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<WeaponComponent>(this)
		);
	}

	SoldierSwordComponent::SoldierSwordComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:WeaponComponent{ registry,ent }
	{
	}

	SoldierSwordComponent::SoldierSwordComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const SoldierSwordComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	SoldierSwordComponent::SoldierSwordComponent(const mapping::component_tag::ComponentTag tag)
		:WeaponComponent{ tag }
	{
	}

	void SoldierSwordComponent::Start()
	{
		if (auto* capsule_collision = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
			capsule_collision)
		{
			capsule_collision->RegisterEventEnter(GetEntity(), [ent = GetEntity(), registry = GetRegistry()](const collision::HitResult& hit_result){ registry->GetComponent<SoldierSwordComponent>(ent).OnHit(hit_result); });
		}
	}

	void SoldierSwordComponent::RenderImGui()
	{
		if(GetRegistry()->CollapsingHeader<SoldierSwordComponent>(GetEntity(),"Soldier Sword"))
		{

		}
	}

	void SoldierSwordComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void SoldierSwordComponent::OnHit(const collision::HitResult& hit_result)
	{
		DamageData damage_data{};
		HierarchyComponent& hierarchy_comp = GetRegistry()->GetComponent<HierarchyComponent>(GetEntity());

		if (const mapping::rename_type::Entity& parent_ent = hierarchy_comp.GetParentEntity();
			hit_result.entity == parent_ent)
			return;

		// 同じエネミーのタグの場合処理を飛ばす
		if (hit_result.collision_tag == collision::CollisionTag::Enemy)
			return;

		if(auto damageable_comp = hit_result.registry->TryGetComponent<DamageableComponent>(hit_result.entity);
			damageable_comp)
		{
			damageable_comp->OnDamaged(hit_result.entity, damage_data, hit_result);
		}
	}

} // cumulonimbus::component
