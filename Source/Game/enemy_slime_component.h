//#pragma once
//#include <unordered_map>
//
//#include "enemy_base_component.h"
//#include "state_machine.h"
//
//namespace cumulonimbus::component
//{
//	class EnemySlimeComponent final : public EnemyBaseComponent
//	{
//	private:
//		/**
//		 * @brief : �G(Slime)�̏�ԑJ�ڕ\
//		 */
//		enum class SlimeState
//		{
//			T_Pose,
//			Idle,			// �ҋ@
//			Walk,			// ����
//			Tracking,		// �ǐ�
//			Birth,			// ����
//			Death,			// ���S
//			Damage,			// ��e
//			Stun,			// �X�^��
//			Attack_Bite,	// �U��(���݂�)
//			Attack_Charge,	// �U��(�ːi)
//
//			End
//		};
//
//		/**
//		 * @brief : FBX�����A�j���[�V�����f�[�^
//		 */
//		enum class AnimationData
//		{
//			Idle_00,
//			Attack_Bite,
//			Attack_Charge,
//			Birth,
//			Death,
//			Damage,
//			Idle_01,
//			Stun,
//			Walk,
//
//			End
//		};
//
//	public:
//		explicit EnemySlimeComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
//		explicit EnemySlimeComponent()  = default; // for cereal
//		~EnemySlimeComponent() override = default;
//
//		void GameUpdate(float dt) override;
//		void RenderImGui()	  override;
//
//		void Load(ecs::Registry* registry) override;
//
//	private:
//		// �G(Slime)�̏�ԊǗ��ϐ�
//		StateMachine<SlimeState, void, const float> slime_state{};
//
//		//-- �p�����[�^ --//
//		// Quaternion::Slerp�̉�]���Ԕ{��
//		float rotation_time_rate{ 1.0f };
//		// ���s�̑���
//		float walk_speed{ 100 };
//		// �ǐՒ��f�̒���
//		float tracking_interruption_distance{ 200 };
//		// �ǐՂ���U���Ɉڂ鋗��
//		float attack_transition_distance{ 100 };
//		// �ǐՂ���U���Ɉڂ�p�x(�x���@)
//		float attack_transition_angle{ 90 };
//		// �ǐՂɈڂ鋗��
//		float tracking_transition_distance{ 200 };
//		// �ǐՂɈڂ�p�x()
//		float tracking_transition_angle{ 90 };
//
//		//-- ���̑� --//
//
//		/**
//		 * @brief : enum class(AnimationData)��int�^�ɕϊ�
//		 */
//		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;
//
//		//-- StateMachine�N���X�ŊǗ�����G�̏�Ԋ֐� --//
//		void TPose(float dt);
//		void Idle(float dt);
//		void Walk(float dt);
//		void Tracking(float dt);
//		void Birth(float dt);
//		void Death(float dt);
//		void Damage(float dt);
//		void Stun(float dt);
//		void AttackBite(float dt);
//		void AttackCharge(float dt);
//	};
//} // cumulonimbus::component
