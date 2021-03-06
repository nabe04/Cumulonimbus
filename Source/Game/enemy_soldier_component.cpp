#include "enemy_soldier_component.h"

#include <string>

#include "arithmetic.h"
#include "ecs.h"
#include "locator.h"
#include "fbx_model_resource.h"
#include "model_loader.h"
// components(Engine)
#include "capsule_collison_component.h"
#include "damageable_component.h"
#include "model_component.h"
#include "player_component.h"
#include "rigid_body_component.h"
#include "sphere_collision_component.h"
#include "transform_component.h"
// components(Geme)
#include "hierarchy_component.h"
#include "soldier_sword_component.h"

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

namespace keyframe_event
{
	const std::string event_1{ "event_1" };
} // keyframe_event

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


	EnemySoldierComponent::EnemySoldierComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:EnemyBaseComponent{ registry,ent }
	{
		Initialize(registry, ent);
	}

	EnemySoldierComponent::EnemySoldierComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent, const EnemySoldierComponent& copy_comp)
		: EnemyBaseComponent{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
		Initialize(registry, ent);
	}

	EnemySoldierComponent::EnemySoldierComponent(mapping::component_tag::ComponentTag tag)
		:EnemyBaseComponent{ tag }
	{

	}

	void EnemySoldierComponent::Start()
	{
		Initialize(GetRegistry(), GetEntity());

		hp = 5;

		const auto registry = this->GetRegistry();
		const auto entity   = this->GetEntity();
		// ダメージイベントの登録
		auto& damageable_comp = registry->GetOrEmplaceComponent<DamageableComponent>(GetEntity());
		damageable_comp.RegistryDamageEvent(GetEntity(), [=](const DamageData& damage_data, const collision::HitResult& hit_result) { registry->GetComponent<EnemySoldierComponent>(entity).OnDamaged(damage_data, hit_result); });
		//  コリジョンイベントの登録
		if(auto* capsule_collision = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
		   capsule_collision)
		{
			capsule_collision->RegisterEventEnter(GetEntity(), [=](const collision::HitResult& hit_result) { registry->GetComponent<EnemySoldierComponent>(entity).OnDamaged(hit_result); });
		}
		if (auto* sphere_collision = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
			sphere_collision)
		{
			sphere_collision->RegisterEventEnter(GetEntity(), [=](const collision::HitResult& hit_result) { registry->GetComponent<EnemySoldierComponent>(entity).OnAttack(hit_result); });
		}
		// 子階層からSoldierSwordComponentを持つエンティティの取得
		if (auto* hierarchy_comp = GetRegistry()->TryGetComponent<HierarchyComponent>(GetEntity());
			hierarchy_comp)
		{
			const auto sub_hierarchies = hierarchy_comp->GetSubHierarchies();
			for (const auto& sub_hierarchy : sub_hierarchies)
			{
				if (!GetRegistry()->HasComponent<SoldierSwordComponent>(sub_hierarchy))
					continue;

				sword_ent = sub_hierarchy;
				break;
			}
		}
	}

	void EnemySoldierComponent::GameUpdate(const float dt)
	{
		soldier_state.Update(dt);
		Movement();
	}

	void EnemySoldierComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<EnemySoldierComponent>(GetEntity(), "Soldier"))
		{
			ImGui::Text("Current Hp : %d", hp);
		}
	}

	void EnemySoldierComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	int EnemySoldierComponent::GetAnimDataIndex(AnimationData anim_data) const
	{
		return static_cast<int>(anim_data);
	}

	void EnemySoldierComponent::Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent)
	{
		InitializeKeyframeEvent();

		auto& damageable_comp = registry->GetOrEmplaceComponent<DamageableComponent>(ent);
		damageable_comp.RegistryDamageEvent(ent, [registry, ent](const DamageData& damage_data, const collision::HitResult& hit_result) { registry->GetComponent<EnemySoldierComponent>(ent).OnDamaged(damage_data, hit_result); });

		// stateに応じての処理の登録
		soldier_state.AddState(SoldierState::Idle		, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Idle(dt); });
		soldier_state.AddState(SoldierState::Wave_Start , [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).WaveStart(dt); });
		soldier_state.AddState(SoldierState::Walk		, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Walk(dt); });
		soldier_state.AddState(SoldierState::Tracking	, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Tracking(dt); });
		soldier_state.AddState(SoldierState::Damage		, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Damage(dt); });
		soldier_state.AddState(SoldierState::Death		, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Death(dt); });
		soldier_state.AddState(SoldierState::Attack_01	, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Attack01(dt); });
		soldier_state.AddState(SoldierState::Attack_02	, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Attack02(dt); });
		soldier_state.AddState(SoldierState::Attack_03	, [ent, registry](const float dt) {registry->GetComponent<EnemySoldierComponent>(ent).Attack03(dt); });

		// 初期stateの設定(SoldierState::Wave_Start)
		//soldier_state.SetState(SoldierState::Wave_Start);
		soldier_state.SetState(SoldierState::Idle);

		//// Todo : 敵の回避テスト
		//soldier_state.SetState(SoldierState::Attack_01);

		// transition_timerの設定
		RegisterTransitionTimer(transition_idle_to_walk, 3.f, 7.f);
		RegisterTransitionTimer(transition_walk_to_idle, 3.f, 5.f);
		RegisterTransitionTimer(transition_tracking_to_attack, 0.5f, 1.5f);

		// yaw回転でのランダム値設定
		SetRandomRotationAngle(-180.f, 180.f);
	}

	void EnemySoldierComponent::InitializeKeyframeEvent()
	{
		RegistryKeyframeEvent(AnimationData::Attack_01, keyframe_event::event_1);
		RegistryKeyframeEvent(AnimationData::Attack_02, keyframe_event::event_1);

		GetKeyframeEvent(AnimationData::Attack_01).SetKeyEvent(keyframe_event::event_1, 0, 16);
		GetKeyframeEvent(AnimationData::Attack_02).SetKeyEvent(keyframe_event::event_1, 20, 30);
	}

	void EnemySoldierComponent::Movement()
	{
		auto* rigid_body_comp = GetRegistry()->TryGetComponent<RigidBodyComponent>(GetEntity());
		if (!rigid_body_comp)
			return;

		if (auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
			transform_comp.GetPosition().y < 0)
		{
			rigid_body_comp->SetCurrentGravity(0);
			transform_comp.SetPosition_Y(.0f);
		}
	}

	void EnemySoldierComponent::RegistryKeyframeEvent(AnimationData anim_data, const std::string& key_name)
	{
		if (keyframe_events.contains(anim_data))
		{
			keyframe_events.at(anim_data).RegistryEvent(key_name);
			return;
		}

		keyframe_events.emplace(anim_data, system::KeyframeEvent{});
		keyframe_events.at(anim_data).RegistryEvent(key_name);
	}

	system::KeyframeEvent& EnemySoldierComponent::GetKeyframeEvent(const AnimationData anim_data)
	{
		if (!keyframe_events.contains(anim_data))
			assert(!"Don't have KeyframeEvent(PlayerComponent::GetKeyframeEvent)");

		return keyframe_events.at(anim_data);
	}

	void EnemySoldierComponent::OnAttack(const collision::HitResult& hit_result)
	{
		auto name = hit_result.registry->GetName(hit_result.entity);

		// ヒット先のタグがPlayer以外なら処理を飛ばす
		if (hit_result.collision_tag != collision::CollisionTag::Player)
			return;

		if (arithmetic::RandomIntInRange(0, 1) == 0)
		{
			soldier_state.SetState(SoldierState::Attack_01);
		}
		else
		{
			soldier_state.SetState(SoldierState::Attack_02);
		}
	}

	void EnemySoldierComponent::OnDamaged(const collision::HitResult& hit_result)
	{
		//GetRegistry()->AddDestroyEntity(GetEntity());
	}

	void EnemySoldierComponent::OnDamaged(const DamageData& damage_data, const collision::HitResult& hit_result)
	{
		if (hp <= 0)
			return;

		hp -= damage_data.damage_amount;

		if(hp <= 0)
		{
			soldier_state.SetState(SoldierState::Death);
		}
		else
		{
			soldier_state.SetState(SoldierState::Damage);
		}
	}

	void EnemySoldierComponent::Idle(float dt)
	{
		auto& timer = transition_timer.at(transition_idle_to_walk);

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット
			GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Idle), true);
		}

		//if (soldier_state.GetInitialize())
		//{
		//	// アニメーションセット
		//	GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Idle));
		//	// 遷移時間のランダム値の再設定
		//	timer.SetRandomVal();
		//}

		//timer.current_time += dt;

		//const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
		//if (Search(GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(), tracking_transition_angle, tracking_transition_distance))
		//{
		//	// 索敵範囲内にプレイヤーがいれば状態遷移(SoldierState::Tracking)
		//	soldier_state.SetState(SoldierState::Tracking);
		//	timer.current_time = 0.0f;
		//	return;
		//}

		//if (timer.current_time < timer.random_val)
		//	return;

		//// 状態遷移(SoldierState::Walk)
		//soldier_state.SetState(SoldierState::Walk);
		//timer.current_time = 0.0f;
	}

	void EnemySoldierComponent::WaveStart(const float dt)
	{
		auto& model_loader = *locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>();
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if(soldier_state.GetInitialize())
		{
			// Walkアニメーションのキーフレームを1フレームに変更
			model_loader.GetModel(model_comp.GetModelID()).GetModelData().SetAnimationKeyFrame(GetAnimDataIndex(AnimationData::Walk), 1);
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Walk));
		}

		//  Walkアニメーションのキーフレームをデフォルのキーフレームに戻す
		model_loader.GetModel(model_comp.GetModelID()).GetModelData().ResetAnimationKeyFrame(GetAnimDataIndex(AnimationData::Walk));

		// 状態遷移(SoldierState::Walk)
		soldier_state.SetState(SoldierState::Tracking);
	}

	void EnemySoldierComponent::Walk(const float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		auto& rigid_body_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& timer = transition_timer.at(transition_walk_to_idle);
		if (soldier_state.GetInitialize())
		{
			auto& model_loader = *locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>();
			// アニメーションセット(AnimationData::Walk)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Walk));
			model_loader.GetModel(model_comp.GetModelID()).GetModelData().ResetAnimationKeyFrame(static_cast<u_int>(AnimationData::Walk));
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

	void EnemySoldierComponent::Tracking(const float dt)
	{
		auto& timer = transition_timer.at(transition_tracking_to_attack);

		if (soldier_state.GetInitialize())
		{
			auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
			// アニメーションセット(AnimationData::Run)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Run), true);
			timer.SetRandomVal();
		}

		float distance = 0; // 自身とプレイヤーとの距離
		{// 追跡処理
			PlayerComponent* player_comp = GetPlayer();
			if (!player_comp)
				return;
			const mapping::rename_type::Entity ent_player = player_comp->GetEntity();
			EnemyBaseComponent::Tracking(player_comp->GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(),
										 { tracking_speed,.0f,tracking_speed },
										 distance);
		}

		{// 状態の切り替え
			//if (distance > tracking_interruption_distance)
			//{// 索敵範囲外なら待機状態に遷移(SoldierState::Idle)
			//	soldier_state.SetState(SoldierState::Idle);
			//}

			if (distance > tracking_interruption_distance)
			{// 索敵範囲外なら待機状態に遷移(SoldierState::Idle)
				//soldier_state.SetState(SoldierState::Idle);
				return;
			}

			//if (distance < attack_transition_distance)
			//	timer.current_time += dt;

			//// timerが一定値を超えない限り次の状態には遷移しない
			//if (timer.current_time < timer.random_val)
			//	return;

			//// プレイヤーとの距離が刺突距離以上なら状態をSoldierState::Attack_03に遷移
			//if (distance > attack_thrust_distance)
			//	soldier_state.SetState(SoldierState::Attack_03);

			//// それ以外はランダムで遷移
			//if(arithmetic::RandomIntInRange(0,1) == 0)
			//{
			//	soldier_state.SetState(SoldierState::Attack_01);
			//}
			//else
			//{
			//	soldier_state.SetState(SoldierState::Attack_02);
			//}

			timer.current_time = 0;
		}
	}

	void EnemySoldierComponent::Damage(float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());

		if(soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Damage)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Damage), false, 0.1f);
		}

		// アニメーション再生中は処理を中断
		if (model_comp.IsPlayAnimation())
			return;

		// 状態遷移(SoldierState::Tracking)
		soldier_state.SetState(SoldierState::Tracking);
	}

	void EnemySoldierComponent::Death(float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Die)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Die), false);
		}

		// アニメーション再生中は処理を中断
		if (model_comp.IsPlayAnimation())
			return;

		//GetRegistry()->AddDestroyEntity(GetEntity());
	}

	void EnemySoldierComponent::Attack01(float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Attack_01)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_01), false);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		GetKeyframeEvent(AnimationData::Attack_01).Update(GetRegistry(), GetEntity(), keyframe_event::event_1);
		if (GetKeyframeEvent(AnimationData::Attack_01).GetKeyEvent(keyframe_event::event_1).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}

		if (GetKeyframeEvent(AnimationData::Attack_01).GetKeyEvent(keyframe_event::event_1).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		// アニメーション再生中は処理を中断
		if (model_comp.IsPlayAnimation())
			return;

		//// Todo : 敵の回避テスト
		//soldier_state.SetState(SoldierState::Attack_01);

		// 状態遷移(SoldierState::Tracking)
		soldier_state.SetState(SoldierState::Tracking);
	}

	void EnemySoldierComponent::Attack02(float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());

		if (soldier_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Attack_02)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_02), false);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		GetKeyframeEvent(AnimationData::Attack_02).Update(GetRegistry(), GetEntity(), keyframe_event::event_1);
		if (GetKeyframeEvent(AnimationData::Attack_02).GetKeyEvent(keyframe_event::event_1).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}

		if (GetKeyframeEvent(AnimationData::Attack_02).GetKeyEvent(keyframe_event::event_1).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		// アニメーション再生中は処理を中断
		if (model_comp.IsPlayAnimation())
			return;

		// 状態遷移(SoldierState::Tracking)
		soldier_state.SetState(SoldierState::Tracking);
	}

	void EnemySoldierComponent::Attack03(float dt)
	{
		//auto& fbx_model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());

		//if (soldier_state.GetInitialize())
		//{
		//	// アニメーションセット(AnimationData::Attack_03)
		//	fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_03), false);
		//}

		//// アニメーション再生中は処理を中断
		//if (fbx_model_comp.IsPlayAnimation())
		//	return;

		//// 状態遷移(SoldierState::Idle)
		//soldier_state.SetState(SoldierState::Idle);
	}
} // cumulonimbus::component