#include "enemy_slime_component.h"

#include <string>

#include "arithmetic.h"
#include "ecs.h"
#include "fbx_model_resource.h"
// components
#include "fbx_model_component.h"
#include "rigid_body_component.h"
#include "transform_component.h"
#include "player_component.h"

// std::unordered_map<std::string, RandomFloat> transition_timerのキー値用変数
namespace
{
	const std::string transition_idle_to_walk{ "idle_to_walk" };
	const std::string transition_walk_to_idle{ "walk_to_idle" };

}

namespace cumulonimbus::component
{
	EnemySlimeComponent::EnemySlimeComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:EnemyBaseComponent{ registry,ent }
	{
		// stateに応じての処理の登録
		slime_state.AddState(SlimeState::T_Pose			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).TPose(dt); });
		slime_state.AddState(SlimeState::Idle			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Idle(dt); });
		slime_state.AddState(SlimeState::Walk			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Walk(dt); });
		slime_state.AddState(SlimeState::Tracking		, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Tracking(dt); });
		slime_state.AddState(SlimeState::Birth			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Birth(dt); });
		slime_state.AddState(SlimeState::Death			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Death(dt); });
		slime_state.AddState(SlimeState::Damage			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Damage(dt); });
		slime_state.AddState(SlimeState::Stun			, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).Stun(dt); });
		slime_state.AddState(SlimeState::Attack_Bite	, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).AttackBite(dt); });
		slime_state.AddState(SlimeState::Attack_Charge	, [ent, registry](const float dt) { registry->GetComponent<EnemySlimeComponent>(ent).AttackCharge(dt); });

		// 初期stateの設定(SlimeState::Idle)
		slime_state.SetState(SlimeState::Idle);

		// transition_timerの設定
		RegisterTransitionTimer(transition_idle_to_walk, 3.f, 7.f);
		RegisterTransitionTimer(transition_walk_to_idle, 3.f, 5.f);


		// yaw回転でのランダム値設定
		SetRandomRotationAngle(-180.f, 180.f);
	}

	void EnemySlimeComponent::Update(float dt)
	{
		slime_state.Update(dt);
	}

	void EnemySlimeComponent::RenderImGui()
	{
	}

	void EnemySlimeComponent::Load(ecs::Registry* registry)
	{
	}

	int EnemySlimeComponent::GetAnimDataIndex(const AnimationData anim_data) const
	{
		return  static_cast<int>(anim_data);
	}

	void EnemySlimeComponent::TPose(float dt)
	{
	}

	void EnemySlimeComponent::Idle(const float dt)
	{
		auto& timer = transition_timer.at(transition_idle_to_walk);

		if(slime_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Idle_00)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Idle_00), true);
			// 遷移時間のランダム値の再設定
			timer.SetRandomVal();
		}

		timer.current_time += dt;

		const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
		if(Search(GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(), tracking_transition_angle, tracking_transition_distance))
		{
			// 索敵範囲内にプレイヤーがいれば状態遷移(SlimeState::Tracking)
			slime_state.SetState(SlimeState::Tracking);
			timer.current_time = 0.0f;
			return;
		}

		if (timer.current_time < timer.random_val)
			return;

		// 状態遷移(SlimeState::Walk)
		slime_state.SetState(SlimeState::Walk);
		timer.current_time = 0.0f;
	}

	void EnemySlimeComponent::Walk(const float dt)
	{
		auto& fbx_model_comp	= GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		auto& rigid_body_comp	= GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
		auto& transform_comp	= GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& timer				= transition_timer.at(transition_walk_to_idle);
		if(slime_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Walk)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Walk), true);
			transform_comp.ActiveQuaternion();
			{// モデル回転の初期設定
				random_rotation_angle.SetRandomVal();
				random_rotation_angle.current_time = 0;
				const DirectX::SimpleMath::Quaternion rotation_result_q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0,1,0 }, DirectX::XMConvertToRadians(random_rotation_angle.random_val));
				transform_comp.SetQuaternionSlerp(DirectX::SimpleMath::Quaternion::Identity, rotation_result_q);
			}
			{// 状態遷移タイマーの初期設定
				timer.current_time = 0;
				timer.SetRandomVal();
			}
		}

		random_rotation_angle.current_time += dt * rotation_time_rate;
		transform_comp.QuaternionSlerp(random_rotation_angle.current_time);

		if(random_rotation_angle.current_time > 1)
		{
			random_rotation_angle.current_time = 1;
		}
		rigid_body_comp.AddForce({ walk_speed,0.0f,walk_speed });

		const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
		if (Search(GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(), tracking_transition_angle, tracking_transition_distance))
		{// 索敵範囲内にプレイヤーがいれば状態遷移(SlimeState::Tracking)
			slime_state.SetState(SlimeState::Tracking);
			timer.current_time = 0;
		}

		timer.current_time += dt;
		if (timer.current_time < timer.random_val)
			return;

		slime_state.SetState(SlimeState::Idle);
		timer.current_time = 0;
	}

	void EnemySlimeComponent::Tracking(const float dt)
	{
		auto& fbx_model_comp	= GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		auto& rigid_body_comp	= GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
		auto& transform_comp	= GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		float distance = 0; // 自身とプレイヤーとの距離
		if(slime_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Walk)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Walk), true);
		}

		{// プレイヤーの位置をもとに回転させ、位置を更新
			// Todo : PlayerComponentを持つエンティティの取得方法を変更したい
			const mapping::rename_type::Entity player_ent = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
			DirectX::SimpleMath::Vector3 player_pos = GetRegistry()->GetComponent<TransformComponent>(player_ent).GetPosition();
			player_pos.y = 0;
			DirectX::SimpleMath::Vector3 self_pos = transform_comp.GetPosition();
			self_pos.y = 0;

			// 自身(スライム)からプレイヤーへのベクトルを作成
			DirectX::SimpleMath::Vector3 self_to_player_vec = player_pos - self_pos;
			distance = self_to_player_vec.Length();
			self_to_player_vec.Normalize();

			DirectX::SimpleMath::Vector3 self_front_vec = transform_comp.GetModelFront();
			self_front_vec.y = 0;
			self_front_vec.Normalize();

			float rad = arithmetic::CalcAngleFromTwoVec(self_to_player_vec, self_front_vec);

			if (!arithmetic::IsEqual(rad, 0.0f))
			{
				if (self_front_vec.Cross(self_to_player_vec).y < 0)
					rad *= -1;

				transform_comp.AdjustRotationFromAxis({ 0,1,0 }, rad);
			}
			rigid_body_comp.AddForce({ walk_speed, 0.0f, walk_speed });
		}

		{// 状態の切り替え
			if(distance > tracking_interruption_distance)
			{// 索敵距離外なら待機状態に遷移
				slime_state.SetState(SlimeState::Idle);
			}

			if(distance < attack_transition_distance)
			{// プレイヤーと自身の距離範囲内なら攻撃状態に遷移
				slime_state.SetState(SlimeState::Attack_Bite);
			}
		}
	}

	void EnemySlimeComponent::Birth(float dt)
	{
	}

	void EnemySlimeComponent::Death(float dt)
	{
	}

	void EnemySlimeComponent::Damage(float dt)
	{
	}

	void EnemySlimeComponent::Stun(float dt)
	{
	}

	void EnemySlimeComponent::AttackBite(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if(slime_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Attack_Bite)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Bite), false);
			//-- プレイヤーの方向に向きを変える --//
			RotateToPlayerDirection();
		}

		if (fbx_model_comp.IsPlayAnimation())
			return;

		slime_state.SetState(SlimeState::Idle);
	}

	void EnemySlimeComponent::AttackCharge(float dt)
	{
	}
} // cumulonimbus::component
