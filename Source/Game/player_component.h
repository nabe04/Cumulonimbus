#pragma once
#include <map>
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "actor3d_component.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class PlayerComponent final : public Actor3DComponent
	{
	private:
		// �v���C���[�̏�ԑJ�ڕ\
		enum class PlayerState
		{
			T_Pose,
			Idle,							// �ҋ@
			Walk_Front,						// ����
			Walk_Back,
			Damage_Small,					// �_���[�W
			Damage_Big,
			Jump_Begin,						// �W�����v
			Jump_Loop,
			Jump_Landing,
			Jump_End,
			Knock_Down_Front_Loop,			// �m�b�N�o�b�N
			Knock_Down_Front_Stand_Up,
			Die,							// ���S
			Attack_Normal_01,				// �ʏ�U��(��)
			Attack_Normal_02,
			Attack_Normal_03,
			Attack_Normal_04_Begin,
			Attacking_Normal_04,
			Attack_Normal_04_End,
			Attacking_Normal_Long_Press,	// �ʏ�U��(�㒷����)
			Attack_Normal_Long_Press_End,
			Attack_Strong_01,				// �ʏ�U��(��)
			Attack_Strong_02,
			Attack_Strong_03,
			Attack_Strong_04,
			Attack_Round_Up_Begin,			// �؂�グ�U��
			Attacking_Round_Up,
			Attack_Round_Up_Fall,
			Attack_Round_Up_End,
			Attacking_Jump_01,				// �W�����v�U��(��)
			Attacking_Jump_02,
			Attacking_Jump_03,
			Attacking_Jump_04,
			Attack_Jump_01_End,
			Attack_Jump_02_End,
			Attack_Jump_03_End,
			Attack_Jump_04_End,
			Attack_Jumping_Strong_Begin,	// �W�����v�U��(��)
			Attacking_Jump_Strong,
			Attack_Jump_Strong_End,
			Avoid_Dash_Begin,				// �_�b�V�����
			Avoid_Dash_End,
			Dash,
			Dash_Attack,					// �_�b�V���U��

			End
		};

		// FBX�����A�j���[�V�����f�[�^
		enum class AnimationData
		{
			T_Pose,
			Attacking_Normal_01,
			Attacking_Normal_02,
			Attacking_Normal_03,
			Attack_Normal_04_Begin,
			Attacking_Normal_04,
			Attack_Normal_04_End,
			Attacking_Normal_Long_Press,
			Attacking_Strong_02,
			Attacking_Strong_03,
			Attacking_Strong_04,
			Attack_Round_Up_Begin,
			Attacking_Round_Up,
			Attack_Round_Up_Fall,
			Attack_Round_Up_End,
			Attacking_Jump_01,
			Attacking_Jump_02,
			Attacking_Jump_03,
			Attacking_Jump_04,
			Attack_Jumping_Strong_Begin,
			Attacking_Jump_Strong,
			Attack_Jump_Strong_End,
			Jump_Begin,
			Jump_Landing,
			Attacking_Strong_01,
			Avoid_Dash_Begin,
			Avoid_Dash_End,
			Dash,
			Dash_Attack,
			Walk_Back,
			Walk_Front,
			Idle,
			Damage_Small,
			Damage_Big,
			Knock_Down_Front_Loop,
			Knock_Down_Front_Stand_Up,
			Die,
			Die_Loop,
			Attack_Normal_01_End,
			Attack_Normal_02_End,
			Attack_Normal_Long_Press_End,
			Attack_Jump_01_End,
			Attack_Jump_02_End,
			Attack_Jump_03_End,
			Attack_Jump_04_End,
			Attack_Normal_01,
			Attack_Normal_02,
			Attack_Normal_03,
			Attack_Normal_All,
			Attack_Strong_01,
			Attack_Strong_02,
			Attack_Strong_03,
			Attack_Strong_04,
			Attack_Strong_All,
			Jump_Loop,
			Jump_End,
			Attack_Jump_Strong_All,
			Attack_Normal_04_All,

			End
		};
	public:
		explicit PlayerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PlayerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const PlayerComponent& copy_comp); // for prefab
		explicit PlayerComponent() = default; // for cereal
		~PlayerComponent() override = default;

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;
		void PreGameUpdate(float dt)		override;
		void GameUpdate(float dt)			override;
		void RenderImGui()					override;
		void Load(ecs::Registry* registry)	override;

		//template<class Archive>
		//void serialize(Archive&& archive);

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;
	private:
		// �v���C���[�̏�ԊǗ��ϐ�
		StateMachine<PlayerState, void, const float> player_state{};
		// ��s����(���͂Ȃ�(���Z�b�g) == PlayerState::End)
		PlayerState precede_input{ PlayerState::End };
		// �A�j���[�V�����L�[�t���[���̒����p(�I�[�t���[���̒���)
		std::map<std::string, u_int> adjust_keyframe_map{};
		// �A�j���[�V�������f�t���[��(��s���͂ȂǂŎg�p)
		std::map<AnimationData, u_int> animation_break_frame{};

		//-- ��Ԃɉ����ẴX�s�[�h�ݒ� --//
		float walk_speed{ 300 };	 // �����̑���
		float dash_speed{ 700 };	 // ����̑���
		float attack_04_speed{ 700 }; // �ʏ�U��04(�W�����v�U��)���̑���
		float avoid_dash_speed{ 900 };  // ����_�b�V�����x
		float jump_movement_speed{ 300 };

		// �p�b�h���͂̃f�b�h�]�[���l
		float threshold{ 0.05f };
		// �p�b�h����������
		float long_press_time{ 0.0f };
		// �p�b�h�������X���b�g��(����������Ɏg�p)
		int   long_press_slot{ 60 };

		// �ʏ�U��04(�W�����v�Ŏa���)���̃W�����v���x
		float attack_04_jump_strength{ 500 };

		/**
		 * @brief : State�ύX���ɃA�j���[�V�����p�̃����o�ϐ��̏�����
		 */
		void InitializeAnimationVariable(PlayerState state = PlayerState::End, float long_press_time = 0.0f)
		{
			precede_input = state;
			this->long_press_time = long_press_time;
		}

		/**
		 * @brief : enum class(AnimationState)��int�^�ɕϊ�
		 */
		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_state) const;

		/**
		 * @brief : ���C�L���X�g�Ŏg�p����p�����[�^�̃Z�b�g
		 */
		void SetRayCastParameter(float dt);

		/**
		 * @brief : �ړ��Ǘ��֐�
		 */
		void Movement(float dt);

		/**
		 * @brief : �����Ǘ��֐�
		 */
		void Rotation(float dt);

		/**
		 * @brief	: �����蔻�肪�s���Ă����̏���
		 * @details	: �����蔻��̏������̂�CollisionManager�ōs��
		 *			  �������s��ꂽ��̓����̏���������
		 */
		void Collision() const;

		/**
		 * @brief : �J�������[�N�Ǘ��֐�
		 */
		void CameraWork();

		/**
		 * @brief					: �A�j���[�V�����L�[�t���[�������l�̐ݒ�
		 * @param animation_name	: ���f���̃A�j���[�V������
		 * @param keyframe			: ������̃��f���̏I�[�L�[�t���[��
		 * @attention				: "animation_name"�����f���̃A�j���[�V�������ɑ��݂��Ȃ��ꍇ��O���o��
		 */
		void SetAdjustKeyFrame(const std::string& animation_name, u_int keyframe);

		/**
		 * @brief					: ���A�j���[�V�����ڍs�t���[���̐ݒ�
		 * @param state				: �t���[����ݒ肵����AnimationState
		 * @param keyframe			: �ڍs�t���[���̎w��
		 */
		void SetAnimationBreakFrame(AnimationData state, u_int keyframe);

		/**
		 * @brief : ���̃A�j���[�V������PlayerState::Attacking_Normal_Long_Press
		 *			�ɑJ�ڂ��ėǂ����̔���֐��B
		 *			long_press_time�̎��ԂŔ��f
		 */
		[[nodiscard]]
		bool IsNextAnimationLongPressAttack() const;

		/**
		 * @brief  : ���͂��f�b�h�]�[�����ɂ��邩�ǂ�������
		 * @return : true : �f�b�h�]�[�����ɂ���
		 */
		[[nodiscard]]
		bool IsDeadZone() const;

		/**
		 * @brief			: ���A�j���[�V�����L�[�t���[����"animation_break_frame"�Őݒ肵���t���[���𒴂��Ă��邩
		 * @param state		: "animation_break_frame"�ɓo�^����Ă���AnimationState
		 * @return			: true -> �����Ă���
		 */
		[[nodiscard]]
		bool IsBreakAnimationFrame(AnimationData state) const;

		/**
		 * @brief : StateMachine�̏�����
		 * @param registry :
		 * @param ent : �����̃G���e�B�e�B
		 */
		void InitializeMoveState(ecs::Registry* registry, const mapping::rename_type::Entity& ent);
		/**
		 * @brief : StateMachine�N���X�ŊǗ�����v���C���[�̏�Ԋ֐�
		 */
		void TPose(float dt);					// T-�|�[�Y
		void Idle(float dt);					// �ҋ@
		void WalkFront(float dt);				// ����
		void WalkBack(float dt);
		void AvoidDashBegin(float dt);			// �_�b�V�����
		void AvoidDashEnd(float dt);
		void Dash(float dt);					// �_�b�V��
		void DamageSmall(float dt);				// �_���[�W
		void DamageBig(float dt);
		void KnockDownFrontLoop(float dt);		// �m�b�N�_�E��
		void KnockDownFrontStandUp(float dt);
		void JumpBegin(float dt);				// �W�����v
		void JumpLoop(float dt);
		void JumpLanding(float dt);
		void JumpEnd(float dt);
		void Die(float dt);						// ���S
		void AttackingNormal01(float dt);		// �ʏ�U��(��)
		void AttackingNormal02(float dt);
		void AttackingNormal03(float dt);
		void AttackingNormal04(float dt);
		void AttackNormal04Begin(float dt);
		void AttackNormal04End(float dt);
		void AttackingNormalLongPress(float dt); // �ʏ�U��(�㒷����)
		void AttackNormalLongPressEnd(float dt);
		void AttackingStrong01(float dt);		 // �ʏ�U��(��)
		void AttackingStrong02(float dt);
		void AttackingStrong03(float dt);
		void AttackRoundUpBegin(float dt);		 // �؂�グ�U��
		void AttackingStrong04(float dt);
		void AttackingRoundUp(float dt);
		void AttackRoundUpFall(float dt);
		void AttackRoundUpEnd(float dt);
		void AttackingJump01(float dt);			 // �W�����v�U��(��)
		void AttackingJump02(float dt);
		void AttackingJump03(float dt);
		void AttackingJump04(float dt);
		void AttackJump01End(float dt);
		void AttackJump02End(float dt);
		void AttackJump03End(float dt);
		void AttackJump04End(float dt);
		void AttackJumpStrongBegin(float dt);	// �W�����v�U��(��)
		void AttackingJumpStrong(float dt);
		void AttackJumpStrongEnd(float dt);
		void DashAttack(float dt);				// �_�b�V���U��
	};
} // cumulonimbus::component