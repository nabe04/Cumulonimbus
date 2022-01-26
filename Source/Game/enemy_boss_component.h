#pragma once
#include "enemy_base_component.h"

#include "behavior_tree.h"
#include "state_machine.h"
#include "keyframe_event.h"

#include "damageable_component.h"

namespace cumulonimbus::component
{
	class ColliderMessageSenderComponent;
} // cumulonimbus::component

namespace cumulonimbus::component
{
	class EnemyBossComponent : public EnemyBaseComponent
	{
	private:
		/**
		 * @brief : ��ԑJ�ڕ\
		 */
		enum class BossState
		{
			// �ҋ@
			Idle,
			// �����n
			Walk,
			Run,
			Tracking,
			// �_���[�W�n
			Damage,
			Knock_Down,
			Death,
			// �U���n
			Attack_Normal_01,
			Attack_Normal_02,
			Attack_Normal_03,
			Attack_Normal_04,
			Attack_Energy_01,
			Attack_Energy_02,
			Attack_Dodge_Back,
			Attack_Combo_01,
			Attack_Combo_02,
			Attack_Combo_03,
			Attack_Combo_04,

			End
		};

		/**
		 * @brief : �S�̂̍s��(�r�w�C�r�A)�\
		 */
		enum class BossBehavior
		{
			Move, // �ҋ@�A�ǐՁA�p�j
			Attack, // �ʏ�U���A�G�l���M�[�U���A�o�b�N�W�����v�U��
			Damage, // �_���[�W�A�m�b�N�_�E���A���S

			End
		};

		/**
		 * @brief : �U���̎��
		 */
		enum class AttackType
		{
			Atk_Melee = 0,
			Atk_Long_Range,

			End
		};

		/**
		 * @brief : HP�ɉ����Ă̍s���p�^�[��
		 */
		enum class AttackPattern
		{
			Pattern_1 = 0,
			Pattern_2,
			Pattern_3,
			Pattern_4,

			End
		};

		/**
		 * @brief : �ړ��̍s��(�r�w�C�r�A)�\
		 */
		enum class MovementBehavior
		{
			Idle,
			Tracking,
			Wandering,

			End
		};

		/**
		 * @brief : �_���[�W�̍s��(�r�w�C�r�A)�\
		 */
		enum class DamageBehavior
		{
			Damage,
			Death,

			End
		};

		/**
		 * @brief :�U���̍s��(�r�w�C�r�A)�\
		 */
		enum class AttackBehavior
		{
			// �ߋ����U��
			// HP 75 �` 100(%)�ł̍s��
			Begin_Atk_Melee_1,
			Atk_N1,
			Atk_N2,
			Atk_N4,
			End_Atk_Melee_1,

			// HP 50 �` 74(%)�ł̍s��
			Begin_Atk_Melee_2,
			Atk_N1_N2,
			Atk_N1_N3,
			Atk_N1_N4,
			Atk_N2_N1,
			Atk_N2_N3,
			Atk_N2_N4,
			End_Atk_Melee_2,

			// HP 25 �` 49(%)�ł̍s��
			Begin_Atk_Melee_3,
			End_Atk_Melee_3,

			// HP 1 ~ 24(%)�ł̍s��
			Begin_Atk_Melee_4,
			End_Atk_Melee_4,

			// �������U��
			// HP 75 �` 100(%)�ł̍s��
			Begin_Atk_Long_1,
			Atk_N3,
			Atk_E2,
			End_Atk_Long_1,

			// HP 50 �` 74(%)�ł̍s��
			Begin_Atk_Long_2,
			Atk_N3_E2,
			Atk_E2_N3,
			End_Atk_Long_2,

			// HP 25 �` 49(%)�ł̍s��
			Begin_Atk_Long_3,
			End_Atk_Long_3,

			// HP 1 ~ 24(%)�ł̍s��
			Begin_Atk_Long_4,
			End_Atk_Long_4,

			End
		};

		/**
		 * @brief : FBX�����A�j���[�V�����f�[�^
		 */
		enum class AnimationData
		{
			Damage,
			Walk,
			Run,
			Attack_Combo_All,
			Attack_Normal_01,
			Attack_Normal_02,
			Attack_Normal_03,
			Attack_Normal_04,
			Attack_Dodge_Back,
			Attack_Energy_01,
			Attack_Energy_02,
			Damage_Knock_Down,
			Attack_Combo_01,
			Attack_Combo_02,
			Attack_Combo_03,
			Attack_Combo_04,
			Death,
			Death_Loop,
			Idle,

			End
		};

	public:
		explicit EnemyBossComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemyBossComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemyBossComponent& copy_comp);
		explicit EnemyBossComponent(mapping::component_tag::ComponentTag tag);
		explicit EnemyBossComponent()  = default;
		~EnemyBossComponent() override = default;

		void Start() override;
		void CommonUpdate(float dt) override;
		void GameUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

	private:
		// �s��(�r�w�C�r�A)�̍X�VStateMachine
		StateMachine<BossBehavior, void, const float> boss_behavior{};
		// �����n�r�w�C�r�A
		system::BehaviorTree<MovementBehavior, void, const float, const bool> movement_behavior{};
		// �_���[�W�n�r�w�C�r�A
		system::BehaviorTree<DamageBehavior, void, const float, const bool> damage_behavior{};
		// �U���n�r�w�C�r�A
		system::BehaviorTree<AttackBehavior, void, const float, const bool> attack_behavior{};
		// �L�[�t���[�����̃C�x���g�Ǘ�
		std::map<AnimationData, system::KeyframeEvent> keyframe_events{};
		//-- �p�����[�^ --//
		// ���s�̑���
		float walk_speed{ 100.f };
		// ����̑���
		float dash_speed{ 150.f };
		// �m�b�N�o�b�N���x��
		KnockBackLevel knock_back_level{};
		// �m�b�N�o�b�N�̑���
		float knock_back_speed{ 10.f };
		//-- �s������ --//
		// �ߋ����U���̋��E����
		float atk_melee_distance{ 100.f };
		// �ǐՎ���
		float tracing_time{};
		// �ő�ǐՎ���
		float max_tracking_time{ 10.f };

		// �U���͈͓���
		bool is_in_attack_range{ false };
		// ���̃r�w�C�r�A�Ɉڍs���邩
		bool is_next_sequence{ false };
		// �r�w�C�r�A�c���[�̈�A�̏���������������
		bool is_behavior_completed{ false };

		// �����蔻��p�v���n�u
		mapping::rename_type::UUID atk_collider_prefab_id{};
		// �G�l���M�[�V���b�g�̃v���n�u
		mapping::rename_type::UUID shot_prefab_id{};
		// �G�l���M�[�V���b�g�������̈ʒu
		std::string spawn_shot_node_name{};

		// ���r�w�C�r�A�ł̍U����
		AttackType next_attack_type{};
		// �U���͈͓��ɑ��݂��邩�̃t���O
		std::bitset<static_cast<int>(AttackType::End)> bit_in_atk_range{};
		// �ő嗚��(�����l : 10)
		u_int max_history_count{ 10 };
		// �U����ނ̗���(���̃��X�g�����ɜp�j���̎��̍U�������߂�)
		std::vector<AttackType> attack_history{};
		// �r�w�C�r�A(Move,Attack)�̗���
		std::vector<BossBehavior> behavior_history{};

		//-- �q�b�g�G�t�F�N�g�n --//
		// ���݂̃q�b�g�G�t�F�N�gID
		mapping::rename_type::UUID current_hit_effect_id{};
		// �q�b�g�G�t�F�N�g(S) ID
		mapping::rename_type::UUID hit_effect_s{};
		// �q�b�g�G�t�F�N�g(M) ID
		mapping::rename_type::UUID hit_effect_m{};
		// �q�b�g�G�t�F�N�g(L) ID
		mapping::rename_type::UUID hit_effect_l{};

		//-- ������ --//
		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;
		/**
		 * @brief : �����̏�����
		 * @remark : �U���A�r�w�C�r�A
		 */
		void InitializeHistory();


		void OnAttack(const collision::HitResult& hit_result) override;
		void OnDamaged(const DamageData& damage_data, const collision::HitResult& hit_result) override;

		/**
		 * @brief : �L�[�t���[���C�x���g�̓o�^
		 */
		void RegistryKeyframeEvent(AnimationData anim_data, const std::string& key_name);
		/**
		 * @brief : �U�����(attack_history)�ɗ����̒ǉ�
		 */
		void AddAttackHistory(AttackType type);
		/**
		 * @brief : �r�w�C�r�A(behavior_history)�ɗ����̒ǉ�
		 */
		void AddBehaviorHistory(BossBehavior type);

		template <auto F>
		[[nodiscard]]
		std::function<void(float)> GetBehaviorUpdateFunc();
		template<auto F>
		[[nodiscard]]
		std::function<void(float, bool)> GetBehaviorActFunc();
		[[nodiscard]]
		int GetAnimDataIndex(AnimationData anim_data) const;
		[[nodiscard]]
		system::KeyframeEvent& GetKeyframeEvent(AnimationData anim_data);
		/**
		 * @brief : ���̍U����ނ̎擾
		 */
		[[nodiscard]]
		AttackType GetNextAttackType() const;
		/**
		 * @brief : ���̃r�w�C�r�A��ނ̎擾
		 */
		[[nodiscard]]
		BossBehavior GetNextBehaviorType() const;

		/**
		 * @brief : HP�ɉ����Ă̍s���p�^�[�����擾
		 */
		[[nodiscard]]
		AttackPattern GetAttackPattern() const;

		void OnEnterAttackRange(ColliderMessageSenderComponent & sender);
		void OnExitAttackRange(ColliderMessageSenderComponent& sender);

		//-- �r�w�C�r�A�X�V���� --//
		/**
		 * @return : true -> �X�V���̃r�w�C�r�A���I��
		 */
		void BehaviorUpdate(float dt);
		void BehaviorMoveUpdate(float dt);
		void BehaviorDamageUpdate(float dt);
		void BehaviorAttackUpdate(float dt);

		//-- �G(�{�X)�̋��� --//
		/**
		 * @return : true -> ���̃r�w�C�r�A�Ɉڍs
		 */
		// �ҋ@
		void Idle(float dt, bool is_start);
		// �����n
		void Tracking(float dt, bool is_start);
		void Wandering(float dt, bool is_start);
		// �_���[�W�n
		void Damage(float dt, bool is_start);
		void Death(float dt, bool is_start);
		// �ʏ�U��(�P��)
		void AttackNormal01(float dt, bool is_start);
		void AttackNormal02(float dt, bool is_start);
		void AttackNormal03(float dt, bool is_start);
		void AttackNormal04(float dt, bool is_start);
		// �R���{�U��
		void AttackCombo01(float dt, bool is_start);
		void AttackCombo02(float dt, bool is_start);
		void AttackCombo03(float dt, bool is_start);
		void AttackCombo04(float dt, bool is_start);
		// �G�l���M�[�U��
		void AttackEnergy01(float dt, bool is_start);
		void AttackEnergy02(float dt, bool is_start);
		// �o�b�N�W�����v�U��
		void AttackDodgeBack(float dt, bool is_start);
	};
} // cumulonimbus::component
