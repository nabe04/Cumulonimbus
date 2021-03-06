#include "player_sword_component.h"
// components
#include "capsule_collison_component.h"
#include "damageable_component.h"
#include "effekseer_component.h"
#include "hierarchy_component.h"
#include "player_component.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::PlayerSwordComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::WeaponComponent, cumulonimbus::component::PlayerSwordComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::PlayerSwordComponent, 1)

namespace cumulonimbus::component
{
	template <class Archive>
	void PlayerSwordComponent::load(Archive&& archive, const uint32_t version)
	{
		if(version == 1)
		{
			archive(
				cereal::base_class<WeaponComponent>(this)
			);
		}
	}

	template <class Archive>
	void PlayerSwordComponent::save(Archive&& archive, const uint32_t version) const
	{
		if(version == 1)
		{
			archive(
				cereal::base_class<WeaponComponent>(this)
			);
		}
	}

	PlayerSwordComponent::PlayerSwordComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:WeaponComponent{ registry,ent }
	{
	}

	PlayerSwordComponent::PlayerSwordComponent(
		ecs::Registry* registry,
		mapping::rename_type::Entity ent,
		const PlayerSwordComponent& copy_comp)
		: WeaponComponent{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	PlayerSwordComponent::PlayerSwordComponent(mapping::component_tag::ComponentTag tag)
		:WeaponComponent{ tag }
	{

	}

	void PlayerSwordComponent::Start()
	{
		if(auto* capsule_collision = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
		   capsule_collision)
		{
			capsule_collision->RegisterEventEnter(GetEntity(), [ent = GetEntity(), registry = GetRegistry()](const collision::HitResult& hit_result){registry->GetComponent<PlayerSwordComponent>(ent).OnHit(hit_result); });
			capsule_collision->SetAllCollisionEnable(false);
		}
	}

	void PlayerSwordComponent::GameUpdate(float dt)
	{
	}


	void PlayerSwordComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<PlayerSwordComponent>(GetEntity(), "Player Sword"))
		{
			auto name = GetRegistry()->GetName(GetEntity());
		}
	}

	void PlayerSwordComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void PlayerSwordComponent::RegistryApplyDamage(const mapping::rename_type::Entity& ent, const std::function<void(u_int)>& func)
	{
		damage_event.RegistryEvent(ent, func);
	}

	void PlayerSwordComponent::OnHit(const collision::HitResult& hit_result)
	{
		DamageData damage_data{};
		HierarchyComponent& hierarchy_comp = GetRegistry()->GetComponent<HierarchyComponent>(GetEntity());

		// ?e???G???e?B?e?B????
		const mapping::rename_type::Entity& parent_ent = hierarchy_comp.GetParentEntity();
		// ?????????e??????????????????
		if (hit_result.entity == parent_ent)
			return;

		// ?????????R???W?????^?O??Enemy???O??????????????????
		if (hit_result.collision_tag != collision::CollisionTag::Enemy)
			return;

		if (auto* player_comp = GetRegistry()->TryGetComponent<PlayerComponent>(parent_ent);
			player_comp)
		{
			damage_data.knock_back_level = player_comp->GetAtkKnockBackLevel();
			damage_data.damage_amount	 = player_comp->GetCurrentDamageAmount();

			const auto effect_ent	= GetRegistry()->CreateEntity();
			auto& transform_comp	= GetRegistry()->GetComponent<TransformComponent>(effect_ent);
			auto& effect_comp		= GetRegistry()->AddComponent<EffekseerComponent>(effect_ent);

			transform_comp.SetPosition(hit_result.position);
			transform_comp.SetScale({ 5,5,5 });

			effect_comp.ChangeEffect(player_comp->GetCurrentHitEffectId());
			effect_comp.SetIsDeleteAtEndOfSpawnTime(true);
			effect_comp.SetIsDeleteEntity(true);
			effect_comp.Play();
		}

		if(auto* damageable_comp = hit_result.registry->TryGetComponent<DamageableComponent>(hit_result.entity);
		   damageable_comp)
		{
			damageable_comp->OnDamaged(hit_result.entity, damage_data, hit_result);
		}
	}

} // cumulonimbus::component