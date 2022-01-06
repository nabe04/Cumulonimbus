#include "enemy_boss_component.h"

#include "arithmetic.h"

#include "model_component.h"
#include "player_component.h"
#include "sphere_collision_component.h"
#include "transform_component.h"

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

	void EnemyBossComponent::Start()
	{
		Initialize(GetRegistry(), GetEntity());
	}

	void EnemyBossComponent::CommonUpdate(const float dt)
	{
		BehaviorUpdate(dt);
	}

	void EnemyBossComponent::Load(ecs::Registry* registry)
	{
	}

	void EnemyBossComponent::Initialize(ecs::Registry* registry, const mapping::rename_type::Entity ent)
	{
		EnemyBossComponent& e_boss_comp = registry->GetComponent<EnemyBossComponent>(ent);
		std::function<bool(float)> behavior_func{};
		std::function<void(EnemyBossComponent& ,std::function<bool(const float dt)>)> b_func{};
		//-- �s��(�r�w�C�r�A)�̍X�VStateMachine�̒ǉ� --//
		boss_behavior.AddState(BossBehavior::Move	 , e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorMoveUpdate>());
		boss_behavior.AddState(BossBehavior::Attack, e_boss_comp.GetBehaviorUpdateFunc<&EnemyBossComponent::BehaviorMoveUpdate>());
		boss_behavior.SetState(BossBehavior::Attack);

		//-- �U��BehaviorTree�̒ǉ� --//
		// �ʏ�U�� 1
		attack_behavior.AddSequence(AttackBehavior::Atk_Normal_01, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal01>());
		// �ʏ�U�� 2
		attack_behavior.AddSequence(AttackBehavior::Atk_Normal_02, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal02>());
		// �ʏ�U�� 3
		attack_behavior.AddSequence(AttackBehavior::Atk_Normal_03, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal03>());
		// �ʏ�U�� 4
		attack_behavior.AddSequence(AttackBehavior::Atk_Normal_04, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackNormal04>());
		// �������U�� 1
		attack_behavior.AddSequence(AttackBehavior::Atk_Energy_01, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy01>());
		// �������U�� 2
		attack_behavior.AddSequence(AttackBehavior::Atk_Energy_02, e_boss_comp.GetBehaviorActFunc<&EnemyBossComponent::AttackEnergy02>());
	}

	void EnemyBossComponent::OnAttack(const collision::HitResult& hit_result)
	{
		// �q�b�g��̃^�O��Player�ȊO�Ȃ珈�����΂�
		if (hit_result.collision_tag != collision::CollisionTag::Player)
			return;

		// �ߋ����U������
	}

	int EnemyBossComponent::GetAnimDataIndex(const AnimationData anim_data) const
	{
		return static_cast<int>(anim_data);
	}

	///////////////////////////////////////////////////////////////////
	//						�r�w�C�r�A�X�V����						 //
	///////////////////////////////////////////////////////////////////

	void EnemyBossComponent::BehaviorUpdate(const float dt)
	{
		boss_behavior.Update(dt);

		if(is_behavior_completed)
		{
			is_behavior_completed = false;
			auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
			if (sphere_collision_comp)
				return;

			for(auto& sphere_data : sphere_collision_comp->GetSpheres() | std::views::values)
			{
				for (auto& hit_result : sphere_data.hit_results | std::views::values)
				{
					if (hit_result.collision_tag != collision::CollisionTag::Player)
						continue;

					// �q�b�g�悪�v���C���[�̏ꍇ�U��State�Ɉڍs����
					boss_behavior.SetState(BossBehavior::Attack);
					return;
				}
			}

			// �v���C���[�̍U���͈͊O�Ȃ̂ōs��State�Ɉڍs����
			boss_behavior.SetState(BossBehavior::Move);
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
			// �U���s���̌���
			if (auto* player = GetPlayer();
				player)
			{// �V�[�����Ƀv���C���[�����݂���ꍇ
				const DirectX::SimpleMath::Vector3 self_pos   = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();
				const DirectX::SimpleMath::Vector3 player_pos = player->GetRegistry()->GetComponent<TransformComponent>(player->GetEntity()).GetPosition();
				const float distance_to_player = DirectX::SimpleMath::Vector3{ self_pos - player_pos }.Length();

				if(distance_to_player <= atk_melee_distance)
				{// �ߋ����U��
					uint atk_index = arithmetic::RandomIntInRange(0, 3);
					attack_behavior.SetBehavior(static_cast<AttackBehavior>(atk_index));
				}
				else
				{// �������U��
					if (arithmetic::RandomIntInRange(0, 1) == 0)
					{
						attack_behavior.SetBehavior(AttackBehavior::Atk_Energy_01);
					}
					else
					{
						attack_behavior.SetBehavior(AttackBehavior::Atk_Energy_02);
					}
				}
			}
			else
			{
				return;
			}
		}

		// �r�w�C�r�A�c���[�ɓo�^����Ă���֐��̎���
		attack_behavior.Update(dt, attack_behavior.GetIsStart());

		// �������̊֐�������������
		if (is_next_sequence)
		{
			is_next_sequence = false;
			attack_behavior.NextSequence();

			// �Ō�܂ōs��������������
			if (attack_behavior.GetIsCompleted())
				is_behavior_completed = true;
		}
	}

	///////////////////////////////////////////////////////////////////
	//							�s��								 //
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		if (model_comp->IsPlayAnimation())
			return;

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
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

		// �r�w�C�r�A�����������̂Ŏ��̃r�w�C�r�A�Ɉڍs����
		is_next_sequence = true;
	}
} // cumulonimbus::component