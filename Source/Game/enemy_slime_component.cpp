#include "enemy_slime_component.h"

#include "ecs.h"
#include "fbx_model_resource.h"
// components
#include "fbx_model_component.h"
#include "movement_component.h"
#include "transform_component.h"

namespace cumulonimbus::component
{
	EnemySlimeComponent::EnemySlimeComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:EnemyBaseComponent{ registry,ent }
	{
		// stateに応じての処理の登録
		slime_state.AddState(SlimeState::T_Pose			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).TPose(dt); });
		slime_state.AddState(SlimeState::Idle			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Idle(dt); });
		slime_state.AddState(SlimeState::Walk			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Walk(dt); });
		slime_state.AddState(SlimeState::Birth			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Birth(dt); });
		slime_state.AddState(SlimeState::Death			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Death(dt); });
		slime_state.AddState(SlimeState::Hit			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Hit(dt); });
		slime_state.AddState(SlimeState::Stun			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Stun(dt); });
		slime_state.AddState(SlimeState::Attack_Bite	, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).AttackBite(dt); });
		slime_state.AddState(SlimeState::Attack_Charge	, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).AttackCharge(dt); });

		// 初期stateの設定(Idle)
		slime_state.SetState(SlimeState::Idle);

		// timer_rangeの設定
		AddTimerRange(SlimeState::Idle, { 3.f,7.f });
	}

	void EnemySlimeComponent::Update(float dt)
	{
	}

	void EnemySlimeComponent::RenderImGui()
	{
	}

	void EnemySlimeComponent::Save(const std::string& file_path)
	{
	}

	void EnemySlimeComponent::Load(const std::string& file_path_and_name)
	{
	}

	void EnemySlimeComponent::AddTimerRange(SlimeState state, const RangeTimerFloat& range)
	{
		if(timer_range.contains(state))
		{
			timer_range.at(state) = range;
			timer_range.at(state).SetRandomVal();
			return;
		}

		timer_range.insert(std::make_pair(state, range));
		timer_range.at(state).SetRandomVal();
	}

	int EnemySlimeComponent::GetAnimDataIndex(const AnimationData anim_data) const
	{
		return  static_cast<int>(anim_data);
	}



	void EnemySlimeComponent::TPose(float dt)
	{
	}

	void EnemySlimeComponent::Idle(float dt)
	{
		auto& timer = timer_range.at(SlimeState::Idle);

		if(slime_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Idle_00)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Idle_00), true);
			timer.SetRandomVal();
		}

		timer.current_time += dt;

		if (timer.current_time < timer.random_val)
			return;

		// 状態遷移(SlimeState::Walk)
		slime_state.SetState(SlimeState::Walk);
		timer.current_time = 0.0f;
	}

	void EnemySlimeComponent::Walk(float dt)
	{
		auto& movement_comp  = GetRegistry()->GetComponent<MovementComponent>(GetEntity());
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		if(slime_state.GetInitialize())
		{

		}

	}


	void EnemySlimeComponent::Birth(float dt)
	{
	}

	void EnemySlimeComponent::Death(float dt)
	{
	}

	void EnemySlimeComponent::Hit(float dt)
	{
	}

	void EnemySlimeComponent::Stun(float dt)
	{
	}

	void EnemySlimeComponent::AttackBite(float dt)
	{
	}

	void EnemySlimeComponent::AttackCharge(float dt)
	{
	}
} // cumulonimbus::component
