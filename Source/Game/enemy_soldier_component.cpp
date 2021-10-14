#include "enemy_soldier_component.h"

#include <string>

#include "arithmetic.h"
#include "ecs.h"
#include "fbx_model_resource.h"
// components
#include "fbx_model_component.h"
#include "player_component.h"
#include "rigid_body_component.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EnemySoldierComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::EnemyBaseComponent, cumulonimbus::component::EnemySoldierComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EnemySoldierComponent, 0)

// std::unordered_map<std::string, RandomFloat> transition_timerのキー値用変数
namespace
{
	const std::string transition_idle_to_walk{ "idle_to_walk" };
	const std::string transition_walk_to_idle{ "walk_to_idle" };
	const std::string transition_tracking_to_attack{ "tracking_to_attack" };
}

namespace cumulonimbus::component
{
	template <class Archive>
	void EnemySoldierComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<EnemyBaseComponent>(this),
			CEREAL_NVP(soldier_state),
			CEREAL_NVP(rotation_time_rate),
			CEREAL_NVP(walk_speed),
			CEREAL_NVP(tracking_speed),
			CEREAL_NVP(tracking_interruption_distance),
			CEREAL_NVP(attack_transition_distance),
			CEREAL_NVP(attack_transition_angle),
			CEREAL_NVP(tracking_transition_distance),
			CEREAL_NVP(tracking_transition_angle),
			CEREAL_NVP(attack_thrust_distance)
		);
	}

	template <class Archive>
	void EnemySoldierComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<EnemyBaseComponent>(this),
			CEREAL_NVP(soldier_state),
			CEREAL_NVP(rotation_time_rate),
			CEREAL_NVP(walk_speed),
			CEREAL_NVP(tracking_speed),
			CEREAL_NVP(tracking_interruption_distance),
			CEREAL_NVP(attack_transition_distance),
			CEREAL_NVP(attack_transition_angle),
			CEREAL_NVP(tracking_transition_distance),
			CEREAL_NVP(tracking_transition_angle),
			CEREAL_NVP(attack_thrust_distance)
		);
	}

	
	EnemySoldierComponent::EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:EnemyBaseComponent{ registry,ent }
	{
		// stateに応じての処理の登録
		soldier_state.AddState(SoldierState::Idle		, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Idle(dt); });
		soldier_state.AddState(SoldierState::Walk		, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Walk(dt); });
		soldier_state.AddState(SoldierState::Tracking	, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Tracking(dt); });
		soldier_state.AddState(SoldierState::Damage		, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Damage(dt); });
		soldier_state.AddState(SoldierState::Death		, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Death(dt); });
		soldier_state.AddState(SoldierState::Attack_01	, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Attack01(dt); });
		soldier_state.AddState(SoldierState::Attack_02	, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Attack02(dt); });
		soldier_state.AddState(SoldierState::Attack_03	, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Attack03(dt); });

		// 初期stateの設定(SoldierState::Idle)
		soldier_state.SetState(SoldierState::Idle);

		// transition_timerの設定
		RegisterTransitionTimer(transition_idle_to_walk, 3.f, 7.f);
		RegisterTransitionTimer(transition_walk_to_idle, 3.f, 5.f);
		RegisterTransitionTimer(transition_tracking_to_attack, 0.5f, 1.5f);

		// yaw回転でのランダム値設定
		SetRandomRotationAngle(-180.f, 180.f);
	}

	EnemySoldierComponent::EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemySoldierComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void EnemySoldierComponent::GameUpdate(float dt)
	{
		soldier_state.Update(dt);
	}

	void EnemySoldierComponent::RenderImGui()
	{
	}

	void EnemySoldierComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	int EnemySoldierComponent::GetAnimDataIndex(AnimationData anim_data) const
	{
		return static_cast<int>(anim_data);
	}

	void EnemySoldierComponent::Idle(float dt)
	{
		auto& timer = transition_timer.at(transition_idle_to_walk);

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Idle));
			// 遷移時間のランダム値の再設定
			timer.SetRandomVal();
		}

		timer.current_time += dt;

		const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
		if (Search(GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(), tracking_transition_angle, tracking_transition_distance))
		{
			// 索敵範囲内にプレイヤーがいれば状態遷移(SoldierState::Tracking)
			soldier_state.SetState(SoldierState::Tracking);
			timer.current_time = 0.0f;
			return;
		}

		if (timer.current_time < timer.random_val)
			return;

		// 状態遷移(SoldierState::Walk)
		soldier_state.SetState(SoldierState::Walk);
		timer.current_time = 0.0f;
	}

	void EnemySoldierComponent::Walk(const float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		auto& rigid_body_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& timer = transition_timer.at(transition_walk_to_idle);
		if (soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Walk)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Walk));
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

		if (random_rotation_angle.current_time > 1)
		{
			random_rotation_angle.current_time = 1;
		}
		rigid_body_comp.AddForce({ walk_speed,0.0f,walk_speed });

		const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
		if (Search(GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(), tracking_transition_angle, tracking_transition_distance))
		{// 索敵範囲内にプレイヤーがいれば状態遷移(SoldierState::Tracking)
			soldier_state.SetState(SoldierState::Tracking);
			timer.current_time = 0;
		}

		timer.current_time += dt;
		if (timer.current_time < timer.random_val)
			return;

		soldier_state.SetState(SoldierState::Idle);
		timer.current_time = 0;
	}

	void EnemySoldierComponent::Tracking(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		auto& timer			 = transition_timer.at(transition_tracking_to_attack);

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Run)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Run), true);
			timer.SetRandomVal();
		}

		float distance = 0; // 自身とプレイヤーとの距離
		{// 追跡処理
			const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
			EnemyBaseComponent::Tracking(
				GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(),
				{ tracking_speed,.0f,tracking_speed },
				distance);
		}

		{// 状態の切り替え
			if (distance > tracking_interruption_distance)
			{// 索敵範囲外なら待機状態に遷移(SoldierState::Idle)
				soldier_state.SetState(SoldierState::Idle);
			}

			if (distance < attack_transition_distance)
				timer.current_time += dt;

			// timerが一定値を超えない限り次の状態には遷移しない
			if (timer.current_time < timer.random_val)
				return;

			// プレイヤーとの距離が刺突距離以上なら状態をSoldierState::Attack_03に遷移
			if (distance > attack_thrust_distance)
				soldier_state.SetState(SoldierState::Attack_03);

			// それ以外はランダムで遷移
			if(arithmetic::RandomIntInRange(0,1) == 0)
			{
				soldier_state.SetState(SoldierState::Attack_01);
			}
			else
			{
				soldier_state.SetState(SoldierState::Attack_02);
			}

			timer.current_time = 0;
		}
	}

	void EnemySoldierComponent::Damage(float dt)
	{
	}

	void EnemySoldierComponent::Death(float dt)
	{
	}

	void EnemySoldierComponent::Attack01(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Attack_01)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_01), false);
		}

		// アニメーション再生中は処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(SoldierState::Idle)
		soldier_state.SetState(SoldierState::Idle);
	}

	void EnemySoldierComponent::Attack02(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Attack_02)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_02), false);
		}

		// アニメーション再生中は処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(SoldierState::Idle)
		soldier_state.SetState(SoldierState::Idle);
	}

	void EnemySoldierComponent::Attack03(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Attack_03)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_03), false);
		}

		// アニメーション再生中は処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(SoldierState::Idle)
		soldier_state.SetState(SoldierState::Idle);
	}
} // cumulonimbus::component