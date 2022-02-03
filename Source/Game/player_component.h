#pragma once
#include <map>
#include <string>

#include <d3d11.h>

#include "state_machine.h"
#include "keyframe_event.h"
// components
#include "actor3d_component.h"
#include "collision_component.h"
#include "damageable_component.h"

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
			Damage_Small,					// �_���[�W
			Damage_Big,
			Knock_Down_Front_Loop,			// �m�b�N�o�b�N
			Knock_Down_Front_Stand_Up,
			Die,							// ���S
			Attack_Normal_01,				// �ʏ�U��(��)
			Attack_Normal_02,
			Attack_Normal_03,
			Attacking_Normal_Long_Press,	// �ʏ�U��(�㒷����)
			Attack_Normal_Long_Press_End,
			Attack_Strong_01,				// �ʏ�U��(��)
			Attack_Strong_02,
			Attack_Strong_03,
			Attack_Strong_04,
			Dodge,							// �o�b�N�X�e�b�v
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
			Atk_N4_Begin,
			Atk_N4_End,
			Atk_Long_Press,
			Avoid_Dash_Begin,
			Avoid_Dash_End,
			Dash,
			Dash_Attack,
			Walk_Front,
			Idle,
			Atk_Long_Press_End,
			Atk_N1,
			Atk_N2,
			Atk_N3,
			Atk_N_All,
			Atk_S1,
			Atk_S2,
			Atk_S3,
			Atk_S4,
			Atk_S_All,
			Atk_N4_All,
			Die,
			Die_Loop,
			Damage_Small,
			Knock_Down_On_Damage,
			Knock_Down_Loop,
			Knock_Down_On_Stand,
			Dodge,

			End
		};

		enum class AdjustCameraState
		{
			To_Idle,
			To_Back,
			To_Walk_Front,
			To_Dash_Front,

			End
		};
	public:
		explicit PlayerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PlayerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const PlayerComponent& copy_comp); // for prefab
		explicit PlayerComponent() = default; // for cereal
		~PlayerComponent() override = default;

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		void Start() override;

		void PreGameUpdate(float dt)		override;
		void GameUpdate(float dt)			override;
		void RenderImGui()					override;

		void Load(ecs::Registry* registry)	override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;


		[[nodiscard]]
		u_int GetCurrentDamageAmount() const
		{
			return current_damage_amount;
		}

		[[nodiscard]]
		KnockBackLevel GetAtkKnockBackLevel() const
		{
			return atk_knock_back;
		}

		[[nodiscard]]
		mapping::rename_type::UUID GetCurrentHitEffectId() const
		{
			return current_hit_effect_id;
		}
	private:
		// �v���C���[�̏�ԊǗ��ϐ�
		StateMachine<PlayerState, void, const float> player_state{};
		// �J���������̏��
		StateMachine<AdjustCameraState, void, const float> adjust_camera_state;
		// ��s����(���͂Ȃ�(���Z�b�g) == PlayerState::End)
		PlayerState precede_input{ PlayerState::End };
		// �A�j���[�V�����L�[�t���[���̒����p(�I�[�t���[���̒���)
		std::map<std::string, u_int> adjust_keyframe_map{};
		// �A�j���[�V�������f�t���[��(��s���͂ȂǂŎg�p)
		std::map<AnimationData, u_int> animation_break_frame{};
		// �L�[�t���[�����̃C�x���g�Ǘ�
		std::map<AnimationData, system::KeyframeEvent> keyframe_events{};
		// �q�b�g���U���g
		collision::HitResult hit_result{};

		KnockBackLevel atk_knock_back{ KnockBackLevel::None };
		KnockBackLevel damage_knock_back{};

		// �v���C���[�̎����̃G���e�B�e�B
		mapping::rename_type::Entity sword_ent{};
		// ���݂̃q�b�g�G�t�F�N�g ID
		mapping::rename_type::UUID current_hit_effect_id{};
		// �q�b�g�G�t�F�N�g(S) ID
		mapping::rename_type::UUID hit_effect_s{};
		// �q�b�g�G�t�F�N�g(M) ID
		mapping::rename_type::UUID hit_effect_m{};
		// �q�b�g�G�t�F�N�g(L) ID
		mapping::rename_type::UUID hit_effect_l{};

		// �A�j���[�V�����J�ڎ���
		float anim_switch_time{ 0.1f };

		//----------------------------------//
		//			�p�����[�^				//
		//----------------------------------//

		// �q�b�g�|�C���g
		int hp{ 5 };

		//-- ��Ԃɉ����ẴX�s�[�h�ݒ� --//
		// �����̑��x
		float walk_speed{ 100.f };
		// ����̑��x
		float dash_speed{ 300.f };
		// �ʏ�U��04(�W�����v�U��)���̑��x
		float attack_04_speed{ 700.0f };
		// �_�b�V���U���̑��x
		float dash_attack_speed{ 200.0f };
		// ���U�� 04���̑��x
		float attack_strong_04_speed{ 300.f };
		// ����_�b�V�����x
		float dodge_speed{ 350.f };
		float avoid_dash_speed{ 350.f };
		float jump_movement_speed{ 300.0f };

		//-- ��Ԃ̉����Ă̍U���͐ݒ� --//
		// ���݂̍U����
		u_int current_damage_amount{ 1 };
		// �ʏ��U�����̃_���[�W
		u_int damage_normal_01{ 1 };
		u_int damage_normal_02{ 1 };
		u_int damage_normal_03{ 1 };
		u_int damage_normal_04{ 1 };
		// �ʏ틭�U�����̃_���[�W
		u_int damage_strong_01{ 2 };
		u_int damage_strong_02{ 2 };
		u_int damage_strong_03{ 2 };
		u_int damage_strong_04{ 2 };
		// �󒆎�U�����̃_���[�W
		u_int damage_jump_normal_01{ 1 };
		u_int damage_jump_normal_02{ 1 };
		u_int damage_jump_normal_03{ 1 };
		u_int damage_jump_normal_04{ 1 };
		// �󒆋��U�����̃_���[�W
		u_int damage_jump_strong{ 2 };

		// �o�ߎ���(�g�p�p�r�͎��R)
		float elapsed_time{ 0.0f };
		// ��𖳓G����
		float avoid_invincible_time{ 0.5f };
		// ��𒆂̎��Ԓ�~����
		float avoid_stop_time{ 5.f };
		// ��𖳓G���̃^�C���X�P�[��(���Ԃ��o����)
		float avoid_time_scale{ 0.5f };

		// �o�ߎ���(�g�p�͎��R)
		float anim_stop_timer{};
		// �_�b�V���U���I���̑ҋ@����
		float anim_stop_dash_atk{ 0.5f };

		//-- �e���(�A�j���[�V����)����̑J�ڎ��� --//
		// �_�b�V������ҋ@
		float switch_dash_attack_to_idle{ 0.3f };

		//-- �J���� --//
		// ��������Ă��Ȃ���(�f�t�H���g)�̃J��������
		float default_camera_length{ 300.f };
		float camera_magnification{ 1.f };
		float max_walk_camera_length{ 400.f };
		float max_dash_camera_length{ 500.f };
		// �C�[�W���O�J�n���̃J��������
		float start_camera_length{};

		// ���삵�Ă��Ȃ���(�f�t�H���g)�̃J�����I�t�Z�b�g
		DirectX::SimpleMath::Vector3 default_camera_offset{ .0f,50.f,.0f };
		DirectX::SimpleMath::Vector3 start_camera_offset{};
		float max_walk_camera_offset_x{ 40.f };
		float max_dash_camera_offset_x{ 70.f };

		// �J���������ύX����(���ݎ���)
		float current_transition_time{};
		// �ҋ@���̃J���������ύX����
		float transition_time_to_idle{ 1.5f };
		// �������̃J���������ύX����
		float transition_time_to_walk{ 0.5f };
		// �_�b�V�����̃J���������ύX����
		float transition_time_to_dash{ 0.7f };

		//-- ��ԃt���O --//
		// �W�����v�t���O
		bool is_jumping{ false };
		// ����t���O
		bool is_avoid{ false };
		// ���̓t���O
		bool is_input{ true };

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
		void InitializeAnimationVariable(const PlayerState state = PlayerState::End, const float long_press_time = 0.0f)
		{
			precede_input = state;
			this->long_press_time = long_press_time;
		}

		/**
		 * @brief : enum class(AnimationState)��int�^�ɕϊ�
		 */
		[[nodiscard]]
		int GetAnimDataIndex(AnimationData anim_state) const;

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

		void RegistryKeyframeEvent(AnimationData anim_data, const std::string& key_name);
		[[nodiscard]]
		system::KeyframeEvent& GetKeyframeEvent(AnimationData anim_data);

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

		void SetCurrentDamage(const u_int amount)
		{
			current_damage_amount = amount;
		}

		/**
		 * @brief : ���̃A�j���[�V������PlayerState::Attacking_Normal_Long_Press
		 *			�ɑJ�ڂ��ėǂ����̔���֐��B
		 *			long_press_time�̎��ԂŔ��f
		 */
		[[nodiscard]]
		bool IsNextAnimationLongPressAttack() const;

		/**
		 * @brief  : Left Stick���͂��f�b�h�]�[�����ɂ��邩�ǂ�������
		 * @return : true -> �f�b�h�]�[�����ɂ���
		 */
		[[nodiscard]]
		bool IsDeadZoneStickLeft() const;

		/**
		 * @brief : Right Trigger���͂��f�b�h�]�[�����ɂ��邩�ǂ�������
		 * @return : true -> �f�b�h�]�[�����ɂ���
		 */
		[[nodiscard]]
		bool IsDeadZoneTriggerRight() const;

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
		 * @brief : �L�[�t���[���C�x���g�̏�����
		 */
		void InitializeKeyframeEvent();

		/**
		 * @brief :	�J������������StateMachine�̏�����
		 */
		void InitializeAdjustCameraLengthState(ecs::Registry* registry, const mapping::rename_type::Entity& ent);

		/**
		 * @brief : �_���[�W����
		 */
		void OnDamaged(const DamageData& damage_data);

		/**
		 * @brief : ���̃R���W�����Ƃ̃q�b�g������
		 */
		void OnHit(const DamageData& damage_data, const collision::HitResult& hit_result);

		/**
		 * @brief : �W���X�g��𔻒�̃q�b�g������
		 */
		void OnHitAvoidRange(const collision::HitResult& hit_result);

		/**
		 * @brief : �A�j���[�V�����J�ڎ��Ԃ̎擾 & �J�ڎ��Ԃ̃��Z�b�g
		 * @param reset_time : ���Z�b�g���̑J�ڎ���
		 * @return : ���Z�b�g�����O�̑J�ڎ���
		 */
		float GetAndResetAnimSwitchTime(float reset_time = 0.1f);

		/**
		 * @brief : �J�������������p�֐�(adjust_camera_state�ł̊Ǘ��֐�)
		 */
		// �ҋ@��Ԏ��ł̃J��������
		void CameraLenToIdle(float dt);
		// ���X�e�B�b�N�����͎��ł̃J��������
		void CameraLenToBack(float dt);
		// ����(�O)���̃J��������
		void CameraLenToWalkFront(float dt);
		// �_�b�V��(�O)���̃J��������
		void CameraLenToDashFront(float dt);

		/**
		 * @brief : StateMachine�N���X�ŊǗ�����v���C���[�̏�Ԋ֐�
		 */
		void TPose(float dt);					// T-�|�[�Y
		void Idle(float dt);					// �ҋ@
		void WalkFront(float dt);				// ����
		void Dodge(float dt);					// �o�b�N�X�e�b�v
		void AvoidDashBegin(float dt);			// �_�b�V�����
		void AvoidDashEnd(float dt);
		void Dash(float dt);					// �_�b�V��
		void DamageSmall(float dt);				// �_���[�W
		void DamageBig(float dt);
		void KnockDownFrontLoop(float dt);		// �m�b�N�_�E��
		void KnockDownFrontStandUp(float dt);
		void Die(float dt);						// ���S
		void AtkNormal01(float dt);		// �ʏ�U��(��)
		void AtkNormal02(float dt);
		void AtkNormal03(float dt);
		void AttackingNormalLongPress(float dt); // �ʏ�U��(�㒷����)
		void AttackNormalLongPressEnd(float dt);
		void AtkStrong01(float dt);		 // �ʏ�U��(��)
		void AtkStrong02(float dt);
		void AtkStrong03(float dt);
		void AtkStrong04(float dt);
		void DashAttack(float dt);				// �_�b�V���U��
	};
} // cumulonimbus::component