#include "enemy_base_component.h"

#include "arithmetic.h"
#include "ecs.h"
#include "rename_type_mapping.h"
#include "locator.h"
#include "effekseer_loader.h"
// components
#include "player_component.h"
#include "player_sword_component.h"
#include "rigid_body_component.h"
#include "scene.h"
#include "scene_manager.h"
#include "transform_component.h"
#include "effekseer_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EnemyBaseComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::Actor3DComponent, cumulonimbus::component::EnemyBaseComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EnemyBaseComponent, 1)

namespace cumulonimbus::component
{
	template <class Archive>
	void EnemyBaseComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<Actor3DComponent>(this),
			CEREAL_NVP(transition_timer),
			CEREAL_NVP(random_rotation_angle)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(max_hp),
				CEREAL_NVP(hp)
			);
		}
	}

	template <class Archive>
	void EnemyBaseComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<Actor3DComponent>(this),
			CEREAL_NVP(transition_timer),
			CEREAL_NVP(random_rotation_angle),
			CEREAL_NVP(max_hp),
			CEREAL_NVP(hp)
		);
	}

	void EnemyBaseComponent::RandomFloat::SetRandomVal()
	{
		random_val = arithmetic::RandomFloatInRange(min, max);
	}

	EnemyBaseComponent::EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:Actor3DComponent{ registry,ent }
	{
		///registry->GetScene()->GetSceneManager()->GetActiveScenes()
	}

	EnemyBaseComponent::EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemyBaseComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	EnemyBaseComponent::EnemyBaseComponent(const mapping::component_tag::ComponentTag tag)
		:Actor3DComponent{ tag }
	{

	}

	void EnemyBaseComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void EnemyBaseComponent::SpawnHitEffect(
		const mapping::rename_type::UUID& effect_id,
		const DirectX::SimpleMath::Vector3& pos,
		const DirectX::SimpleMath::Vector3& scale) const
	{
		const auto spawn_ent = GetRegistry()->CreateEntity();
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(spawn_ent);
		auto& effekseer_comp = GetRegistry()->AddComponent<EffekseerComponent>(spawn_ent);

		transform_comp.SetPosition(pos);
		transform_comp.SetScale(scale);
		effekseer_comp.ChangeEffect(effect_id);
		effekseer_comp.SetIsDeleteAtEndOfSpawnTime(true);
		effekseer_comp.SetIsDeleteEntity(true);
		effekseer_comp.Play();
	}

	void EnemyBaseComponent::RegisterTransitionTimer(
		const std::string& key_value,
		const float min, const float max)
	{
		if(transition_timer.contains(key_value))
		{
			transition_timer.at(key_value).min = min;
			transition_timer.at(key_value).max = max;
			transition_timer.at(key_value).SetRandomVal();
		}

		const RandomFloat range{ min,max };
		transition_timer.insert(std::make_pair(key_value, range));
		transition_timer.at(key_value).SetRandomVal();
	}

	void EnemyBaseComponent::SetRandomRotationAngle(const float min, const float max)
	{
		random_rotation_angle.min = min;
		random_rotation_angle.max = max;
		random_rotation_angle.SetRandomVal();
	}

	bool EnemyBaseComponent::Search(
		const DirectX::SimpleMath::Vector3& target_pos,
		const float search_range, const float search_length) const
	{
		const auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		DirectX::SimpleMath::Vector3 self_to_target{ target_pos - transform_comp.GetPosition() };

		// ???G?????O?????????????f
		if (self_to_target.Length() > search_length)
			return false;
		// ???G?????O?????????????f
		self_to_target.Normalize();
		if (self_to_target.Dot(transform_comp.GetModelFront()) > DirectX::XMConvertToRadians(search_range / 2))
			return false;

		return true;
	}

	void EnemyBaseComponent::Tracking(
		DirectX::SimpleMath::Vector3 target_pos,
		const DirectX::SimpleMath::Vector3& velocity,
		float& distance)
	{
		auto& transform_comp  = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& rigid_body_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());

		target_pos.y = 0;
		DirectX::SimpleMath::Vector3 self_pos = transform_comp.GetPosition();
		self_pos.y = 0;
		// ???g?????^?[?Q?b?g?????x?N?g????????
		DirectX::SimpleMath::Vector3 self_to_target_vec{ target_pos - self_pos };
		distance = self_to_target_vec.Length();
		self_to_target_vec.Normalize();

		DirectX::SimpleMath::Vector3 self_front_vec = transform_comp.GetModelFront();
		self_front_vec.y = 0;
		self_front_vec.Normalize();

		if (float rad = arithmetic::CalcAngleFromTwoVec(self_to_target_vec, self_front_vec);
			!arithmetic::IsEqual(rad, 0.0f))
		{
			if (self_front_vec.Cross(self_to_target_vec).y < 0)
				rad *= -1;
			// ???f???????]????
			transform_comp.AdjustRotationFromAxis({ 0,1,0 }, rad);
		}

		// ??(???x)?????Z
		rigid_body_comp.AddForce({ velocity.x, velocity.y, velocity.z});
	}


	void EnemyBaseComponent::RotateToPlayerDirection() const
	{
		PlayerComponent* player_comp = GetPlayer();
		if (!player_comp)
			return;
		const mapping::rename_type::Entity player_ent = player_comp->GetEntity();
		const DirectX::SimpleMath::Vector3 player_pos = GetRegistry()->GetComponent<TransformComponent>(player_ent).GetPosition();
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		DirectX::SimpleMath::Vector3 self_to_player{ player_pos - transform_comp.GetPosition() };
		self_to_player.y = 0;
		self_to_player.Normalize();
		DirectX::SimpleMath::Vector3 model_front = transform_comp.GetModelFront();
		model_front.y = 0;
		model_front.Normalize();

		float dot = model_front.Dot(self_to_player);
		dot		  = arithmetic::Clamp(dot, -1.0f, 1.0f);
		float rad = acosf(dot);
		if (model_front.Cross(self_to_player).y < 0)
			rad *= -1;

		transform_comp.AdjustRotationFromAxis({ 0,1,0 }, rad);
	}

	PlayerComponent* EnemyBaseComponent::GetPlayer() const
	{
		for(const auto& active_scenes = GetRegistry()->GetScene()->GetSceneManager()->GetActiveScenes();
			const auto& active_scene : active_scenes | std::views::values)
		{
			auto& registry = *active_scene->GetRegistry();
			if (registry.GetComponentSize<PlayerComponent>() <= 0)
				continue;

			return &registry.GetArray<PlayerComponent>().GetComponents().at(0);
		}

		return nullptr;
	}

} // cumulonimbus::component
