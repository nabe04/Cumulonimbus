#include "energy_shot_component.h"

#include "damageable_component.h"
#include "effekseer_component.h"
#include "transform_component.h"
#include "sphere_collision_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EnergyShotComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::EnergyShotComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EnergyShotComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void EnergyShotComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	template <class Archive>
	void EnergyShotComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	EnergyShotComponent::EnergyShotComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
	}

	EnergyShotComponent::EnergyShotComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent, const EnergyShotComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void EnergyShotComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void EnergyShotComponent::Start()
	{
		if(auto* efk_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());
		   efk_comp)
		{
			efk_comp->Play();
		}

		if(auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
		   sphere_collision_comp)
		{
			sphere_collision_comp->RegisterEventEnter(GetEntity(), [ent = GetEntity(), registry = GetRegistry()](const collision::HitResult& hit_result){registry->GetComponent<EnergyShotComponent>(ent).OnHit(hit_result); });
			sphere_collision_comp->SetAllCollisionEnable(true);
		}
	}

	void EnergyShotComponent::GameUpdate(const float dt)
	{
		Move(dt);
	}

	void EnergyShotComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<EnergyShotComponent>(GetEntity(), "Energy Shot"))
		{

		}
	}

	void EnergyShotComponent::OnHit(const collision::HitResult& hit_result)
	{
		// 判定先のコリジョンタグがPlayer以外の場合処理を抜ける
		if (hit_result.collision_tag != collision::CollisionTag::Player)
			return;

		if (auto* damageable_comp = hit_result.registry->TryGetComponent<DamageableComponent>(hit_result.entity);
			damageable_comp)
		{
			DamageData damage_data{};
			damage_data.damage_amount = damage_amount;

			damageable_comp->OnDamaged(hit_result.entity, damage_data, hit_result);
		}
	}

	void EnergyShotComponent::Move(const float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		const auto pos = (transform_comp.GetModelFront() * shot_speed * dt) + transform_comp.GetPosition();
		transform_comp.SetPosition(pos);

		if (auto* efk_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());
			efk_comp)
		{
			efk_comp->SetPos(pos);
		}
	}
} // cumulonimbus::component
