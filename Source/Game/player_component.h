#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "actor3d_component.h"
#include "state_machine.h"
#include "enum_state_map.h"

namespace cumulonimbus::component
{
	class PlayerComponent final : public Actor3DComponent
	{
	private:
		// �v���C���[�̏�Ԃ�\��
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
			Attacking_Normal_01,		    // �ʏ�U��(��)
			Attacking_Normal_02,
			Attacking_Normal_03,
			Attack_Normal_01_End,
			Attack_Normal_02_End,
			Attack_Normal_04_Begin,
			Attacking_Normal_04,
			Attack_Normal_04_End,
			Attacking_Normal_Long_Press,	// �ʏ�U��(�㒷����)
			Attack_Normal_Long_Press_End,
			Attacking_Strong_01,			// �ʏ�U��(��)
			Attacking_Strong_02,
			Attacking_Strong_03,
			Attacking_Strong_04,
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
		enum class AnimationState
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
			Jump_Loop,
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
			Jump_End,
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

			End
		};

	public:
		explicit PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent);
		explicit PlayerComponent()  = default; // for cereal
		~PlayerComponent() override = default;

		void NewFrame(float dt) override;
		void Update(float dt)	override;
		void RenderImGui()		override;

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;
	private:
		// �v���C���[�̏�ԕϐ�
		StateMachine<PlayerState, void, const float> player_state{};
		// ��s����(���͂Ȃ�(���Z�b�g) == PlayerState::End)
		PlayerState precede_input = PlayerState::End;

		// �����̑���
		float walk_speed = 20;
		// ����̑���
		float dash_speed = 1;

		// �p�b�h���͂̃f�b�h�]�[���l
		float threshold				= 0.05f;
		// �p�b�h����������
		float long_press_time		= 0.0f;
		// �p�b�h�������X���b�g��(����������Ɏg�p)
		int   long_press_slot		= 60;

		/**
		 * @brief : State�ύX���ɃA�j���[�V�����p�̃����o�ϐ��̏�����
		 */
		void InitializeAnimationVariable(PlayerState state = PlayerState::End, float long_press_time = 0.0f)
		{
			precede_input			= state;
			this->long_press_time	= long_press_time;
		}

		/**
		 * @brief : enum class(AnimationState)��int�^�ɕϊ�
		 */
		[[nodiscard]] int GetAnimStateIndex(AnimationState anim_state) const;

		/**
		 * @brief : ���C�L���X�g�Ŏg�p����p�����[�^�̃Z�b�g
		 */
		void SetRayCastParameter(float dt);

		/**
		 * @brief : �ړ��Ǘ��֐�
		 */
		void Movement(float dt);

		/**
		 * @brief : �J�������[�N�Ǘ��֐�
		 */
		void CameraWork();

		/**
		 * @brief : ���̃A�j���[�V������PlayerState::Attacking_Normal_Long_Press
		 *			�ɑJ�ڂ��ėǂ����̔���֐��B
		 *			long_press_time�̎��ԂŔ��f
		 */
		[[nodiscard]] bool IsNextAnimationLongPressAttack() const;


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
		void AttackNormal01End(float dt);
		void AttackNormal02End(float dt);
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
		void AttackingJumpStrong(float dt);		// �W�����v�U��(��)
		void AttackJumpStrongEnd(float dt);
		void DashAttack(float dt);				// �_�b�V���U��
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::PlayerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::PlayerComponent)