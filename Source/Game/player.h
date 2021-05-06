#pragma once

#include <string>
#include <vector>

#include "ecs.h"
#include "actor_component.h"
#include "state_machine.h"

class PlayerComponent final :public ActorComponent
{
public:
	enum class PlayerState
	{
		Stay,
		Move,
		Stop,
		Evasion,
		Jump,
		Landing,
		Attack01,
		Attack02,
		Attack03_1,
		Attack03_2,
		Attack04_1,
		Attack04_2,
		Attack05_1,
		Attack05_2,
		Damage,
		Counter,
		Fall,
		Death,
		JumpDash,

		End,
	};

private:

	StateMachine<PlayerState, void, const float> player_state{};

	enum class AnimeState
	{
		Stay,
		Evasion,
		Move,
		Stop,
		JumpStart,
		Jump,
		Fall,
		Landing,
		Attack01,
		Attack02,
		Attack03_1,
		Attack03_2,
		Attack04_1,
		Attack04_2,
		Attack05_1,
		Attack05_2,
		Damage,
		Death,
		DeathState,
		Counter,
		//JumpDash,
	};

	//enum class MoveState
	//{
	//	Forward,
	//	Back,
	//	Right,
	//	Left,
	//};
	//
	//StateMachine<MoveState, void, const float> move_state{};

	DirectX::XMFLOAT3 angle;
	float old_sita;
	DirectX::XMFLOAT3 adjust_angle;

	DirectX::XMFLOAT3 old_position;

	const float speed = 2.5f;
	float velocity = 0.0f;
	float max_velocity = 3.0f;
	bool forward_move_flag = false;
	bool back_move_flag = false;
	const float speed_angle_y = 30.0f;

	const float evasion_speed = 7.5f;
	float evasion_time = 0;
	bool stiffness_flag = false;
	bool just_evasion_flag = false;

	const float jump_force = 12;
	bool graund_flag = false;
	float jump_time = 0;
	bool jump_start_flag = false;

	const float jump_dash_speed = 6.5f;
	float jump_dash_time = 0;
	bool jump_dash_flag = false;

	const int max_hp = 30;
	int hp = 10;
	int old_hp = 0;
	//bool damage_flag = false;
	float damage_time = 0;
	bool once_adjust_flag = false;
	float blow_away = 6.5f;

	bool attack_finish_flag = false;
	bool next_attack_flag = false;
	float attack_time = 0;
	bool attack_hit_flag = false;
	const float attack_move_speed = 0.3f;
	bool attack_effect_flag = false;

	//bool counter_flag = false;
	float counter_time = 0;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 delta_position;
	DirectX::XMFLOAT3 player_front;

	bool reduction_x = false;
	float radius = 18;
	DirectX::XMFLOAT3 enemy_position;
	DirectX::XMFLOAT3 enemy_front;

	DirectX::XMFLOAT3 stage_position;
	float stage_radius;

	// For Imgui
	std::vector<std::string> str_player_states;

private:

	void StateInit()
	{
		attack_effect_flag = false;
		evasion_time = 0;
		stiffness_flag = false;
		just_evasion_flag = false;
		jump_time = 0;
		jump_dash_time = 0;
		jump_start_flag = false;
		damage_time = 0;
		once_adjust_flag = false;
		blow_away = 6.5f;
		attack_time = 0;
		attack_finish_flag = false;
		attack_hit_flag = false;
		next_attack_flag = false;
		counter_time = 0;
	}

	void PlayerStay(const float delta_time);
	void PlayerMove(const float delta_time);
	void PlayerStop(const float delta_time);
	void PlayerEvasion(const float delta_time);
	void PlayerJump(const float delta_time);
	void PlayerLanding(const float delta_time);
	void PlayerDamage(const float delta_time);
	void PlayerAttack01(const float delta_time);
	void PlayerAttack02(const float delta_time);
	void PlayerAttack03_1(const float delta_time);
	void PlayerAttack03_2(const float delta_time);
	void PlayerAttack04_1(const float delta_time);
	void PlayerAttack04_2(const float delta_time);
	void PlayerAttack05_1(const float delta_time);
	void PlayerAttack05_2(const float delta_time);
	void PlayerCounter(const float delta_time);
	void PlayerFall(const float delta_time);
	void PlayerDeath(const float delta_time);
	void PlayerJumpDash(const float delta_time);

public:

	PlayerComponent(class Entity* entity, ActorType actor_type = ActorType::Actor3D);

	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	void RenderImgui() override;

	void SetVelocity(float speed, float delta_time)
	{
		velocity += speed * delta_time;
		if (max_velocity < velocity)
		{
			velocity = max_velocity;
		}
		else if (-max_velocity > velocity)
		{
			velocity = -max_velocity;
		}
	}

	const int GetPlayerState()
	{
		return static_cast<int>(player_state.GetState());
	}

	const void SubtractPlayerHp(int set_hp) // セッター
	{
		hp -= set_hp;
	}

	const int GetMaxHp() { return max_hp; }

	const void PlayerHp(int num) // ゲッター
	{
		hp = num;
	}

	const int PlayerHp() // ゲッター
	{
		return hp;
	}

	const int OldHp()
	{
		return old_hp;
	}

	const void AttackHitFlag(bool flag) // セッター
	{
		attack_hit_flag = flag;
	}
	const bool AttackHitFlag() // ゲッター
	{
		return attack_hit_flag;
	}

	void SetJustEvasionFlag(bool flag)
	{
		just_evasion_flag = flag;
	}
};