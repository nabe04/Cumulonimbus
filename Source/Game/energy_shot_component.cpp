#include "energy_shot_component.h"

#include "easing.h"
#include "locator.h"
// components
#include "damageable_component.h"
#include "effekseer_component.h"
#include "transform_component.h"
#include "sphere_collision_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EnergyShotComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::EnergyShotComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EnergyShotComponent, 1)

namespace cumulonimbus::component
{
	template <class Archive>
	void EnergyShotComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(shot_speed),
				CEREAL_NVP(current_speed),
				CEREAL_NVP(initial_speed),
				CEREAL_NVP(max_speed),
				CEREAL_NVP(elapsed_time),
				CEREAL_NVP(max_survival_time),
				CEREAL_NVP(damage_amount)
			);
		}
	}

	template <class Archive>
	void EnergyShotComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(shot_speed),
			CEREAL_NVP(current_speed),
			CEREAL_NVP(initial_speed),
			CEREAL_NVP(max_speed),
			CEREAL_NVP(elapsed_time),
			CEREAL_NVP(max_survival_time),
			CEREAL_NVP(damage_amount)
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
		current_survive_time += dt;
		if(current_survive_time > max_survival_time)
		{
			GetRegistry()->AddDestroyEntity(GetEntity());
		}

		Move(dt);
	}


	void EnergyShotComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<EnergyShotComponent>(GetEntity(), "Energy Shot"))
		{
			ImGui::DragFloat("Survival Time"  , &max_survival_time, .5f, 0.0f, 20.f);
			ImGui::DragFloat("Initial Speed"  , &initial_speed	 , .5f, 0.0f, 1000.f);
			ImGui::DragFloat("Max Speed"	  , &max_speed		 , .5f, 0.0f, 1000.f);
			ImGui::DragFloat("Max Easing Time", &easing_max_time , .5f, 0.0f, 1000.f);
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

		elapsed_time += dt;
		const float speed = Easing::GetEasingVal(elapsed_time, initial_speed, max_speed - initial_speed, easing_max_time, CUBIC, ESOUT);

		const auto pos = (direction * speed * dt) + transform_comp.GetPosition();
		transform_comp.SetPosition(pos);

		if (auto* efk_comp = GetRegistry()->TryGetComponent<EffekseerComponent>(GetEntity());
			efk_comp)
		{
			efk_comp->SetPos(pos);
		}
	}
} // cumulonimbus::component
