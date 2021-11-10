//#pragma once
//#include "enemy_base_component.h"
//#include "state_machine.h"
//
//namespace cumulonimbus::component
//{
//	class EnemySoldierComponent final : public EnemyBaseComponent
//	{
//	private:
//		/**
//		 * @brief : �G(Soldier)�̏�ԑJ�ڕ\
//		 */
//		enum class SoldierState
//		{
//			Idle,
//			Walk,
//			Tracking,
//			Damage,
//			Death,
//			Attack_01,
//			Attack_02,
//			Attack_03,
//
//			End
//		};
//
//		/**
//		 * @brief : FBX�����A�j���[�V�����f�[�^
//		 */
//		enum class AnimationData
//		{
//			Walk,
//			Run,
//			Attack_03,
//			Attack_01,
//			Attack_02,
//			Death,
//			Death_Loop,
//			Damage,
//			Attack_All,
//			Idle,
//
//			End
//		};
//
//	public:
//		explicit EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
//		explicit EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemySoldierComponent& copy_comp); // for prefab
//		explicit EnemySoldierComponent()  = default; // for cereal
//		~EnemySoldierComponent() override = default;
//
//		void GameUpdate(float dt) override;
//		void RenderImGui()	  override;
//
//		void Load(ecs::Registry* registry) override;
//
//		template<class Archive>
//		void load(Archive&& archive, uint32_t version);
//
//		template<class Archive>
//		void save(Archive&& archive, uint32_t version) const;
//
//	private:
//		// �G(ForestDemon)�̏�ԊǗ��ϐ�
//		StateMachine<SoldierState, void, const float> soldier_state{};
//
//		//-- �p�����[�^ --//
//		// Quaternion::Slerp�̉�]���Ԕ{��
//		float rotation_time_rate{ 1.0f };
//		// ���s�̑���
//		float walk_speed{ 100 };
//		// �ǐՒ��̑���
//		float tracking_speed{ 150 };
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
//		// �˂��U���Ɉڂ鋗��
//		float attack_thrust_distance{ 50 };
//
//		/**
//		 * @brief : enum class(AnimationData)��int�^�ɕϊ�
//		 */
//		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;
//
//		//-- StateMachine�N���X�ŊǗ�����G�̏�Ԋ֐� --//
//		void Idle(float dt);
//		void Walk(float dt);
//		void Tracking(float dt);
//		void Damage(float dt);
//		void Death(float dt);
//		void Attack01(float dt);
//		void Attack02(float dt);
//		void Attack03(float dt);
//	};
//
//} // cumulonimbus::component