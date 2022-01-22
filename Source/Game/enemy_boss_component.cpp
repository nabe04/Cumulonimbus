#include "enemy_boss_component.h"

#include "arithmetic.h"
#include "locator.h"
#include "model_component.h"
#include "player_component.h"
#include "sphere_collision_component.h"
#include "transform_component.h"
#include "collider_message_receiver_component.h"
#include "collider_message_sender_component.h"
#include "capsule_collison_component.h"
#include "prefab_loader.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EnemyBossComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::EnemyBaseComponent, cumulonimbus::component::EnemyBossComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EnemyBossComponent, 1)

namespace keyframe_event
{
	const std::string event_1{ "Event_1" };
} // keyframe_event

namespace collision_name
{
	const std::string for_attack{ "atk_collider" };
	const std::string for_damage{ "damage_collider" };
} // collision_name
namespace cumulonimbus::component
{
	template <class Archive>
	void EnemyBossComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<EnemyBaseComponent>(this)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(walk_speed),
				CEREAL_NVP(dash_speed),
				CEREAL_NVP(atk_melee_distance),
				CEREAL_NVP(shot_prefab_id),
				CEREAL_NVP(spawn_shot_node_name)
			);
		}
	}

	template <class Archive>
	void EnemyBossComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<EnemyBaseComponent>(this)
		);

		if(version >= 0)
		{
			archive(
				CEREAL_NVP(walk_speed),
				CEREAL_NVP(dash_speed),
				CEREAL_NVP(atk_melee_distance),
				CEREAL_NVP(shot_prefab_id),
				CEREAL_NVP(spawn_shot_node_name)
			);
		}
	}

	EnemyBossComponent::EnemyBossComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:EnemyBaseComponent{ registry,ent }
	{
		EnemyBossComponent::Initialize(registry, ent);
	}

	EnemyBossComponent::EnemyBossComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemyBossComponent& copy_comp)
		: EnemyBaseComponent{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
		EnemyBossComponent::Initialize(registry, ent);
	}

	EnemyBossComponent::EnemyBossComponent(const mapping::component_tag::ComponentTag tag)
		:EnemyBaseComponent{ tag }
	{
	}

	void EnemyBossComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void EnemyBossComponent::Start()
	{
		// 履歴の初期化
		InitializeHistory();

		EnemyBossComponent& e_boss_comp = GetRegistry()->GetComponent<EnemyBossComponent>(GetEntity());

		//-- キーフレームイベント登録 --//
		// Atk_N_01
		RegistryKeyframeEvent(AnimationData::Attack_Normal_01, keyframe_event::event_1);
		// Atk_N_03
		RegistryKeyframeEvent(AnimationData::Attack_Normal_03, keyframe_event::event_1);

		//-- キーフレームイベントの範囲指定 --//
		// Atk_N_01
		GetKeyframeEvent(AnimationData::Attack_Normal_01).SetKeyEvent(keyframe_event::event_1, 16, 22);
		// Atk_N_03
		GetKeyframeEvent(AnimationData::Attack_Normal_03).SetKeyEvent(keyframe_event::event_1, 15, 20);

		//
		if (auto* collider_receiver = GetRegistry()->TryGetComponent<ColliderMessageReceiverComponent>(GetEntity());
			collider_receiver)
		{
			collider_receiver->RegisterReceivedMessage(collision::HitEvent::OnCollisionEnter, GetEntity(), [&e_boss_comp](ColliderMessageSenderComponent& sender) {e_boss_comp.OnEnterAttackRange(sender); });
			collider_receiver->RegisterReceivedMessage(collision::HitEvent::OnCollisionExit , GetEntity(), [&e_boss_comp](ColliderMessageSenderComponent& sender) {e_boss_comp.OnExitAttackRange(sender); });
		}

		if (auto* capsule_collider = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
			capsule_collider)
		{
			capsule_collider->RegisterEventEnter(GetEntity(), [&e_boss_comp](const collision::HitResult& hit_result) { e_boss_comp.OnAttack(hit_result); });

			if (auto* capsule = capsule_collider->TryGetCapsuleFromName(collision_name::for_attack);
				capsule)
			{
				capsule->is_enable = false;
			}
		}

		GetRegistry()->GetOrEmplaceComponent<DamageableComponent>(GetEntity()).RegistryDamageEvent(GetEntity(), [&e_boss_comp](const DamageData& damage_data, const collision::HitResult& hit_result) {e_boss_comp.OnDamaged(damage_data, hit_result); });

		//-- 行動(ビヘイビア)の更新StateMachineの追加 --//
		boss_behavior.AddState(BossBehavior::Move  , e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorMoveUpdate>());
		boss_behavior.AddState(BossBehavior::Damage, e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorDamageUpdate>());
		boss_behavior.AddState(BossBehavior::Attack, e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorAttackUpdate>());
		boss_behavior.SetState(BossBehavior::Move);

		movement_behavior.ClearAll();
		movement_behavior.AddSequence(MovementBehavior::Idle	 , e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::Idle>());
		movement_behavior.AddSequence(MovementBehavior::Tracking , e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::Tracking>());
		movement_behavior.AddSequence(MovementBehavior::Wandering, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::Wandering>());

		damage_behavior.ClearAll();
		damage_behavior.AddSequence(DamageBehavior::Damage, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::Damage>());
		damage_behavior.AddSequence(DamageBehavior::Death , e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::Death>());

		attack_behavior.ClearAll();
		//-- 攻撃BehaviorTreeの追加 --//
		// 通常攻撃 1
		attack_behavior.AddSequence(AttackBehavior::Atk_N1, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal01>());
		// 通常攻撃 2
		attack_behavior.AddSequence(AttackBehavior::Atk_N2, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal02>());
		// 通常攻撃 3
		attack_behavior.AddSequence(AttackBehavior::Atk_N3, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal03>());
		// 通常攻撃 4
		attack_behavior.AddSequence(AttackBehavior::Atk_N4, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal04>());
		// 通常攻撃(1 + 2)
		attack_behavior.AddSequence(AttackBehavior::Atk_N1_N2, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal01>());
		attack_behavior.AddSequence(AttackBehavior::Atk_N1_N2, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal02>());
		// 通常攻撃(1 + 3)
		attack_behavior.AddSequence(AttackBehavior::Atk_N1_N3, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal01>());
		attack_behavior.AddSequence(AttackBehavior::Atk_N1_N3, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal03>());
		// 通常攻撃(1 + 4)
		attack_behavior.AddSequence(AttackBehavior::Atk_N1_N4, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal01>());
		attack_behavior.AddSequence(AttackBehavior::Atk_N1_N4, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal04>());
		// 通常攻撃(2 + 1)
		attack_behavior.AddSequence(AttackBehavior::Atk_N2_N1, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal02>());
		attack_behavior.AddSequence(AttackBehavior::Atk_N2_N1, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal01>());
		// 通常攻撃(2 + 3)
		attack_behavior.AddSequence(AttackBehavior::Atk_N2_N3, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal02>());
		attack_behavior.AddSequence(AttackBehavior::Atk_N2_N3, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal03>());
		// 通常攻撃(2 + 4)
		attack_behavior.AddSequence(AttackBehavior::Atk_N2_N4, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal02>());
		attack_behavior.AddSequence(AttackBehavior::Atk_N2_N4, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal04>());
		// 遠距離攻撃 1
		attack_behavior.AddSequence(AttackBehavior::Atk_E1, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy01>());
		// 遠距離攻撃 2
		attack_behavior.AddSequence(AttackBehavior::Atk_E2, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy02>());
		// 遠距離攻撃(1 + 2)
		attack_behavior.AddSequence(AttackBehavior::Atk_E1_E2, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy01>());
		attack_behavior.AddSequence(AttackBehavior::Atk_E1_E2, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy02>());
		// 遠距離攻撃(2 + 1)
		attack_behavior.AddSequence(AttackBehavior::Atk_E2_E1, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal02>());
		attack_behavior.AddSequence(AttackBehavior::Atk_E2_E1, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal01>());
	}

	void EnemyBossComponent::CommonUpdate(const float dt)
	{

	}

	void EnemyBossComponent::GameUpdate(const float dt)
	{
		BehaviorUpdate(dt);

		is_in_attack_range = false;
	}

	void EnemyBossComponent::RenderImGui()
	{
		if(GetRegistry()->CollapsingHeader<EnemyBossComponent>(GetEntity(),"E Boss"))
		{
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& prefab_loader = *asset_manager.GetLoader<asset::PrefabLoader>();

			ImGui::Text("Shot Prefab");
			prefab_loader.ImSelectablePrefab(asset_manager, shot_prefab_id);

			if(auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());
			   model_comp)
			{
				ImGui::Text("Spawn Shot Pos");
				model_comp->ImSelectableNode(spawn_shot_node_name);
			}
		}

	}

	void EnemyBossComponent::Initialize(ecs::Registry* registry, const mapping::rename_type::Entity ent)
	{
		//EnemyBossComponent& e_boss_comp = registry->GetComponent<EnemyBossComponent>(ent);
		////-- 行動(ビヘイビア)の更新StateMachineの追加 --//
		//boss_behavior.AddState(BossBehavior::Move  , e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorMoveUpdate>());
		//boss_behavior.AddState(BossBehavior::Attack, e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorAttackUpdate>());
		//boss_behavior.SetState(BossBehavior::Attack);

		////-- 攻撃BehaviorTreeの追加 --//
		//// 通常攻撃 1
		//attack_behavior.AddSequence(AttackBehavior::Atk_Normal_01, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal01>());
		//// 通常攻撃 2
		//attack_behavior.AddSequence(AttackBehavior::Atk_Normal_02, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal02>());
		//// 通常攻撃 3
		//attack_behavior.AddSequence(AttackBehavior::Atk_Normal_03, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal03>());
		//// 通常攻撃 4
		//attack_behavior.AddSequence(AttackBehavior::Atk_Normal_04, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal04>());
		//// 遠距離攻撃 1
		//attack_behavior.AddSequence(AttackBehavior::Atk_Energy_01, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy01>());
		//// 遠距離攻撃 2
		//attack_behavior.AddSequence(AttackBehavior::Atk_Energy_02, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy02>());
	}

	void EnemyBossComponent::InitializeHistory()
	{
		const u_int half_count = max_history_count / 2;

		// 攻撃履歴の初期化
		attack_history.clear();
		for (u_int i = 0; i < half_count; ++i)
		{
			AddAttackHistory(AttackType::Atk_Melee);
			AddAttackHistory(AttackType::Atk_Long_Range);
		}

		// ビヘイビア履歴の初期化
		behavior_history.clear();
		for (u_int i = 0; i < half_count; ++i)
		{
			AddBehaviorHistory(BossBehavior::Move);
			AddBehaviorHistory(BossBehavior::Attack);
		}
	}

	void EnemyBossComponent::OnAttack(const collision::HitResult& hit_result)
	{
		// ヒット先のタグがPlayer以外なら処理を飛ばす
		if (hit_result.collision_tag != collision::CollisionTag::Player)
			return;

		auto* capsule_collider = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
		if (!capsule_collider)
			return;;

		auto* atk_capsule = capsule_collider->TryGetCapsuleFromName(collision_name::for_attack);
		if (!atk_capsule)
			return;

		for(const auto& atk_hit_result : atk_capsule->hit_results | std::views::values)
		{
			if(atk_hit_result.is_hit &&
			   (atk_hit_result.collision_tag == collision::CollisionTag::Player))
			{
				// 近距離攻撃処理
				if (auto* damageable_comp = hit_result.registry->TryGetComponent<DamageableComponent>(hit_result.entity);
					damageable_comp)
				{
					DamageData damage_data{};
					damage_data.damage_amount = 1;

					damageable_comp->OnDamaged(hit_result.entity, damage_data, hit_result);
				}

				break;
			}
		}
	}

	void EnemyBossComponent::OnDamaged(const DamageData& damage_data, const collision::HitResult& hit_result)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		boss_behavior.SetState(BossBehavior::Damage);
	}

	void EnemyBossComponent::RegistryKeyframeEvent(AnimationData anim_data, const std::string& key_name)
	{
		if (keyframe_events.contains(anim_data))
		{
			keyframe_events.at(anim_data).RegistryEvent(key_name);
			return;
		}

		keyframe_events.emplace(anim_data, system::KeyframeEvent{});
		keyframe_events.at(anim_data).RegistryEvent(key_name);
	}

	void EnemyBossComponent::AddAttackHistory(const AttackType type)
	{
		attack_history.emplace_back(type);

		// 最大格納数を超えたら一番古い履歴を削除
		if (attack_history.size() > max_history_count)
		{
		    // 一つあとの値を格納し最初の値を削除
			for (int i = 0; i < attack_history.size() - 1; ++i)
			{
				attack_history.at(i) = attack_history.at(i + 1);
			}

			attack_history.pop_back();
		}
	}

	void EnemyBossComponent::AddBehaviorHistory(BossBehavior type)
	{
		behavior_history.emplace_back(type);

		// 最大格納数を超えたら一番古い履歴を削除
		if (behavior_history.size() > max_history_count)
		{
			// 一つあとの値を格納し最初の値を削除
			for (int i = 0; i < behavior_history.size() - 1; ++i)
			{
				behavior_history.at(i) = behavior_history.at(i + 1);
			}

			behavior_history.pop_back();
		}
	}


	template <auto F>
	std::function<void(float)> EnemyBossComponent::GetBehaviorUpdateFunc()
	{
		return std::bind(F, this, std::placeholders::_1);
	}

	template <auto F>
	std::function<void(float, bool)> EnemyBossComponent::GetBehaviorActFunc()
	{
		return std::bind(F, this, std::placeholders::_1, std::placeholders::_2);
	}

	int EnemyBossComponent::GetAnimDataIndex(const AnimationData anim_data) const
	{
		return static_cast<int>(anim_data);
	}

	system::KeyframeEvent& EnemyBossComponent::GetKeyframeEvent(AnimationData anim_data)
	{
		if (!keyframe_events.contains(anim_data))
			assert(!"Don't have KeyframeEvent(EnemyBossComponent::GetKeyframeEvent)");

		return keyframe_events.at(anim_data);
	}

	EnemyBossComponent::AttackType EnemyBossComponent::GetNextAttackType() const
	{
		// 空の場合ランダムに選択する
		if (attack_history.empty())
		{
			return static_cast<AttackType>(arithmetic::RandomIntInRange(0, 1));
		}

		const int index = arithmetic::RandomIntInRange(0, static_cast<int>(attack_history.size() - 1));
		const AttackType type = attack_history.at(index);

		// 同じ攻撃を避けるため選択された履歴を逆の履歴を返す
		if (type == AttackType::Atk_Melee)
			return AttackType::Atk_Long_Range;

		if (type == AttackType::Atk_Long_Range)
			return AttackType::Atk_Melee;

		// 基本このパターンには入らない(年の為に用意)
		return static_cast<AttackType>(arithmetic::RandomIntInRange(0, 1));
	}

	EnemyBossComponent::BossBehavior EnemyBossComponent::GetNextBehaviorType() const
	{
		// 空の場合ランダムの選択する
		if(behavior_history.empty())
		{
			return static_cast<BossBehavior>(arithmetic::RandomIntInRange(0, 1));
		}

		const int index = arithmetic::RandomIntInRange(0, static_cast<int>(behavior_history.size() - 1));
		const BossBehavior type = behavior_history.at(index);

		// 同じビヘイビアを避けるため選択された履歴の逆の履歴を返す
		if (type == BossBehavior::Move)
			return BossBehavior::Attack;

		if (type == BossBehavior::Attack)
			return BossBehavior::Move;

		// 基本このパターンには入らない(年の為に用意)
		return static_cast<BossBehavior>(arithmetic::RandomIntInRange(0, 1));
	}

	void EnemyBossComponent::OnEnterAttackRange(ColliderMessageSenderComponent& sender)
	{
		if (sender.GetHitResult().collision_tag != collision::CollisionTag::Player)
			return;

		is_in_attack_range = true;

		// 近距離攻撃の選択
		if (sender.GetCollisionTag() == collision::CollisionTag::Attack_Melee)
		{
			bit_in_atk_range.set(static_cast<int>(AttackType::Atk_Melee));
			//next_attack_type = AttackType::Atk_Melee;
			return;
		}

		// 遠距離攻撃の選択
		if (sender.GetCollisionTag() == collision::CollisionTag::Attack_Long_Range)
		{
			bit_in_atk_range.set(static_cast<int>(AttackType::Atk_Long_Range));
			//next_attack_type = AttackType::Atk_Long_Range;
		}
	}

	void EnemyBossComponent::OnExitAttackRange(ColliderMessageSenderComponent& sender)
	{
		if (sender.GetHitResult().collision_tag != collision::CollisionTag::Player)
			return;

		// 近距離攻撃の選択
		if (sender.GetCollisionTag() == collision::CollisionTag::Attack_Melee)
		{
			bit_in_atk_range.reset(static_cast<int>(AttackType::Atk_Melee));
			//next_attack_type = AttackType::Atk_Melee;
			return;
		}

		// 遠距離攻撃の選択
		if (sender.GetCollisionTag() == collision::CollisionTag::Attack_Long_Range)
		{
			bit_in_atk_range.reset(static_cast<int>(AttackType::Atk_Long_Range));
			//next_attack_type = AttackType::Atk_Long_Range;
		}
	}

	///////////////////////////////////////////////////////////////////
	//						ビヘイビア更新処理						 //
	///////////////////////////////////////////////////////////////////
	void EnemyBossComponent::BehaviorUpdate(const float dt)
	{
		boss_behavior.Update(dt);

		if(is_behavior_completed)
		{
			is_behavior_completed = false;


			//{
			//	boss_behavior.SetState(BossBehavior::Move);
			//	return;
			//}

			if (bit_in_atk_range.test(static_cast<size_t>(AttackType::Atk_Melee)))
			{// 近距離攻撃範囲内に
				next_attack_type = AttackType::Atk_Melee;
				boss_behavior.SetState(BossBehavior::Attack);
			}
			else if(bit_in_atk_range.test(static_cast<size_t>(AttackType::Atk_Long_Range)))
			{
				// 攻撃が行動かの選択
				const BossBehavior type = GetNextBehaviorType();
				boss_behavior.SetState(type);
			}
			else
			{
				boss_behavior.SetState(BossBehavior::Move);
			}

			//if(is_in_attack_range)
			//{
			//	// ヒット先がプレイヤーの場合攻撃Stateに移行する
			//	boss_behavior.SetState(BossBehavior::Attack);
			//	return;
			//}

			//// プレイヤーの攻撃範囲外なので行動Stateに移行する
			//boss_behavior.SetState(BossBehavior::Move);
			////boss_behavior.SetState(BossBehavior::Attack);
		}
	}

	void EnemyBossComponent::BehaviorMoveUpdate(const float dt)
	{
		if(boss_behavior.GetInitialize())
		{
			// 履歴の追加
			AddBehaviorHistory(BossBehavior::Move);

			// 攻撃行動の決定
			if (auto* player = GetPlayer();
				player)
			{// シーン内にプレイヤーが存在する場合
				movement_behavior.SetBehavior(MovementBehavior::Tracking);
			}
			else
			{
				movement_behavior.SetBehavior(MovementBehavior::Idle);
			}
		}

		movement_behavior.Update(dt, movement_behavior.GetIsStart());

		// 実装中の関数が完了した時
		if (is_next_sequence)
		{
			is_next_sequence = false;
			movement_behavior.NextSequence();

			// 最後まで行動が完了した時
			if (movement_behavior.GetIsCompleted())
				is_behavior_completed = true;
		}
	}

	void EnemyBossComponent::BehaviorDamageUpdate(const float dt)
	{
		if (boss_behavior.GetInitialize())
		{
			if(hp <= 0)
			{
				damage_behavior.SetBehavior(DamageBehavior::Death);
			}
			else
			{
				damage_behavior.SetBehavior(DamageBehavior::Damage);
			}
		}

		damage_behavior.Update(dt, damage_behavior.GetIsStart());

		// 実装中の関数が完了した時
		if (is_next_sequence)
		{
			is_next_sequence = false;
			attack_behavior.NextSequence();

			// 最後まで行動が完了した時
			if (attack_behavior.GetIsCompleted())
				is_behavior_completed = true;
		}
	}

	void EnemyBossComponent::BehaviorAttackUpdate(const float dt)
	{
		if (boss_behavior.GetInitialize())
		{
			// 履歴の追加
			AddBehaviorHistory(BossBehavior::Attack);

			// 攻撃行動の決定
			if (auto* player = GetPlayer();
				player)
			{// シーン内にプレイヤーが存在する場合

				// ランダムな攻撃タイプ(近距離)のインデックス番号を取得
				auto GetRandomAtkMeleeBehavior = []()
				{
					return static_cast<AttackBehavior>(arithmetic::RandomIntInRange(static_cast<int>(AttackBehavior::Begin_Atk_Melee) + 1, static_cast<int>(AttackBehavior::End_Atk_Melee) - 1));
				};

				// ランダムな攻撃タイプ(遠距離)のインデックス番号を取得
				auto GetRandomAtkLongBehavior = []()
				{
					return static_cast<AttackBehavior>(arithmetic::RandomIntInRange(static_cast<int>(AttackBehavior::Begin_Atk_Long) + 1, static_cast<int>(AttackBehavior::End_Atk_Long) - 1));
				};

				if(next_attack_type == AttackType::Atk_Melee)
				{// 近距離攻撃
					AddAttackHistory(AttackType::Atk_Melee);
					//uint atk_index = GetRandomAtkIndex(AttackBehavior::Begin_Atk_Melee, AttackBehavior::End_Atk_Melee);
					//attack_behavior.SetBehavior(static_cast<AttackBehavior>(atk_index));
					//attack_behavior.SetBehavior(AttackBehavior::Atk_N1);
					attack_behavior.SetBehavior(GetRandomAtkMeleeBehavior());
				}
				if (next_attack_type == AttackType::Atk_Long_Range)
				{// 遠距離攻撃
					AddAttackHistory(AttackType::Atk_Long_Range);
					//if (arithmetic::RandomIntInRange(0, 1) == 0)
					//{
					//	attack_behavior.SetBehavior(AttackBehavior::Atk_E1);
					//}
					//else
					//{
					//	attack_behavior.SetBehavior(AttackBehavior::Atk_E2);
					//}
					attack_behavior.SetBehavior(GetRandomAtkLongBehavior());
				}
			}
			else
			{
				return;
			}
		}

		// ビヘイビアツリーに登録されている関数の実装
		attack_behavior.Update(dt, attack_behavior.GetIsStart());

		// 実装中の関数が完了した時
		if (is_next_sequence)
		{
			is_next_sequence = false;
			attack_behavior.NextSequence();

			// 最後まで行動が完了した時
			if (attack_behavior.GetIsCompleted())
				is_behavior_completed = true;
		}
	}

	///////////////////////////////////////////////////////////////////
	//							行動								 //
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	//						  Movement								 //
	///////////////////////////////////////////////////////////////////
	void EnemyBossComponent::Idle(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if(is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Idle));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::Tracking(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			tracing_time = 0;
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Run), true);

			// 追跡後の攻撃を決定
			next_attack_type = GetNextAttackType();
		}

		{// 追跡処理
			PlayerComponent* player_comp = GetPlayer();
			if (!player_comp)
				return;
			float distance_to_player{};
			const mapping::rename_type::Entity ent_player = player_comp->GetEntity();
			EnemyBaseComponent::Tracking(player_comp->GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(),
										 { dash_speed,.0f,dash_speed },
										distance_to_player);
		}

		tracing_time += dt;

		if(next_attack_type == AttackType::Atk_Melee &&
		   bit_in_atk_range.test(static_cast<int>(AttackType::Atk_Melee)))
		{
			boss_behavior.SetState(BossBehavior::Attack);
		}
		else if(next_attack_type == AttackType::Atk_Long_Range &&
				bit_in_atk_range.test(static_cast<int>(AttackType::Atk_Long_Range)))
		{
			boss_behavior.SetState(BossBehavior::Attack);
		}

		if (tracing_time < max_tracking_time)
			return;

		//if (model_comp->IsPlayAnimation())
		//	return;

		//if(max_tracking_time )

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::Wandering(float dt, bool is_start)
	{

	}

	///////////////////////////////////////////////////////////////////
	//						  Damage								 //
	///////////////////////////////////////////////////////////////////
	void EnemyBossComponent::Damage(float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Damage));
		}

		if (model_comp->IsPlayAnimation())
			return;


		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::Death(float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Death));
		}
	}

	///////////////////////////////////////////////////////////////////
	//						  Attack								 //
	///////////////////////////////////////////////////////////////////
	void EnemyBossComponent::AttackNormal01(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Normal_01));
		}

		GetKeyframeEvent(AnimationData::Attack_Normal_01).Update(GetRegistry(), GetEntity(), keyframe_event::event_1);
		if (GetKeyframeEvent(AnimationData::Attack_Normal_01).GetKeyEvent(keyframe_event::event_1).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (auto* capsule_collider = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
				capsule_collider)
			{
				if (auto* capsule = capsule_collider->TryGetCapsuleFromName(collision_name::for_attack);
					capsule)
				{
					capsule->is_enable = true;
				}
			}
		}

		if(GetKeyframeEvent(AnimationData::Attack_Normal_01).GetKeyEvent(keyframe_event::event_1).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (auto* capsule_collider = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
				capsule_collider)
			{
				if (auto* capsule = capsule_collider->TryGetCapsuleFromName(collision_name::for_attack);
					capsule)
				{
					capsule->is_enable = false;
				}
			}
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;

		if (auto* capsule_collider = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
			capsule_collider)
		{
			if (auto* capsule = capsule_collider->TryGetCapsuleFromName(collision_name::for_attack);
				capsule)
			{
				capsule->is_enable = false;
			}
		}
	}

	void EnemyBossComponent::AttackNormal02(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Normal_02));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackNormal03(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Normal_03));
		}

		GetKeyframeEvent(AnimationData::Attack_Normal_03).Update(GetRegistry(), GetEntity(), keyframe_event::event_1);
		if (GetKeyframeEvent(AnimationData::Attack_Normal_03).GetKeyEvent(keyframe_event::event_1).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			auto* prefab_loader = locator::Locator::GetAssetManager()->GetLoader<asset::PrefabLoader>();
			mapping::rename_type::Entity spawn_shot_ent{};
			if (prefab_loader->HasPrefab(shot_prefab_id))
			{
				spawn_shot_ent = prefab_loader->Instantiate(GetRegistry(), shot_prefab_id);
			}

			if (model_comp->HasNode(spawn_shot_node_name.c_str()))
			{
				const auto direction = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetModelFront();
				const auto model_world = model_comp->GetNodeWorldPos(spawn_shot_node_name.c_str());
				const auto pos = model_world + (direction * 80);

				//GetRegistry()->GetComponent<TransformComponent>(spawn_shot_ent).SetPosition(model_comp->GetNodeWorldPos(spawn_shot_node_name.c_str()));
				GetRegistry()->GetComponent<TransformComponent>(spawn_shot_ent).SetPosition(pos);
			}
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackNormal04(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Normal_04));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackCombo01(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Combo_01));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackCombo02(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Combo_02));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackCombo03(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Combo_03));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackCombo04(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Combo_04));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackEnergy01(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Energy_01));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackEnergy02(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Energy_02));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

	void EnemyBossComponent::AttackDodgeBack(const float dt, const bool is_start)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());

		if (!model_comp)
		{
			is_next_sequence = true;
			return;
		}

		if (is_start)
		{
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Dodge_Back));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}
} // cumulonimbus::component