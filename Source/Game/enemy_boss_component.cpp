#include "enemy_boss_component.h"

#include "arithmetic.h"

#include "locator.h"
#include "model_component.h"
#include "player_component.h"
#include "sphere_collision_component.h"
#include "transform_component.h"
#include "collider_message_receiver_component.h"
#include "collider_message_sender_component.h"
#include "prefab_loader.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::EnemyBossComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::EnemyBaseComponent, cumulonimbus::component::EnemyBossComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::EnemyBossComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void EnemyBossComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<EnemyBaseComponent>(this)
		);
	}

	template <class Archive>
	void EnemyBossComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<EnemyBaseComponent>(this)
		);
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
		EnemyBossComponent& e_boss_comp = GetRegistry()->GetComponent<EnemyBossComponent>(GetEntity());

		//
		if (auto* collider_receiver = GetRegistry()->TryGetComponent<ColliderMessageReceiverComponent>(GetEntity());
			collider_receiver)
		{
			collider_receiver->RegisterReceivedMessage(GetEntity(), [&e_boss_comp](ColliderMessageSenderComponent& sender) {e_boss_comp.AttackTypeSelection(sender); });
		}

		//-- 行動(ビヘイビア)の更新StateMachineの追加 --//
		boss_behavior.AddState(BossBehavior::Move  , e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorMoveUpdate>());
		boss_behavior.AddState(BossBehavior::Attack, e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorAttackUpdate>());
		boss_behavior.SetState(BossBehavior::Attack);

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
		// 遠距離攻撃 1
		attack_behavior.AddSequence(AttackBehavior::Atk_E1, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy01>());
		// 遠距離攻撃 2
		attack_behavior.AddSequence(AttackBehavior::Atk_E2, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy02>());
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

	void EnemyBossComponent::OnAttack(const collision::HitResult& hit_result)
	{
		// ヒット先のタグがPlayer以外なら処理を飛ばす
		if (hit_result.collision_tag != collision::CollisionTag::Player)
			return;

		// 近距離攻撃処理
	}

	int EnemyBossComponent::GetAnimDataIndex(const AnimationData anim_data) const
	{
		return static_cast<int>(anim_data);
	}

	///////////////////////////////////////////////////////////////////
	//						ビヘイビア更新処理						 //
	///////////////////////////////////////////////////////////////////

	void EnemyBossComponent::AttackTypeSelection(ColliderMessageSenderComponent& sender)
	{
		is_in_attack_range = true;

		// 近距離攻撃の選択
		if(sender.GetCollisionTag() == collision::CollisionTag::Attack_Melee)
		{
			next_attack_type = AttackType::Atk_Melee;
			return;
		}

		// 遠距離攻撃の選択
		if(sender.GetCollisionTag() == collision::CollisionTag::Attack_Long_Range)
		{
			next_attack_type = AttackType::Atk_Long_Range;
		}
	}

	void EnemyBossComponent::BehaviorUpdate(const float dt)
	{
		boss_behavior.Update(dt);

		if(is_behavior_completed)
		{
			is_behavior_completed = false;

			if(is_in_attack_range)
			{
				// ヒット先がプレイヤーの場合攻撃Stateに移行する
				boss_behavior.SetState(BossBehavior::Attack);
				return;
			}

			// プレイヤーの攻撃範囲外なので行動Stateに移行する
			//boss_behavior.SetState(BossBehavior::Move);
			boss_behavior.SetState(BossBehavior::Attack);
		}
	}

	void EnemyBossComponent::BehaviorMoveUpdate(const float dt)
	{
		if(boss_behavior.GetInitialize())
		{
			//
		}
	}

	void EnemyBossComponent::BehaviorAttackUpdate(const float dt)
	{
		if (boss_behavior.GetInitialize())
		{
			// 攻撃行動の決定
			if (auto* player = GetPlayer();
				player)
			{// シーン内にプレイヤーが存在する場合
				//const DirectX::SimpleMath::Vector3 self_pos   = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();
				//const DirectX::SimpleMath::Vector3 player_pos = player->GetRegistry()->GetComponent<TransformComponent>(player->GetEntity()).GetPosition();
				//const float distance_to_player = DirectX::SimpleMath::Vector3{ self_pos - player_pos }.Length();

				if(next_attack_type == AttackType::Atk_Melee)
				{// 近距離攻撃
					uint atk_index = arithmetic::RandomIntInRange(0, 3);
					//attack_behavior.SetBehavior(static_cast<AttackBehavior>(atk_index));
					attack_behavior.SetBehavior(AttackBehavior::Atk_N3);
				}
				if (next_attack_type == AttackType::Atk_Long_Range)
				{// 遠距離攻撃
					if (arithmetic::RandomIntInRange(0, 1) == 0)
					{
						attack_behavior.SetBehavior(AttackBehavior::Atk_E1);
					}
					else
					{
						attack_behavior.SetBehavior(AttackBehavior::Atk_E2);
					}
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
			model_comp->SwitchAnimation(GetAnimDataIndex(AnimationData::Run));
		}

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
	}

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

		if (model_comp->IsPlayAnimation())
			return;

		// ビヘイビアが完了したので次のビヘイビアに移行する
		is_next_sequence = true;
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

			auto* prefab_loader = locator::Locator::GetAssetManager()->GetLoader<asset::PrefabLoader>();
			if(prefab_loader->HasPrefab(shot_prefab_id))
			{
				prefab_loader->Instantiate(GetRegistry(), shot_prefab_id);
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