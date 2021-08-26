#pragma once
#include "enemy_base_component.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class EnemyForestDemonComponent final : public EnemyBaseComponent
	{
	private:
		/**
		 * @brief : �G(Forest Demon)�̏�ԑJ�ڕ\
		 */
		enum class ForestDemonState
		{
			T_Pose,
			Idle,				// �ҋ@(�ʏ�)
			Ready,				// �ҋ@(�퓬�O)
			Walk,				// ����
			Tracking,			// �ǐ�
			Death,				// ���S
			Damage,				// ��e
			Attack_Normal,		// �U��(�ʏ�)
			Attack_Mow_Down,	// �U��(�ガ����)

			End
		};

		/**
		 * @brief : FBX�����A�j���[�V�����f�[�^
		 */
		enum class AnimationData
		{
			Attack_Mow_Down,
			Attack_Normal,
			Death,
			Damage,
			Idle_00,
			Idle_01,
			Ready,
			Run,
			Walk,

			End
		};

	public:
		explicit EnemyForestDemonComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemyForestDemonComponent()  = default; // for cereal
		~EnemyForestDemonComponent() override = default;

		void Update(float dt) override;
		void RenderImGui()    override;

		void Save(const std::string& file_path)			 override;
		void Load(const std::string& file_path_and_name) override;

	private:
		// �G(ForestDemon)�̏�ԊǗ��ϐ�
		StateMachine<ForestDemonState, void, const float> forest_demon_state{};

		//-- �p�����[�^ --//
		// Quaternion::Slerp�̉�]���Ԕ{��
		float rotation_time_rate{ 1.0f };
		// ���s�̑���
		float walk_speed{ 100 };
		// �ǐՒ��̑���
		float tracking_speed{ 150 };
		// �ǐՒ��f�̒���
		float tracking_interruption_distance{ 200 };
		// �ǐՂ���U���Ɉڂ鋗��
		float attack_transition_distance{ 100 };
		// �ǐՂ���U���Ɉڂ�p�x(�x���@)
		float attack_transition_angle{ 90 };
		// �ǐՂɈڂ鋗��
		float tracking_transition_distance{ 200 };
		// �ǐՂɈڂ�p�x()
		float tracking_transition_angle{ 90 };

		//-- ���̑� --//


		/**
		 * @brief : enum class(AnimationData)��int�^�ɕϊ�
		 */
		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;

		//-- StateMachine�N���X�ŊǗ�����G�̏�Ԋ֐� --//
		void TPose(float dt);
		void Idle(float dt);
		void Ready(float dt);
		void Walk(float dt);
		void Tracking(float dt);
		void Death(float dt);
		void Damage(float dt);
		void AttackNormal(float dt);
		void AttackMowDown(float dt);
	};
} // cumulonimbus::component
