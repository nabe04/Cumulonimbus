#include <cmath>

#include <d3d11.h>
#include <SimpleMath.h>

#include "player.h"
#include "ecs.h"

#include "input_manager.h"
#include "transform.h"
#include "transform_component.h"

#include "scene.h"
#include "fbx_model_component.h"
#include "enemy.h"
#include "player_damage_collision.h"
#include "sword_collision.h"
#include "player_magic_attack_collision.h"

// �G�t�F�N�g�B
#include "player_dash_effect_component.h"
#include "player_attack_magic_effect_component.h"
#include "player_evasion_effect_component.h"
#include "player_just_evasion_effect_component.h"
#include "player_damage_effect_component.h"
#include "player_jump_effect_component.h"
#include "player_magic_circle_effect_component.h"
#include "player_attack_hit_effect_component.h"
#include "player_damage_rush_effect_component.h"
#include "player_attack_effect01_component.h"
#include "player_attack_effect02_component.h"
#include "player_attack_effect03_component.h"
#include "player_attack_effect04_component.h"
#include "player_attack_effect05_component.h"
#include "player_attack_effect06_component.h"
#include "player_attack_effect07_component.h"

#include "sound_resource.h"

#include "game_end.h"

PlayerComponent::PlayerComponent(Entity* entity, ActorType actor_type)
	: ActorComponent{ entity, actor_type }
{
	player_state.AddState(PlayerState::Stay, [this](const float delta_time) {PlayerStay(delta_time);      });
	player_state.AddState(PlayerState::Move, [this](const float delta_time) {PlayerMove(delta_time);      });
	player_state.AddState(PlayerState::Stop, [this](const float delta_time) {PlayerStop(delta_time);      });
	player_state.AddState(PlayerState::Evasion, [this](const float delta_time) {PlayerEvasion(delta_time);   });
	player_state.AddState(PlayerState::Jump, [this](const float delta_time) {PlayerJump(delta_time);      });
	player_state.AddState(PlayerState::Landing, [this](const float delta_time) {PlayerLanding(delta_time);      });
	player_state.AddState(PlayerState::Attack01, [this](const float delta_time) {PlayerAttack01(delta_time);  });
	player_state.AddState(PlayerState::Attack02, [this](const float delta_time) {PlayerAttack02(delta_time);  });
	player_state.AddState(PlayerState::Attack03_1, [this](const float delta_time) {PlayerAttack03_1(delta_time);  });
	player_state.AddState(PlayerState::Attack03_2, [this](const float delta_time) {PlayerAttack03_2(delta_time);  });
	player_state.AddState(PlayerState::Attack04_1, [this](const float delta_time) {PlayerAttack04_1(delta_time);  });
	player_state.AddState(PlayerState::Attack04_2, [this](const float delta_time) {PlayerAttack04_2(delta_time);  });
	player_state.AddState(PlayerState::Attack05_1, [this](const float delta_time) {PlayerAttack05_1(delta_time);  });
	player_state.AddState(PlayerState::Attack05_2, [this](const float delta_time) {PlayerAttack05_2(delta_time);  });
	player_state.AddState(PlayerState::Damage, [this](const float delta_time) {PlayerDamage(delta_time);    });
	player_state.AddState(PlayerState::Counter, [this](const float delta_time) {PlayerCounter(delta_time);   });
	player_state.AddState(PlayerState::Fall, [this](const float delta_time) {PlayerFall(delta_time);      });
	player_state.AddState(PlayerState::Death, [this](const float delta_time) {PlayerDeath(delta_time);     });
	player_state.AddState(PlayerState::JumpDash, [this](const float delta_time) {PlayerJumpDash(delta_time);     });

	//move_state.AddState(MoveState::Forward, [this](const float delta_time) {PlayerForward(delta_time); });
	//move_state.AddState(MoveState::Back,    [this](const float delta_time) {PlayerBack(delta_time);    });
	//move_state.AddState(MoveState::Right,   [this](const float delta_time) {PlayerRight(delta_time);   });
	//move_state.AddState(MoveState::Left,    [this](const float delta_time) {PlayerLeft(delta_time);    });

	old_position = DirectX::XMFLOAT3(0, 0, 0);

	angle = DirectX::XMFLOAT3(0, 0, 0);
	old_sita = 0.0f;
	adjust_angle = DirectX::XMFLOAT3(0, 0, 0);

	//speed = 2.5f;
	velocity = 0.0f;
	max_velocity = 3.0f;
	forward_move_flag = false;
	back_move_flag = false;
	//speed_angle_y = 30.0f;

	//evasion_speed = 4;
	evasion_time = 0;
	stiffness_flag = false;
	just_evasion_flag = false;

	//jump_force = 12;
	graund_flag = false;
	jump_time = 0;
	jump_start_flag = false;

	//jump_dash_speed = 4.0f;
	jump_dash_time = 0;
	jump_dash_flag = false;

	hp = 30;
	old_hp = hp;
	//damage_flag = false;
	damage_time = 0;
	once_adjust_flag = false;
	blow_away = 6.5f;

	next_attack_flag = false;
	attack_finish_flag = false;
	attack_time = 0;
	attack_hit_flag = false;
	attack_effect_flag = false;
	//attack_move_speed = 0.1f;

	//counter_flag = false;
	counter_time = 0;

	position = XMFLOAT3(0, 0, 0);
	player_front = XMFLOAT3(0, 0, 0);

	reduction_x = false;
	delta_position = XMFLOAT3(0, 0, 0);
	radius = 18;
	enemy_position = XMFLOAT3(0, 0, 0);
	enemy_front = XMFLOAT3(0, 0, 0);
	stage_position = XMFLOAT3(0, 0, 0);
	stage_radius = 10;

	entity->GetComponent<TransformComponent>()->GetTransform()->SetPosition(DirectX::XMFLOAT3(100, 0, 0));
	entity->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation(DirectX::XMFLOAT3(0, -90, 0));
	entity->GetComponent<TransformComponent>()->GetTransform()->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	// For Imgui
	str_player_states.emplace_back("Stay");
	str_player_states.emplace_back("Move");
	str_player_states.emplace_back("Stop");
	str_player_states.emplace_back("Evasion");
	str_player_states.emplace_back("Jump");
	str_player_states.emplace_back("Landing");
	str_player_states.emplace_back("Attack01");
	str_player_states.emplace_back("Attack02");
	str_player_states.emplace_back("Attack03_1");
	str_player_states.emplace_back("Attack03_2");
	str_player_states.emplace_back("Attack04_1");
	str_player_states.emplace_back("Attack04_2");
	str_player_states.emplace_back("Attack05_1");
	str_player_states.emplace_back("Attack05_2");
	str_player_states.emplace_back("Damage");
	str_player_states.emplace_back("Counter");
	str_player_states.emplace_back("Fall");
	str_player_states.emplace_back("Death");
	str_player_states.emplace_back("JumpDash");

	player_state.SetState(PlayerState::Stay);
	GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(0, true);

	rasterizer_state = RasterizeState::Cull_Back_CCW_True;

	GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
}


void PlayerComponent::NewFrame(const float delta_time)
{
	auto enemy = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy);
	enemy_position = enemy->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

	auto stage = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Stage);
	stage_position = stage->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	stage_radius = 360;

	GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
	GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->SetRadius(5, 0);

	GetEntity()->GetComponent<PlayerMagicAttackCollisionComponent>()->IsJudge(false);

	GetEntity()->GetComponent<PlayerDamageCollisionComponent>()->IsJudge(true);
}

void PlayerComponent::Update(const float delta_time)
{
	position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

	forward_move_flag = false;
	back_move_flag = false;

	graund_flag = false;

	auto input = GetEntity()->GetScene()->GetPadLink();
	if (!input)
		assert(!"Not found Pad link");

	// �d�́B
	if (player_state.GetState() != PlayerState::JumpDash)
	{
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionY(-5.5f);
	}
	position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

	if (position.y < 0)
	{
		// �����Ă���Ƃ��n�ʂɂ�����B
		if (player_state.GetState() == PlayerState::Fall)
		{
			StateInit();
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Landing), false, 0.1f);
			player_state.SetState(PlayerState::Landing);
			// ���n���Đ��B
			GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Landing)->Play(false);
		}
		graund_flag = true;
		jump_dash_flag = false;
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->SetPositionY(0);
	}
	else if (player_state.GetState() == PlayerState::Stay)// �����Ă���Ƃ��B
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Fall), true, 0.1f);
		player_state.SetState(PlayerState::Fall);
	}

	angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();

	// �ړ������B
	if (player_state.GetState() != PlayerState::Damage &&
		player_state.GetState() != PlayerState::Death &&
		player_state.GetState() != PlayerState::JumpDash &&
		player_state.GetState() != PlayerState::Evasion)
	{
		if (input->State(InputType::GamePad::Up_DPad, 0))
		{
			forward_move_flag = true;

			// �v���C���[��front�x�N�g���B
			DirectX::XMFLOAT3 front = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
			DirectX::XMVECTOR front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)); // ���ςƂ����Ƃ��͕K�����K���B

			// �J������front�x�N�g���B
			DirectX::XMFLOAT3 camera_front = GetEntity()->GetScene()->GetView()->GetCameraFront();
			camera_front.y = 0;
			DirectX::XMVECTOR camera_front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera_front)); // ���ςƂ����Ƃ��͕K�����K���B

			// ront�x�N�g�����m�̓��ς����߂āAacos����ς̌��ʂɂ����ă�(���W�A��)�����߂�B
			DirectX::XMVECTOR data = DirectX::XMVector3Dot(front_v, camera_front_v);
			float cos_sita = 0.0f;
			DirectX::XMStoreFloat(&cos_sita, data);

			//float sita = acosf(cos_sita);
			// �֐��ɂ͊p�x�œ��ꂽ���̂Ń�(0.01745f)�Ŋ���B
			//sita /= 0.01745f;
			//adjust_angle.y += 3.14f * 2.0f * delta_time;
			//cos_sita = 1.0f - cos_sita;
			//adjust_angle.y += 5.0f * delta_time;

			// �R�T�C���J�[�u�ŉ񂷁B
			cos_sita = (1.0f - cos_sita) * speed_angle_y;
			//if (cos_sita > adjust_angle.y)
			//{
			//	cos_sita = adjust_angle.y;
			//}

			// �J������front�x�N�g�����E�������𔻒肵�����̂ŃJ�����̃x�N�g������v���C���[�̃x�N�g���ɑ΂��Ă�
			// �O�ς��Ƃ��ăv���C���[�̃x�N�g�������E�ǂ����������ʂ̃x�N�g����Y�����̃v���X�}�C�i�X�Ŕ��f����B
			DirectX::XMVECTOR cross_v = DirectX::XMVector3Cross(camera_front_v, front_v);
			DirectX::XMFLOAT3 cross;
			DirectX::XMStoreFloat3(&cross, cross_v);

			if (cross.y < 0)
			{
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(cos_sita);
				angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
				if (!std::isfinite(angle.y))
				{
					assert(!"Angle is nan");
				}
			}
			else
			{
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita);
				angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
				if (!std::isfinite(angle.y))
				{
					assert(!"Angle is nan");
				}
			}

			// �ړ��B
			angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
			SetVelocity(speed, delta_time);
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(velocity * sinf(angle.y * 0.01745f));
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(velocity * cosf(angle.y * 0.01745f));
		}
		if (input->State(InputType::GamePad::Down_DPad, 0))
		{
			back_move_flag = true;
			// �v���C���[��front�x�N�g���B
			DirectX::XMFLOAT3 front = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
			DirectX::XMVECTOR front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)); // ���ςƂ����Ƃ��͕K�����K���B


			// �J������front�x�N�g���B
			DirectX::XMFLOAT3 camera_front = GetEntity()->GetScene()->GetView()->GetCameraFront();
			camera_front.y = 0;
			DirectX::XMVECTOR camera_front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera_front)); // ���ςƂ����Ƃ��͕K�����K���B

			// ront�x�N�g�����m�̓��ς����߂āAacos����ς̌��ʂɂ����ă�(���W�A��)�����߂�B
			DirectX::XMVECTOR data = DirectX::XMVector3Dot(front_v, camera_front_v);
			float cos_sita = 0.0f;
			DirectX::XMStoreFloat(&cos_sita, data);
			//float sita = acosf(cos_sita);
			//// �֐��ɂ͊p�x�œ��ꂽ���̂Ń�(0.01745f)�Ŋ���B
			//sita /= 0.01745f;

			//adjust_angle.y += 5.0f * delta_time;

			// �R�T�C���J�[�u�ŉ񂷁B
			cos_sita = (-1.0f - cos_sita) * speed_angle_y;
			//if (cos_sita > adjust_angle.y)
			//{
			//	cos_sita = adjust_angle.y;
			//}

			// �J������front�x�N�g�����E�������𔻒肵�����̂ŃJ�����̃x�N�g������v���C���[�̃x�N�g���ɑ΂��Ă�
			// �O�ς��Ƃ��ăv���C���[�̃x�N�g�������E�ǂ����������ʂ̃x�N�g����Y�����̃v���X�}�C�i�X�Ŕ��f����B
			DirectX::XMVECTOR cross_v = DirectX::XMVector3Cross(camera_front_v, front_v);
			DirectX::XMFLOAT3 cross;
			DirectX::XMStoreFloat3(&cross, cross_v);

			if (cross.y < 0)
			{
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(cos_sita);
				angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
				if (!std::isfinite(angle.y))
				{
					assert(!"Angle is nan");
				}
			}
			else
			{
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita);
				angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
				if (!std::isfinite(angle.y))
				{
					assert(!"Angle is nan");
				}
			}

			//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(180);

			// �ړ��B
			angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
			SetVelocity(speed, delta_time);
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(velocity * sinf(angle.y * 0.01745f));
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(velocity * cosf(angle.y * 0.01745f));
		}
		if (input->State(InputType::GamePad::Left_DPad, 0))
		{
			// �v���C���[��front�x�N�g���B
			DirectX::XMFLOAT3 front = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
			DirectX::XMVECTOR front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)); // ���ςƂ����Ƃ��͕K�����K���B

			// �J������front�x�N�g���B
			DirectX::XMFLOAT3 camera_front = GetEntity()->GetScene()->GetView()->GetCameraFront();
			camera_front.y = 0;
			DirectX::XMVECTOR camera_front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera_front)); // ���ςƂ����Ƃ��͕K�����K���B

			// ront�x�N�g�����m�̓��ς����߂āAacos����ς̌��ʂɂ����ă�(���W�A��)�����߂�B
			DirectX::XMVECTOR data = DirectX::XMVector3Dot(front_v, camera_front_v);
			float cos_sita = 0.0f;
			DirectX::XMStoreFloat(&cos_sita, data);

			float difference = 0.0f;
			if (cos_sita > 1.0f)
			{
				difference = fabsf(1.0f - cos_sita);
				if (difference > 0)
				{
					cos_sita = 1.0f;
				}
			}
			else if (cos_sita < -1.0f)
			{
				difference = fabsf(1.0f + cos_sita);
				if (difference > 0)
				{
					cos_sita = -1.0f;
				}
			}

			float sita = acosf(cos_sita);
			// �֐��ɂ͊p�x�œ��ꂽ���̂Ń�(0.01745f)�Ŋ���B
			sita /= 0.01745f;

			//adjust_angle.y += 5.0f * delta_time;
			//cos_sita = cos_sita * 10;
			//if (cos_sita > adjust_angle.y)
			//{
			//	cos_sita = adjust_angle.y;
			//}

			// �J������front�x�N�g�����E�������𔻒肵�����̂ŃJ�����̃x�N�g������v���C���[�̃x�N�g���ɑ΂��Ă�
			// �O�ς��Ƃ��ăv���C���[�̃x�N�g�������E�ǂ����������ʂ̃x�N�g����Y�����̃v���X�}�C�i�X�Ŕ��f����B
			DirectX::XMVECTOR cross_v = DirectX::XMVector3Cross(camera_front_v, front_v);
			DirectX::XMFLOAT3 cross;
			DirectX::XMStoreFloat3(&cross, cross_v);

			//if (cross.y < 0)
			//{
			//	if (forward_move_flag == false && back_move_flag == false)
			//	{
			//		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita);
			//	}
			//}
			//else
			//{
			//	if (forward_move_flag == false && back_move_flag == false)
			//	{
			//		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(cos_sita);
			//	}
			//}

			if (cross.y < 0)
			{
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(sita);
				angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
				if (!std::isfinite(angle.y))
				{
					assert(!"Angle is nan");
				}
			}
			else
			{
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-sita);
				angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
				if (!std::isfinite(angle.y))
				{
					assert(!"Angle is nan");
				}
			}

			float left_stick = pad_input::GetAnalogStickVal(0, PadInput::AnalogStickType::Left_Y_Thumb);

			// �ړ��̓��͓͂����Ă��邪�X�e�b�N�̌X�����Ȃ��ꍇ�̓L�[���͂Ƃ݂Ȃ��B
			//if (left_stick == 0)
			//{
			//	if (cross.y < 0)
			//	{
			//		if (forward_move_flag == true) // �O�������Ă���Ƃ��B
			//		{
			//			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita);
			//		}
			//		else if (back_move_flag == true) // ���������Ă���Ƃ��B
			//		{
			//			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(cos_sita);
			//		}
			//		else //�^���������Ă���Ƃ��B
			//		{
			//			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita);
			//		}
			//		if (forward_move_flag == false && back_move_flag == false)
			//		{
			//			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita);
			//		}
			//	}
			//	else
			//	{
			//		if (forward_move_flag == false && back_move_flag == false)
			//		{
			//			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(cos_sita);
			//		}
			//	}
			//}
			//else
			//{
			//	if (forward_move_flag == true) // �O�������Ă���Ƃ��B
			//	{
			//		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita * (1 - left_stick));
			//	}
			//	else if (back_move_flag == true) // ���������Ă���Ƃ��B
			//	{
			//		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita * (-1 - left_stick));
			//	}
			//	else //�^���������Ă���Ƃ��B
			//	{
			//		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita * (1 - left_stick));
			//	}
			//}

			if (left_stick == 0.0f)
			{
				if (forward_move_flag == true) // �O�������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-45); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
				else if (back_move_flag == true) // ���������Ă���Ƃ��B
				{
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-135); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
				else //�^���������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-90); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
			}
			else
			{
				if (forward_move_flag == true) // �O�������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-90 * (1 - left_stick)); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
				else if (back_move_flag == true) // ���������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-90 * (1 - left_stick)); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
				else //�^���������Ă���Ƃ��R���g���[���̃X�e�B�b�N��Y�l��0�ɂȂ��Ă��܂����ߐ^���̕�����p�ӂ���B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-90 * (1 - left_stick)); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
				}
			}

			// �ړ��B
			angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
			SetVelocity(speed, delta_time);
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(velocity * sinf(angle.y * 0.01745f));
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(velocity * cosf(angle.y * 0.01745f));

			//adjust_angle.y = -1;
			//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustAngle(adjust_angle);
		}
		else if (input->State(InputType::GamePad::Right_DPad, 0))
		{
			// �v���C���[��front�x�N�g���B
			DirectX::XMFLOAT3 front = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
			DirectX::XMVECTOR front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)); // ���ςƂ����Ƃ��͕K�����K���B

			// �J������front�x�N�g���B
			DirectX::XMFLOAT3 camera_front = GetEntity()->GetScene()->GetView()->GetCameraFront();
			camera_front.y = 0;
			DirectX::XMVECTOR camera_front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera_front)); // ���ςƂ����Ƃ��͕K�����K���B

			// ront�x�N�g�����m�̓��ς����߂āAacos����ς̌��ʂɂ����ă�(���W�A��)�����߂�B
			DirectX::XMVECTOR data = DirectX::XMVector3Dot(front_v, camera_front_v);
			float cos_sita = 0.0f;
			DirectX::XMStoreFloat(&cos_sita, data);

			float difference = 0.0f;
			if (cos_sita > 1.0f)
			{
				difference = fabsf(1.0f - cos_sita);
				if (difference > 0)
				{
					cos_sita = 1.0f;
				}
			}
			else if (cos_sita < -1.0f)
			{
				difference = fabsf(1.0f + cos_sita);
				if (difference > 0)
				{
					cos_sita = -1.0f;
				}
			}

			float sita = acosf(cos_sita);
			// �֐��ɂ͊p�x�œ��ꂽ���̂Ń�(0.01745f)�Ŋ���B
			sita /= 0.01745f;

			//adjust_angle.y += 5.0f * delta_time;
			//cos_sita = cos_sita * 10;
			//if (cos_sita > adjust_angle.y)
			//{
			//	cos_sita = adjust_angle.y;
			//}

			// �J������front�x�N�g�����E�������𔻒肵�����̂ŃJ�����̃x�N�g������v���C���[�̃x�N�g���ɑ΂��Ă�
			// �O�ς��Ƃ��ăv���C���[�̃x�N�g�������E�ǂ����������ʂ̃x�N�g����Y�����̃v���X�}�C�i�X�Ŕ��f����B
			DirectX::XMVECTOR cross_v = DirectX::XMVector3Cross(camera_front_v, front_v);
			DirectX::XMFLOAT3 cross;
			DirectX::XMStoreFloat3(&cross, cross_v);

			if (cross.y < 0) // �v���C���[���J�����̃x�N�g���������ɂ���B
			{
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(sita);
				angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
				if (!std::isfinite(angle.y))
				{
					assert(!"Angle is nan");
				}
			}
			else // �v���C���[���J�����̃x�N�g�������E�ɂ���B
			{
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-sita);
				angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
				if (!std::isfinite(angle.y))
				{
					assert(!"Angle is nan");
				}
			}

			//if (cross.y < 0)
			//{
			//	if (forward_move_flag == false && back_move_flag == false)
			//	{
			//		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(sita);
			//	}
			//}
			//else
			//{
			//	if (forward_move_flag == false && back_move_flag == false)
			//	{
			//		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-sita);
			//	}
			//}

			float left_stick = pad_input::GetAnalogStickVal(0, PadInput::AnalogStickType::Left_Y_Thumb);

			// �ړ��̓��͓͂����Ă��邪�X�e�b�N�̌X�����Ȃ��ꍇ�̓L�[���͂Ƃ݂Ȃ��B
			if (left_stick == 0.0f)
			{
				if (forward_move_flag == true) // �O�������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(45); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
				else if (back_move_flag == true) // ���������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(135); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
				else //�^���������Ă���Ƃ��B
				{
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(90); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
			}
			else
			{
				if (forward_move_flag == true) // �O�������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(90 * (1 - left_stick)); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
				else if (back_move_flag == true) // ���������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(90 * (1 - left_stick)); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
				else //�^���������Ă���Ƃ��B
				{
					GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(90 * (1 - left_stick)); // �J�����̐��ʂɃ��Z�b�g���Ă��邽�߁A�J�����̐��ʂ���̊p�x�𑫂��B
					angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
					if (!std::isfinite(angle.y))
					{
						assert(!"Angle is nan");
					}
				}
			}

			// �ړ��B
			angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
			SetVelocity(speed, delta_time);
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(velocity * sinf(angle.y * 0.01745f));
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(velocity * cosf(angle.y * 0.01745f));

			//adjust_angle.y = 1;
			//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustAngle(adjust_angle);
		}

		// �U�����Ă���Ԃ͈ړ��ʂ�ł������Ă���B
		if (player_state.GetState() >= PlayerState::Attack01 &&
			player_state.GetState() <= PlayerState::Attack05_2 &&
			attack_finish_flag == false)
		{
			position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
			DirectX::XMFLOAT3 delta_position = DirectX::XMFLOAT3(position.x - old_position.x, position.y - old_position.y, position.z - old_position.z);
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPosition(DirectX::XMFLOAT3(-delta_position.x, -delta_position.y, -delta_position.z));
		}
	}

	// �ړ��A�j���[�V�����B
	if (player_state.GetState() == PlayerState::Stay ||
		player_state.GetState() == PlayerState::Stop ||
		attack_finish_flag == true)
	{
		if (input->State(InputType::GamePad::Up_DPad, 0) || input->State(InputType::GamePad::Down_DPad, 0) ||
			input->State(InputType::GamePad::Right_DPad, 0) || input->State(InputType::GamePad::Left_DPad, 0))
		{
			//GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(AnimeState::Move, true, 1.0f);

			// �A�j���[�V�����̍Đ��̓��o��������̃A�j���[�V�����ɂ���B
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Evasion), false, 0.1f);
			StateInit();
			player_state.SetState(PlayerState::Move);
		}
	}

	// Stop���[�V�����B
	if (player_state.GetState() == PlayerState::Stay)
	{
		if (player_state.GetOldState() == PlayerState::Move && velocity > 0)
		{
			player_state.SetState(PlayerState::Stop);
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stop), false, 0.1f);
			// Stop���Đ��B
			GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Stop)->Play(false);
		}
	}

	// ����B
	if (player_state.GetState() != PlayerState::Death &&
		player_state.GetState() != PlayerState::Jump &&
		player_state.GetState() != PlayerState::Fall &&
		player_state.GetState() != PlayerState::Evasion)
	{
		if (input->Trg(InputType::GamePad::Right_Shoulder, 0))
		{
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Evasion), false, 0.001f);
			StateInit();
			player_state.SetState(PlayerState::Evasion);
			// �G�t�F�N�g�B
			GetEntity()->GetComponent<PlayerEvasionEffectComponent>()->Stop();
			GetEntity()->GetComponent<PlayerEvasionEffectComponent>()->Play();
			// ������Đ��B
			GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Step)->Play(false);
		}
	}

	// �W�����v�B
	if (graund_flag == true &&
		player_state.GetState() != PlayerState::Damage &&
		player_state.GetState() != PlayerState::Death &&
		player_state.GetState() != PlayerState::Jump &&
		player_state.GetState() != PlayerState::Evasion)
	{
		if (input->Trg(InputType::GamePad::Pad_A, 0))
		{
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::JumpStart), false, 0.1f);
			StateInit();
			jump_start_flag = true;
			player_state.SetState(PlayerState::Jump);
			// �G�t�F�N�g�B
			GetEntity()->GetComponent<PlayerJumpEffectComponent>()->Stop();
			GetEntity()->GetComponent<PlayerJumpEffectComponent>()->Play();
			// �W�����v���Đ��B
			GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Jump)->Play(false);
		}
	}

	// �W�����v�_�b�V���B
	if (player_state.GetState() != PlayerState::Damage &&
		player_state.GetState() != PlayerState::Death &&
		player_state.GetState() != PlayerState::JumpDash)
	{
		if (player_state.GetState() == PlayerState::Jump ||
			player_state.GetState() == PlayerState::Fall)
		{
			if (input->Trg(InputType::GamePad::Pad_A, 0) && jump_time != 0 && jump_dash_flag == false)
			{
				GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Evasion), false, 0.1f);
				StateInit();
				player_state.SetState(PlayerState::JumpDash);
				// �G�t�F�N�g�B(��)
				GetEntity()->GetComponent<PlayerEvasionEffectComponent>()->Stop();
				GetEntity()->GetComponent<PlayerEvasionEffectComponent>()->my_position = position;
				GetEntity()->GetComponent<PlayerEvasionEffectComponent>()->Play();
				// �G�t�F�N�g�B(�T�[�N��)
				GetEntity()->GetComponent<PlayerMagicCircleEffectComponent>()->Stop();
				GetEntity()->GetComponent<PlayerMagicCircleEffectComponent>()->Play();
				// ������Đ��B
				GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Step)->Play(false);
			}
		}
	}
	//GetEntity()->GetComponent<PlayerMagicCircleEffectComponent>()->Play();

	// �U��(��)�B
	if (player_state.GetState() != PlayerState::Evasion)
	{
		if (player_state.GetState() == PlayerState::Stay ||
			player_state.GetState() == PlayerState::Stop ||
			player_state.GetState() == PlayerState::Move)
		{
			if (input->Trg(InputType::GamePad::Pad_X, 0))
			{
				GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Attack01), false, 0.1f);
				StateInit();
				player_state.SetState(PlayerState::Attack01);
				// �U�����Đ��B
				GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Slash)->Play(false);
			}
		}
	}

	// �J�E���^�[(��)�B
	if (PlayerState::Evasion == player_state.GetState() &&
		PlayerState::Counter != player_state.GetState() &&
		just_evasion_flag == true)
	{
		if (input->Trg(InputType::GamePad::Pad_X, 0))
		{
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Counter), false, 0.1f);
			StateInit();
			player_state.SetState(PlayerState::Counter);
		}
	}

	// ����(��)�B
	if (player_state.GetState() != PlayerState::Damage &&
		player_state.GetState() != PlayerState::Death)
	{
		//// �����œG�Ƃ̓����蔻����Ƃ�B
		//position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
		//bool damage_hit_flag = true;
		if (hp < old_hp)
		{
			player_front = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
			enemy_front = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Damage), false, 0.1f);
			StateInit();
			player_state.SetState(PlayerState::Damage);

			int enemy_state = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<EnemyComponent>()->GetEnemyState();

			if (static_cast<EnemyComponent::EnemyStates>(enemy_state) >= EnemyComponent::EnemyStates::Rush_begin &&
				static_cast<EnemyComponent::EnemyStates>(enemy_state) <= EnemyComponent::EnemyStates::Rush_end)
			{
				// �G�t�F�N�g(�ːi��H������Ƃ�)�B
				GetEntity()->GetComponent<PlayerDamageRushEffectComponent>()->Stop();
				GetEntity()->GetComponent<PlayerDamageRushEffectComponent>()->Play();
			}
			else
			{
				// �G�t�F�N�g�B
				GetEntity()->GetComponent<PlayerDamageEffectComponent>()->Stop();
				GetEntity()->GetComponent<PlayerDamageEffectComponent>()->Play();
			}
		}
	}

	if (hp <= 0) // ���񂾂Ƃ��͋����I��Death�ɃZ�b�g����B
	{
		if (player_state.GetState() != PlayerState::Death)
		{
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Death), false);
		}
		player_state.SetState(PlayerState::Death);
	}

	// �J�����ʒu�␳�B
	position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	//GetEntity()->GetScene()->GetView()->SetCameraPos(DirectX::XMFLOAT3(position.x, position.y + 35, position.z - 120), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));
	//GetEntity()->GetScene()->GetView()->SetCameraTarget(DirectX::XMFLOAT3(position.x, position.y + 22, position.z));

	// state�̃A�b�v�f�[�g�͈�񂾂��B
	angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
	player_state.Update(delta_time);	// Player's state
	GetEntity()->GetComponent<FbxModelComponent>()->UpdateAnimState(delta_time); // FBX Animation state

	// �ړ��ʂ����߂�B
	position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	delta_position = DirectX::XMFLOAT3(position.x - old_position.x, position.y - old_position.y, position.z - old_position.z);
	if (delta_position.x == 0 && delta_position.y == 0 && delta_position.z == 0)
	{
		//if (player_state.GetState() >= PlayerState::Attack01 &&
		//	player_state.GetState() <= PlayerState::Attack05_2 ||
		//	player_state.GetState() == PlayerState::Counter ||
		//	player_state.GetState() == PlayerState::Evasion ||
		//	player_state.GetState() == PlayerState::Stop ||
		//	player_state.GetState() == PlayerState::Damage ||
		//	player_state.GetState() == PlayerState::Jump ||
		//	player_state.GetState() == PlayerState::Fall ||
		//	player_state.GetState() == PlayerState::JumpDash ||
		//	player_state.GetState() == PlayerState::Landing)
		//{
		//
		//}
		//else if (player_state.GetState() != PlayerState::Death &&
		//	     player_state.GetState() != PlayerState::Stay) // state��stay����Ȃ��ꍇ�B
		if (player_state.GetState() == PlayerState::Move)
		{
			GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
			player_state.SetState(PlayerState::Stay);
		}
	}

	// �ړ�������������B
	position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	// �����ʁB(��r�p)
	float reduction_volume = 0;
	// �����ʁB(�ۑ��p)
	float reduction_store = 0;
	// �␳�����ʒ����p�ϐ��B
	const float corrected_decimal = 0.01f;
	// �ړ��ʌv�Z�B
	delta_position = DirectX::XMFLOAT3(position.x - old_position.x, position.y - old_position.y, position.z - old_position.z);

	// �v���C���[�̋����v�Z�B
	//XMVECTOR vec = XMLoadFloat3(&XMFLOAT3(stage_position.x - position.x, stage_position.y - position.y, stage_position.z - position.z));
	DirectX::SimpleMath::Vector3 vec{ stage_position.x - position.x, stage_position.y - position.y, stage_position.z - position.z };
	float length = 0.0f;
	XMStoreFloat(&length, XMVector3Length(vec));


	// ���������B
	float move_limit_length = stage_radius + radius;
	while (true)
	{
		position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
		//vec = XMLoadFloat3(&XMFLOAT3(stage_position.x - position.x, stage_position.y - position.y, stage_position.z - position.z));
		vec = DirectX::SimpleMath::Vector3{ stage_position.x - position.x, stage_position.y - position.y, stage_position.z - position.z };
		XMStoreFloat(&length, XMVector3Length(vec));

		reduction_volume = delta_position.x;
		float reduction_volume2 = delta_position.z;

		// ��r���邽�߂Ƀv���X�ɂ��낦�Ă����B
		if (reduction_volume < 0)
		{
			reduction_volume *= -1;
		}
		// ��r���邽�߂Ƀv���X�ɂ��낦�Ă����B
		if (reduction_volume2 < 0)
		{
			reduction_volume2 *= -1;
		}

		// ���������B
		if (length > move_limit_length)
		{
			// �ړ��ʂ̑����ق����ɖ߂��Ă����B
			if (reduction_volume < reduction_volume2) // (�ړ���X���ړ���Z��菭�Ȃ��Ƃ�)���Z�������炷�B
			{
				reduction_store += reduction_volume2 * corrected_decimal;
				reduction_x = false;
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(-delta_position.z * corrected_decimal);
			}
			else if (reduction_volume > reduction_volume2)// (�ړ���X���ړ���Z��葽���Ƃ�)���X�������炷�B
			{
				reduction_store += reduction_volume * corrected_decimal;
				reduction_x = true;
				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(-delta_position.x * corrected_decimal);
			}
		}
		else // �v���C���[�ƃX�e�[�W�̋������X�e�[�W�̔��a�ƃv���C���[�̔��a�̍��v���Z���ƁB
		{
			break;
		}

		// ���Z�������炵�Ă��鎞�B
		if (reduction_volume < reduction_volume2)
		{
			// ���炵�Ă���ړ��ʂ̍��v���ő�̈ړ��ʂ�葽���ƃ��[�v���甲����B
			if (reduction_store >= reduction_volume2)
			{
				// ��Ɍ��炵���ق��̒l��ۑ����Ă����B
				reduction_volume = delta_position.z;
				break;
			}
		}
		else if (reduction_volume > reduction_volume2)// ���X�������炵�Ă��鎞�B
		{
			// ���炵�Ă���ړ��ʂ̍��v���ő�̈ړ��ʂ�葽���ƃ��[�v���甲����B
			if (reduction_store >= reduction_volume)
			{
				// ��Ɍ��炵���ق��̒l��ۑ����Ă����B
				reduction_volume = delta_position.x;
				break;
			}
		}
		else
		{
			break;
		}
	}
	reduction_store = 0.0f;

	while (true)
	{
		position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
		//vec = XMLoadFloat3(&XMFLOAT3(stage_position.x - position.x, stage_position.y - position.y, stage_position.z - position.z));
		vec = DirectX::SimpleMath::Vector3{ stage_position.x - position.x, stage_position.y - position.y, stage_position.z - position.z };
		XMStoreFloat(&length, XMVector3Length(vec));
		// ���������B
		if (length > move_limit_length)
		{
			// �����ň��ڂ̃��[�v�Ő�Ɍ��炵���������ǂ��������m�F����B
			if (reduction_x == true) // ���X������������B
			{
				if (delta_position.z < 0)
				{
					reduction_store += (delta_position.z * corrected_decimal) * -1;
				}
				else if (delta_position.z > 0)
				{
					reduction_store += delta_position.z * corrected_decimal;
				}

				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(-delta_position.z * corrected_decimal);
			}
			else if (reduction_x == false) // ���Z������������B
			{
				if (delta_position.x < 0)
				{
					reduction_store += (delta_position.x * corrected_decimal) * -1;
				}
				else if (delta_position.x > 0)
				{
					reduction_store += delta_position.x * corrected_decimal;
				}

				GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(-delta_position.x * 0.01f);
			}
		}
		else
		{
			break;
		}

		float reduction_volume2 = 0; // �ő�l��r�p�ɕϐ������B

		if (reduction_x == true)
		{
			reduction_volume2 = delta_position.z;
			// ��r���邽�߂Ƀv���X�ɂ��낦�Ă����B
			if (reduction_volume2 < 0)
			{
				reduction_volume2 *= -1;
			}
			if (reduction_store >= reduction_volume2)
			{
				break;
			}
		}
		else if (reduction_x == false)
		{
			reduction_volume2 = delta_position.x;
			// ��r���邽�߂Ƀv���X�ɂ��낦�Ă����B
			if (reduction_volume2 < 0)
			{
				reduction_volume2 *= -1;
			}
			if (reduction_store >= reduction_volume2)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	//// �ړ�������������B
	//position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	//vec = XMLoadFloat3(&XMFLOAT3(stage_position.x - position.x, stage_position.y - position.y, stage_position.z - position.z));
	//XMStoreFloat(&length, XMVector3Length(vec));
	//// ���������B
	//move_limit_length = stage_radius + radius;
	//if (length > move_limit_length)
	//{
	//	//delta_position = DirectX::XMFLOAT3(position.x - old_position.x, position.y - old_position.y, position.z - old_position.z);
	//	// X���������␳����B
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(-delta_position.x);
	//}
	//
	//// �ړ�������������B
	//position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	//vec = XMLoadFloat3(&XMFLOAT3(stage_position.x - position.x, stage_position.y - position.y, stage_position.z - position.z));
	//XMStoreFloat(&length, XMVector3Length(vec));
	//// ���������B
	//move_limit_length = stage_radius + radius;
	//if (length > move_limit_length)
	//{
	//	//delta_position = DirectX::XMFLOAT3(position.x - old_position.x, position.y - old_position.y, position.z - old_position.z);
	//	// Z���������␳����B
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(-delta_position.z);
	//}

	position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	old_position = position;

	//if (!std::isfinite(position.x))
	//{
	//	assert(!"PositionX is nan");
	//}
	//if (std::isinf(position.y))
	//{
	//	assert(!"PositionY is nan");
	//}if (std::isinf(position.z))
	//{
	//	assert(!"PositionZ is nan");
	//}

	old_hp = hp;

	//// ���Z����(�e�X�g�p)
	//if (key_input::Trg(InputType::Key::KEY_H))
	//{
	//	player_state.SetState(PlayerState::Stay);
	//	GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
	//	hp = 10;
	//	GetEntity()->GetComponent<PlayerDamageCollisionComponent>()->IsJudge(true);
	//}

	//GetEntity()->GetComponent<PlayerJustEvasionEffectComponent>()->PlayUseVariables();
	//GetEntity()->GetComponent<PlayerJustEvasionEffectComponent>()->UpdatePostion();
	//position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	//GetEntity()->GetComponent<PlayerAttackMagicEffectComponent>()->my_position = position;
	//GetEntity()->GetComponent<PlayerAttackMagicEffectComponent>()->PlayUseVariables();
}

void PlayerComponent::PlayerStay(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	if (is_play_anime == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
	}

	velocity = 0.0f;
	adjust_angle.y = 0.0f;
	StateInit();
}

void PlayerComponent::PlayerMove(const float delta_time)
{
	// �G�t�F�N�g�B
	GetEntity()->GetComponent<PlayerDashEffectComponent>()->PlayLoop();

	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	// 1���ڂ̑����̍Đ��B
	if (keyframe == 4)
	{
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::FootSteps)->Play(false);
	}
	// 2���ڂ̑����̍Đ��B
	if (keyframe == 21)
	{
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::FootSteps)->Play(false);
	}

	if (is_play_anime == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Move), true, 0.1f);
	}
}

void PlayerComponent::PlayerStop(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();

	if (is_play_anime == false)
	{
		player_state.SetState(PlayerState::Stay);
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
	}
}

void PlayerComponent::PlayerEvasion(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();

	// �^�C�}�[�͉��ŁA���ۂ̓A�j���[�V�������I������Ƃ��ɔ�����悤�ɂ���B
	evasion_time += 1 * delta_time;

	// �d���̃A�j���[�V�������I�������Ƃ��B
	if (is_play_anime == false && stiffness_flag == true)
	{
		evasion_time = 0;
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
	}

	// ����̃A�j���[�V�����������Ă���Ƃ��B
	if (is_play_anime == true && stiffness_flag == false)
	{
		// �����ňꎞ�I�Ƀ_���[�W�p�̓����蔻��������B
		GetEntity()->GetComponent<PlayerDamageCollisionComponent>()->IsJudge(false);
	}

	// ����̃A�j���[�V������0.2�b�Ԃ����������B
	if (is_play_anime == true && stiffness_flag == false && evasion_time < 0.2f)
	{
		// ����̈ړ��B
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(evasion_speed * sinf(angle.y * 0.01745f));
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(evasion_speed * cosf(angle.y * 0.01745f));
	}

	// ����̃A�j���[�V�������I�����čd�����Ă��Ȃ��Ƃ��B
	if (is_play_anime == false && stiffness_flag == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stop), false, 0.1f);
		stiffness_flag = true;
		just_evasion_flag = false;

		// �G�t�F�N�g(�ʏ��)�B
		GetEntity()->GetComponent<PlayerEvasionEffectComponent>()->Stop();
		// �G�t�F�N�g(�ʏ��)�B
		GetEntity()->GetComponent<PlayerJustEvasionEffectComponent>()->Stop();
	}

	// �����œG�Ƃ̋����𑪂��āA�G���U�����Ă���State�ɓ����Ă�����W���X�g����̃G�t�F�N�g���Đ�����B
	position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	XMFLOAT3 player_enemy = XMFLOAT3(enemy_position.x - position.x, enemy_position.y - position.y, enemy_position.z - position.z);

	XMVECTOR player_enemy_v = XMVector3Length(XMLoadFloat3(&player_enemy));
	float length = 0.0f;
	XMStoreFloat(&length, player_enemy_v);

	// �����œG�̃R���|�[�l���g����State�̏�Ԃ��Ƃ��Ă��ĕۑ�����B
	int enemy_state = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<EnemyComponent>()->GetEnemyState();
	int tutorial_enemy_state = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<EnemyComponent>()->GetTutorialEnemyState();

	// �����œG�Ƃ̋������߂��G���U�������Ă�����W���X�g����̃t���O���I���ɂ���B

	//static_cast<EnemyComponent::EnemyStates>(enemy_state) >= EnemyComponent::EnemyStates::Scratch &&
		//static_cast<EnemyComponent::EnemyStates>(enemy_state) <= EnemyComponent::EnemyStates::Lightning_end ||
		//length < 100 &&
		//static_cast<EnemyComponent::TutorialEnemyStates>(tutorial_enemy_state) == EnemyComponent::TutorialEnemyStates::Tutorial_Scratch)

	if (length < 100)
	{
		if (static_cast<EnemyComponent::EnemyStates>(enemy_state) == EnemyComponent::EnemyStates::Scratch ||
			static_cast<EnemyComponent::EnemyStates>(enemy_state) == EnemyComponent::EnemyStates::Rush ||
			static_cast<EnemyComponent::EnemyStates>(enemy_state) == EnemyComponent::EnemyStates::Bite ||
			static_cast<EnemyComponent::TutorialEnemyStates>(tutorial_enemy_state) == EnemyComponent::TutorialEnemyStates::Tutorial_Scratch)
		{
			if (stiffness_flag == false && just_evasion_flag == false)
			{
				if (player_state.GetOldState() != PlayerState::Damage)
				{
					just_evasion_flag = true;
					// ������1�x�����G�t�F�N�g���Đ�����B
					GetEntity()->GetComponent<PlayerJustEvasionEffectComponent>()->Stop();
					GetEntity()->GetComponent<PlayerJustEvasionEffectComponent>()->Play();
					// �W���X�g����̉��Đ��B
					GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Dodge)->Play(false);
				}
			}
		}
	}

	if (just_evasion_flag == true)
	{
		GetEntity()->GetComponent<PlayerJustEvasionEffectComponent>()->UpdatePostion();
	}
}

void PlayerComponent::PlayerJump(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();

	if (is_play_anime == false && jump_start_flag == true)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Jump), true, 0.5f);
		jump_start_flag = false;
	}

	// �^�C�}�[�͉��ŁA���ۂ̓A�j���[�V�������I������Ƃ��ɔ�����悤�ɂ���B
	//jump_time++;
	jump_time += 1 * delta_time;

	if (jump_time > 25 * delta_time)
	//if (jump_force - jump_time / (delta_time / 2) < 0)
	{
		jump_time = 0;
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Fall), true, 0.1f);
		player_state.SetState(PlayerState::Fall);
		//GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(AnimeState::Stay, true, 1.0f);
		//player_state.SetState(PlayerState::Stay);
	}
	if (jump_start_flag == false)
	{
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionY(jump_force);
		//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionY(jump_force - jump_time / (delta_time / 2));
	}
}

void PlayerComponent::PlayerLanding(const float delta_time)
{
	if (player_state.GetInitialize())
	{
		velocity = 0.0f;
	}

	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (is_play_anime == false || velocity > 0)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
	}
}

void PlayerComponent::PlayerDamage(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();

	// �����ňꎞ�I�Ƀ_���[�W�p�̓����蔻��������B
	GetEntity()->GetComponent<PlayerDamageCollisionComponent>()->IsJudge(false);

	// �^�C�}�[�͉��ŁA���ۂ̓A�j���[�V�������I������Ƃ��ɔ�����悤�ɂ���B
	damage_time += 1 * delta_time;

	blow_away -= 0.12f;

	if (blow_away < 0)
	{
		blow_away = 0;
	}

	// �v���C���[��front�x�N�g���B
	DirectX::XMFLOAT3 front = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
	DirectX::XMVECTOR front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)); // ���ςƂ����Ƃ��͕K�����K���B

	XMVECTOR enemy_front_v = XMLoadFloat3(&enemy_front);

	XMVECTOR cos_sita_v = XMVector3Dot(front_v, enemy_front_v);
	float cos_sita = 0.0f;
	XMStoreFloat(&cos_sita, cos_sita_v);

	float difference = 0.0f;
	if (cos_sita > 1.0f)
	{
		difference = fabsf(1.0f - cos_sita);
		if (difference > 0)
		{
			cos_sita = 1.0f;
		}
	}
	else if (cos_sita < -1.0f)
	{
		difference = fabsf(1.0f + cos_sita);
		if (difference > 0)
		{
			cos_sita = -1.0f;
		}
	}

	float blow_angle = acosf(cos_sita);
	blow_angle /= 0.01745f;

	// �G��front�x�N�g�����E�������𔻒肵�����̂œG�̃x�N�g������v���C���[�̃x�N�g���ɑ΂��Ă�
	// �O�ς��Ƃ��ăv���C���[�̃x�N�g�������E�ǂ����������ʂ̃x�N�g����Y�����̃v���X�}�C�i�X�Ŕ��f����B
	DirectX::XMVECTOR cross_v = DirectX::XMVector3Cross(enemy_front_v, front_v);
	DirectX::XMFLOAT3 cross;
	DirectX::XMStoreFloat3(&cross, cross_v);

	if (once_adjust_flag == false)
	{
		if (cross.y < 0)
		{
			blow_angle -= 180;
			once_adjust_flag = true;
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_Y(blow_angle);
			angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
			if (!std::isfinite(angle.y))
			{
				assert(!"Angle is nan");
			}
		}
		else
		{
			blow_angle += 180;
			once_adjust_flag = true;
			GetEntity()->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_Y(-blow_angle);
			angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
			if (!std::isfinite(angle.y))
			{
				assert(!"Angle is nan");
			}
		}

	}

	//cos_sita = (1.0f - cos_sita) * 120;

	if (cross.y < 0)
	{
		//cos_sita -= 180 * 0.01745f;
		//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(cos_sita);
		angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
		if (!std::isfinite(angle.y))
		{
			assert(!"Angle is nan");
		}
	}
	else
	{
		//cos_sita += 180 * 0.01745f;
		//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita);
		angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
		if (!std::isfinite(angle.y))
		{
			assert(!"Angle is nan");
		}
	}

	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(blow_away * sinf(blow_angle * 0.01745f));
	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(blow_away * cosf(blow_angle * 0.01745f));
	//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX((enemy_front.x * blow_away) * sinf(angle.y * 0.01745f));
	//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ((enemy_front.z * blow_away) * cosf(angle.y * 0.01745f));

	//DirectX::XMVECTOR length_v = DirectX::XMVector3Length(enemy_player_v);
	//float length = 0.0f;
	//DirectX::XMStoreFloat(&length, length_v);

	//if (damage_time > 16 * delta_time)
	if (is_play_anime == false)
	{
		damage_time = 0;
		//blow_away = 16;
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
	}

	//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(-blow_away * sinf(angle.y * 0.01745f));
	//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(-blow_away * cosf(angle.y * 0.01745f));
}

void PlayerComponent::PlayerAttack01(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (attack_hit_flag == false)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
	}

	if (keyframe > 12 && attack_effect_flag == false)
	{
		attack_effect_flag = true;
		// �G�t�F�N�g�B
		GetEntity()->GetComponent<PlayerAttack01EffectComponent>()->Stop();
		GetEntity()->GetComponent<PlayerAttack01EffectComponent>()->Play();
	}

	if (keyframe > 29)
	{
		GetEntity()->GetComponent<PlayerAttack01EffectComponent>()->Stop();
	}

	// ���̍U���̓��͂������Ă���ꍇ�̓A�j���[�V�����̓r���Ő؂�ւ���B
	if (next_attack_flag == true && keyframe > 21)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Attack02), false, 0.1f);
		player_state.SetState(PlayerState::Attack02);
		attack_hit_flag = false;
		StateInit();
		// �U�����Đ��B
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Slash)->Play(false);
	}

	// �U���̃A�j���[�V�����̐U�肪�I������Ƃ��ɓ����蔻��������B
	if (next_attack_flag == false && keyframe > 29)
	{
		attack_finish_flag = true;
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
	}

	// ���̍U��������Ȃ��܂܃A�j���[�V�������I������ꍇ�U���̃X�e�[�g���I����B
	if (is_play_anime == false && next_attack_flag == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
		attack_hit_flag = false;
		next_attack_flag = false;
		attack_finish_flag = false;
		attack_time = 0;
	}

	// ������11�t���[���������B
	//int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (keyframe <= 11)
	{
		// �ړ��B
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(attack_move_speed * sinf(angle.y * 0.01745f));
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(attack_move_speed * cosf(angle.y * 0.01745f));
	}

	// ���̍U���̓��͂̓��͂͂��̃A�b�v�f�[�g�̓��ڂ���ɂ���B
	if (GetEntity()->GetScene()->GetPadLink()->Trg(InputType::GamePad::Pad_X, 0) &&
		attack_time > 0)
	{
		next_attack_flag = true;
	}

	// �^�C�}�[�̓A�j���[�V�����̃t���[���̃J�E���g�p�B
	attack_time += delta_time;
}
void PlayerComponent::PlayerAttack02(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (attack_hit_flag == false)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
	}

	if (keyframe > 10 && attack_effect_flag == false)
	{
		attack_effect_flag = true;
		// �G�t�F�N�g�B
		GetEntity()->GetComponent<PlayerAttack02EffectComponent>()->Stop();
		GetEntity()->GetComponent<PlayerAttack02EffectComponent>()->Play();
	}

	if (keyframe > 40)
	{
		GetEntity()->GetComponent<PlayerAttack02EffectComponent>()->Stop();
	}

	// ���̍U���̓��͂������Ă���ꍇ�̓A�j���[�V�����̓r���Ő؂�ւ���B
	if (next_attack_flag == true && keyframe > 20)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Attack03_1), false, 0.1f);
		player_state.SetState(PlayerState::Attack03_1);
		attack_hit_flag = false;
		StateInit();
		// �U�����Đ��B
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Slash)->Play(false);
	}

	if (next_attack_flag == false && keyframe > 40)
	{
		attack_finish_flag = true;
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
	}

	// ���̍U��������Ȃ��܂܃A�j���[�V�������I������ꍇ�U���̃X�e�[�g���I����B
	if (is_play_anime == false && next_attack_flag == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
		attack_hit_flag = false;
		attack_finish_flag = false;
		next_attack_flag = false;
		attack_time = 0;
	}

	// �����Ō��̃|�W�V�������Ƃ��Ă���B
	//XMFLOAT3 sword_position = ;

	// �����Ō��ƓG�œ����蔻����Ƃ�B
	//bool hit_flag = false;
	//if (hit_flag == true && attack_hit_flag == false)
	//{
	//	// �����œG��HP�����炷�B
	//	attack_hit_flag = true;
	//}

	// ���̍U���̓��͂̓��͂͂��̃A�b�v�f�[�g�̓��ڂ���ɂ���B
	if (GetEntity()->GetScene()->GetPadLink()->Trg(InputType::GamePad::Pad_X, 0) &&
		attack_time > 0)
	{
		next_attack_flag = true;
	}

	// �^�C�}�[�̓A�j���[�V�����̃t���[���̃J�E���g�p�B
	attack_time += delta_time;
}
void PlayerComponent::PlayerAttack03_1(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (attack_hit_flag == false)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
	}

	if (keyframe > 9 && attack_effect_flag == false)
	{
		attack_effect_flag = true;
		// �G�t�F�N�g�B
		GetEntity()->GetComponent<PlayerAttack03EffectComponent>()->Stop();
		GetEntity()->GetComponent<PlayerAttack03EffectComponent>()->Play();
	}

	if (keyframe > 14)
	{
		GetEntity()->GetComponent<PlayerAttack03EffectComponent>()->Stop();
	}

	if (keyframe == 10)
	{
		// �U�����Đ��B
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Slash)->Play(false);
	}

	//�@�A�j���[�V�����̓r���Ő؂�ւ���B
	if (is_play_anime == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Attack03_2), false, 0.0f);
		player_state.SetState(PlayerState::Attack03_2);
		attack_hit_flag = false;
		StateInit();
	}

	//// �ړ��B
	//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(attack_move_speed * sinf(angle.y * 0.01745f));
	//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(attack_move_speed * cosf(angle.y * 0.01745f));

	// �����Ō��ƓG�œ����蔻����Ƃ�B
	//bool hit_flag = false;
	//if (hit_flag == true && attack_hit_flag == false)
	//{
	//	// �����œG��HP�����炷�B
	//	attack_hit_flag = true;
	//}
}

void PlayerComponent::PlayerAttack03_2(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (attack_hit_flag == false)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
	}

	if (keyframe > 5 && attack_effect_flag == false)
	{
		attack_effect_flag = true;
		// �G�t�F�N�g�B
		GetEntity()->GetComponent<PlayerAttack04EffectComponent>()->Stop();
		GetEntity()->GetComponent<PlayerAttack04EffectComponent>()->Play();
	}

	if (keyframe > 25)
	{
		GetEntity()->GetComponent<PlayerAttack04EffectComponent>()->Stop();
	}

	// ���̍U���̓��͂������Ă���ꍇ�̓A�j���[�V�����̓r���Ő؂�ւ���B
	if (next_attack_flag == true && attack_time > 0.2f)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Attack04_1), false, 0.1f);
		player_state.SetState(PlayerState::Attack04_1);
		attack_hit_flag = false;
		StateInit();
		// �U�����Đ��B
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Slash)->Play(false);
	}

	if (next_attack_flag == false && attack_time > 0.5f)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
		attack_finish_flag = true;
	}

	// ���̍U��������Ȃ��܂܃A�j���[�V�������I������ꍇ�U���̃X�e�[�g���I����B
	if (is_play_anime == false && next_attack_flag == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
		attack_hit_flag = false;
		attack_finish_flag = false;
		next_attack_flag = false;
		attack_time = 0;
	}

	// ������5�t���[�������������B
	if (keyframe <= 5)
	{
		// �ړ��B
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(attack_move_speed * sinf(angle.y * 0.01745f));
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(attack_move_speed * cosf(angle.y * 0.01745f));
	}
	// �����Ō��ƓG�œ����蔻����Ƃ�B
	//bool hit_flag = false;
	//if (hit_flag == true && attack_hit_flag == false)
	//{
	//	// �����œG��HP�����炷�B
	//	attack_hit_flag = true;
	//}

	// ���̍U���̓��͂̓��͂͂��̃A�b�v�f�[�g�̓��ڂ���ɂ���B
	if (GetEntity()->GetScene()->GetPadLink()->Trg(InputType::GamePad::Pad_X, 0) &&
		attack_time > 0)
	{
		next_attack_flag = true;
	}

	// �^�C�}�[�̓A�j���[�V�����̃t���[���̃J�E���g�p�B
	attack_time += delta_time;
}

void PlayerComponent::PlayerAttack04_1(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (attack_hit_flag == false)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
	}

	if (keyframe > 12 && attack_effect_flag == false)
	{
		attack_effect_flag = true;
		// �G�t�F�N�g�B
		GetEntity()->GetComponent<PlayerAttack05EffectComponent>()->Stop();
		GetEntity()->GetComponent<PlayerAttack05EffectComponent>()->Play();
	}

	if (keyframe > 17)
	{
		GetEntity()->GetComponent<PlayerAttack05EffectComponent>()->Stop();
	}

	// �A�j���[�V�����̓r���Ő؂�ւ���B
	if (is_play_anime == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Attack04_2), false, 0.1f);
		player_state.SetState(PlayerState::Attack04_2);
		attack_hit_flag = false;
		StateInit();
		// ���@�U�����Đ��B
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Wind)->Play(false);
	}

	// ������5�t���[�������������B
	if (keyframe <= 5)
	{
		// �ړ��B
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(attack_move_speed * sinf(angle.y * 0.01745f));
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(attack_move_speed * cosf(angle.y * 0.01745f));
	}
	// �����Ō��ƓG�œ����蔻����Ƃ�B
	//bool hit_flag = false;
	//if (hit_flag == true && attack_hit_flag == false)
	//{
	//	// �����œG��HP�����炷�B
	//	attack_hit_flag = true;
	//}
}

void PlayerComponent::PlayerAttack04_2(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();

	if (attack_hit_flag == false)
	{
		//GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
		GetEntity()->GetComponent<PlayerMagicAttackCollisionComponent>()->IsJudge(true);
		//position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
		//GetEntity()->GetComponent<PlayerAttackMagicEffectComponent>()->my_position = position;

		// �G�t�F�N�g�B
		GetEntity()->GetComponent<PlayerAttackMagicEffectComponent>()->PlayLoop();
	}

	// ���̍U���̓��͂������Ă���ꍇ�̓A�j���[�V�����̓r���Ő؂�ւ���B
	if (next_attack_flag == true && attack_time > 0.27f)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Attack05_1), false, 0.1f);
		player_state.SetState(PlayerState::Attack05_1);
		attack_hit_flag = false;
		StateInit();
		GetEntity()->GetComponent<PlayerAttackMagicEffectComponent>()->Stop();
		// �U�����Đ��B
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Slash)->Play(false);
	}

	if (next_attack_flag == false && attack_time > 0.5f)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
		attack_finish_flag = true;
		GetEntity()->GetComponent<PlayerAttackMagicEffectComponent>()->Stop();
	}

	// ���̍U��������Ȃ��܂܃A�j���[�V�������I������ꍇ�U���̃X�e�[�g���I����B
	if (is_play_anime == false && next_attack_flag == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
		GetEntity()->GetComponent<PlayerAttackMagicEffectComponent>()->Stop();
		attack_hit_flag = false;
		attack_finish_flag = false;
		next_attack_flag = false;
		attack_time = 0;
	}

	//// �ړ��B
	//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(attack_move_speed * sinf(angle.y * 0.01745f));
	//GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(attack_move_speed * cosf(angle.y * 0.01745f));

	// �����Ō��ƓG�œ����蔻����Ƃ�B
	//bool hit_flag = false;
	//if (hit_flag == true && attack_hit_flag == false)
	//{
	//	// �����œG��HP�����炷�B
	//	attack_hit_flag = true;
	//}

	// ���̍U���̓��͂̓��͂͂��̃A�b�v�f�[�g�̓��ڂ���ɂ���B
	if (GetEntity()->GetScene()->GetPadLink()->Trg(InputType::GamePad::Pad_X, 0) &&
		attack_time > 0)
	{
		next_attack_flag = true;
	}

	// �^�C�}�[�̓A�j���[�V�����̃t���[���̃J�E���g�p�B
	attack_time += delta_time;
}

void PlayerComponent::PlayerAttack05_1(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (attack_hit_flag == false)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
	}

	if (keyframe > 12 && attack_effect_flag == false)
	{
		attack_effect_flag = true;
		// �G�t�F�N�g�B
		GetEntity()->GetComponent<PlayerAttack06EffectComponent>()->Stop();
		GetEntity()->GetComponent<PlayerAttack06EffectComponent>()->Play();
	}

	if (keyframe > 19)
	{
		GetEntity()->GetComponent<PlayerAttack06EffectComponent>()->Stop();
	}

	// �A�j���[�V�����̓r���Ő؂�ւ���B
	if (is_play_anime == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Attack05_2), false, 0.1f);
		player_state.SetState(PlayerState::Attack05_2);
		attack_hit_flag = false;
		StateInit();
		// �U�����Đ��B
		GetEntity()->GetScene()->GetSoundResourceManager()->GetPlayerSound(Player_SE::Slash)->Play(false);
	}

	// ������8�t���[���o������11�t���[���������B
	if (keyframe > 8 && keyframe <= 19)
	{
		// �ړ��B
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(attack_move_speed * sinf(angle.y * 0.01745f));
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(attack_move_speed * cosf(angle.y * 0.01745f));
	}

}

void PlayerComponent::PlayerAttack05_2(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	if (attack_hit_flag == false)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
	}

	if (keyframe > 18 && attack_effect_flag == false)
	{
		attack_effect_flag = true;
		// �G�t�F�N�g�B
		GetEntity()->GetComponent<PlayerAttack07EffectComponent>()->Stop();
		GetEntity()->GetComponent<PlayerAttack07EffectComponent>()->Play();
	}

	if (keyframe > 41)
	{
		GetEntity()->GetComponent<PlayerAttack07EffectComponent>()->Stop();
	}

	if (next_attack_flag == false && attack_time > 0.5f)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
		attack_finish_flag = true;
	}

	// ���̍U��������Ȃ��܂܃A�j���[�V�������I������ꍇ�U���̃X�e�[�g���I����B
	if (is_play_anime == false && next_attack_flag == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
		attack_finish_flag = false;
		attack_hit_flag = false;
		next_attack_flag = false;
		attack_time = 0;
	}

	// ������17�t���[���������B
	if (keyframe <= 17)
	{
		// �ړ��B
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(attack_move_speed * sinf(angle.y * 0.01745f));
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(attack_move_speed * cosf(angle.y * 0.01745f));
	}

	// �^�C�}�[�̓A�j���[�V�����̃t���[���̃J�E���g�p�B
	attack_time += delta_time;
}

void PlayerComponent::PlayerCounter(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();

	if (attack_hit_flag == false)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(true);
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->SetRadius(70, 0);
	}

	// �����ňꎞ�I�Ƀ_���[�W�p�̓����蔻��������B
	GetEntity()->GetComponent<PlayerDamageCollisionComponent>()->IsJudge(false);

	// �^�C�}�[�͉��ŁA���ۂ̓A�j���[�V�������I������Ƃ��ɔ�����悤�ɂ���B
	counter_time += 1 * delta_time;

	//// �����Ō��̃|�W�V�������Ƃ��Ă���B
	////XMFLOAT3 sword_position = ;
	//
	//// �����Ō��ƓG�œ����蔻����Ƃ�B
	//bool counter_hit_flag = false;
	//if (counter_hit_flag == true && attack_hit_flag == false)
	//{
	//	// �����œG��HP�����炷�B
	//	//attack_hit_flag = true;
	//}

	// �v���C���[��front�x�N�g���B
	DirectX::XMFLOAT3 front = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
	DirectX::XMVECTOR front_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)); // ���ςƂ����Ƃ��͕K�����K���B

	// �����Ńv���C���[����G�ւ̃x�N�g�������B
	position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	XMFLOAT3 player_enemy = XMFLOAT3(enemy_position.x - position.x, enemy_position.y - position.y, enemy_position.z - position.z);
	DirectX::XMVECTOR player_enemy_v = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&player_enemy)); // ���ςƂ����Ƃ��͕K�����K���B

	// front�x�N�g�����m�̓��ς����߂āAacos����ς̌��ʂɂ����ă�(���W�A��)�����߂�B
	DirectX::XMVECTOR data = DirectX::XMVector3Dot(front_v, player_enemy_v);
	float cos_sita = 0.0f;
	DirectX::XMStoreFloat(&cos_sita, data);

	cos_sita = (1.0f - cos_sita) * speed_angle_y;

	// �J������front�x�N�g�����E�������𔻒肵�����̂ŃJ�����̃x�N�g������v���C���[�̃x�N�g���ɑ΂��Ă�
	// �O�ς��Ƃ��ăv���C���[�̃x�N�g�������E�ǂ����������ʂ̃x�N�g����Y�����̃v���X�}�C�i�X�Ŕ��f����B
	DirectX::XMVECTOR cross_v = DirectX::XMVector3Cross(player_enemy_v, front_v);
	DirectX::XMFLOAT3 cross;
	DirectX::XMStoreFloat3(&cross, cross_v);

	if (cross.y < 0)
	{
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(cos_sita);
		angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
		if (!std::isfinite(angle.y))
		{
			assert(!"Angle is nan");
		}
	}
	else
	{
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustWorldRotation_Y(-cos_sita);
		angle = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorldRotation();
		if (!std::isfinite(angle.y))
		{
			assert(!"Angle is nan");
		}
	}

	if (counter_time > 0.5f)
	{
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
		attack_finish_flag = true;
	}

	//if (counter_time > 60 * delta_time)
	if (is_play_anime == false)
	{
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Stay), true, 0.1f);
		player_state.SetState(PlayerState::Stay);
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::Sword)->GetComponent<SwordCollisionComponent>()->IsJudge(false);
		attack_hit_flag = false;
		counter_time = 0;
	}
}

void PlayerComponent::PlayerFall(const float delta_time)
{
	jump_time = -1;
}

void PlayerComponent::PlayerDeath(const float delta_time)
{
	// �����ňꎞ�I�Ƀ_���[�W�p�̓����蔻��������B
	GetEntity()->GetComponent<PlayerDamageCollisionComponent>()->IsJudge(false);

	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();
	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();
	// �^�C�}�[�͉��ŁA���ۂ̓A�j���[�V�������I������Ƃ��ɔ�����悤�ɂ���B
	if (keyframe < 27)
	{
		damage_time += 1 * delta_time;
		blow_away -= 0.12f;
	}

	if (blow_away < 0 || keyframe >= 27)
	{
		blow_away = 0;
		// GAME_OVER
		GetEntity()->GetScene()->GetOtherEntity(EntityTag::GameEnd)->GetComponent<GameEndComponent>()->Activate(GameEndComponent::EndState::Over);
	}

	if (is_play_anime == true)
	{
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(-blow_away * sinf(angle.y * 0.01745f));
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(-blow_away * cosf(angle.y * 0.01745f));
	}
}

void PlayerComponent::PlayerJumpDash(const float delta_time)
{
	bool is_play_anime = GetEntity()->GetComponent<FbxModelComponent>()->IsPlayAnimation();

	int keyframe = GetEntity()->GetComponent<FbxModelComponent>()->CurrentKeyframe();

	// �^�C�}�[�͉��ŁA���ۂ̓A�j���[�V�������I������Ƃ��ɔ�����悤�ɂ���B
	jump_dash_time += 1 * delta_time;

	//if (jump_dash_time > 10 * delta_time)
	if (keyframe >= 21)
	{
		jump_dash_time = 0;
		jump_dash_flag = true;
		GetEntity()->GetComponent<FbxModelComponent>()->SwitchAnimation(static_cast<int>(AnimeState::Fall), false, 0.3f);
		player_state.SetState(PlayerState::Fall);
	}

	//if (is_play_anime == true && jump_dash_time < 0.2f)
	if (keyframe < 21)
	{
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionX(jump_dash_speed * sinf(angle.y * 0.01745f));
		GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AjustPositionZ(jump_dash_speed * cosf(angle.y * 0.01745f));
	}
	else
	{
		int a = 0;
	}
}

void PlayerComponent::RenderImgui()
{
	ActorComponent::RenderImgui();

	if (ImGui::TreeNode("Player Parameter"))
	{
		float left_stick = pad_input::GetAnalogStickVal(0, PadInput::AnalogStickType::Left_Y_Thumb);
		float right_angle = 0;
		float left_angle = 0;

		if (left_stick < 0)
		{
			right_angle = 90 * (-1 - left_stick);
			left_angle = -90 * (-1 - left_stick);
		}
		else if (left_stick > 0)
		{
			right_angle = 90 * (1 - left_stick);
			left_angle = -90 * (1 - left_stick);
		}

		// �����œG�Ƃ̋����𑪂��āA�G���U�����Ă���State�ɓ����Ă�����W���X�g����̃G�t�F�N�g���Đ�����B
		position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
		XMFLOAT3 player_enemy = XMFLOAT3(enemy_position.x - position.x, enemy_position.y - position.y, enemy_position.z - position.z);

		XMVECTOR player_enemy_v = XMVector3Length(XMLoadFloat3(&player_enemy));
		float length = 0.0f;
		XMStoreFloat(&length, player_enemy_v);

		std::string hp_string = "HP ";
		std::string test_1 = hp_string + std::to_string(hp);
		std::string forward_string = "Forward Move ";
		std::string test_2 = forward_string + std::to_string(forward_move_flag);
		std::string back_string = "Back Move ";
		std::string test_3 = back_string + std::to_string(back_move_flag);
		std::string velocity_string = "Velocity ";
		std::string test_4 = velocity_string + std::to_string(velocity);
		std::string stick_string = "Stick Num ";
		std::string test5 = stick_string + std::to_string(left_stick);
		std::string left_string = "Angle_L ";
		std::string test6 = left_string + std::to_string(left_angle);
		std::string right_string = "Angle_R ";
		std::string test7 = right_string + std::to_string(right_angle);
		std::string blow_string = "Blow ";
		std::string test8 = blow_string + std::to_string(blow_away);
		std::string just_evasion_string = "JustEvasion ";
		std::string test9 = just_evasion_string + std::to_string(just_evasion_flag);
		std::string length_string = "Length ";
		std::string test10 = length_string + std::to_string(length);

		// �v���C���[��front�x�N�g���B
		DirectX::XMFLOAT3 front_3 = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetModelFront();
		DirectX::XMVECTOR front_v = DirectX::XMLoadFloat3(&front_3);
		float front[3] = { front_3.x, front_3.y, front_3.z };
		// �J������front�x�N�g���B
		DirectX::XMFLOAT3 camera_front_3 = GetEntity()->GetScene()->GetView()->GetCameraFront();
		DirectX::XMVECTOR camera_front_v = DirectX::XMLoadFloat3(&camera_front_3);
		float camera_front[3] = { camera_front_3.x, camera_front_3.y, camera_front_3.z };

		DirectX::XMVECTOR data = DirectX::XMVector3Dot(front_v, camera_front_v);
		float cos_sita = 0.0f;
		DirectX::XMStoreFloat(&cos_sita, data);

		char* hp_char = test_1.data();
		char* forward_char = test_2.data();
		char* back_char = test_3.data();
		char* velocity_char = test_4.data();
		char* stick_char = test5.data();
		char* left_char = test6.data();
		char* right_char = test7.data();
		char* blow_char = test8.data();
		char* just_evasion_char = test9.data();
		char* length_char = test10.data();

		ImGui::Text(str_player_states.at(static_cast<u_int>(player_state.GetState())).c_str());
		ImGui::Text(hp_char);
		ImGui::Text(forward_char);
		ImGui::Text(back_char);
		ImGui::Text(velocity_char);
		ImGui::Text(stick_char);
		ImGui::Text(left_char);
		ImGui::Text(right_char);
		ImGui::Text(blow_char);
		ImGui::SliderFloat3("PlayerFront", front, -1000, 1000);
		ImGui::SliderFloat3("CamerarFront", camera_front, -1000, 1000);
		ImGui::SliderFloat("cos_sita", &cos_sita, -1000, 1000);
		ImGui::Text(just_evasion_char);
		ImGui::Text(length_char);

		ImGui::TreePop();
	}

}
