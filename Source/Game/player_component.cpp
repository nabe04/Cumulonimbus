#include "player_component.h"

#include "cum_imgui_helper.h"
#include "ecs.h"
#include "locator.h"

#include "arithmetic.h"
#include "camera_component.h"
#include "fbx_model_component.h"
#include "movement_component.h"
#include "raycast_component.h"
#include "scene.h"
#include "transform_component.h"

// �v���C���[�A�j���[�V�����̃L�[�t���[�������l
namespace adjust_key_frame
{

} // adjust_slot

namespace cumulonimbus::component
{
	PlayerComponent::PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:Actor3DComponent{ registry, ent }
	{
		// state�ɉ����Ă̏����̓o�^
		player_state.AddState(PlayerState::T_Pose, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).TPose(dt); });
		player_state.AddState(PlayerState::Idle, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Idle(dt); });
		player_state.AddState(PlayerState::Walk_Front, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).WalkFront(dt); });
		player_state.AddState(PlayerState::Walk_Back, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).WalkBack(dt); });
		player_state.AddState(PlayerState::Dash, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Dash(dt); });
		player_state.AddState(PlayerState::Avoid_Dash_Begin, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AvoidDashBegin(dt); });
		player_state.AddState(PlayerState::Avoid_Dash_End, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AvoidDashEnd(dt); });
		player_state.AddState(PlayerState::Damage_Small, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DamageSmall(dt); });
		player_state.AddState(PlayerState::Damage_Big, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DamageBig(dt); });
		player_state.AddState(PlayerState::Jump_Begin, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).JumpBegin(dt); });
		player_state.AddState(PlayerState::Jump_Loop, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).JumpLoop(dt); });
		player_state.AddState(PlayerState::Jump_Landing, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).JumpLanding(dt); });
		player_state.AddState(PlayerState::Jump_End, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).JumpEnd(dt); });
		player_state.AddState(PlayerState::Knock_Down_Front_Loop, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).KnockDownFrontLoop(dt); });
		player_state.AddState(PlayerState::Knock_Down_Front_Stand_Up, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).KnockDownFrontStandUp(dt); });
		player_state.AddState(PlayerState::Die, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Die(dt); });
		player_state.AddState(PlayerState::Attack_Normal_01, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormal01(dt); });
		player_state.AddState(PlayerState::Attack_Normal_02, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormal02(dt); });
		player_state.AddState(PlayerState::Attack_Normal_03, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormal03(dt); });
		player_state.AddState(PlayerState::Attacking_Normal_04, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormal04(dt); });
		player_state.AddState(PlayerState::Attack_Normal_04_Begin, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormal04Begin(dt); });
		player_state.AddState(PlayerState::Attack_Normal_04_End, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormal04End(dt); });
		player_state.AddState(PlayerState::Attacking_Normal_Long_Press, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormalLongPress(dt); });
		player_state.AddState(PlayerState::Attack_Normal_Long_Press_End, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormalLongPressEnd(dt); });
		player_state.AddState(PlayerState::Attack_Strong_01, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingStrong01(dt); });
		player_state.AddState(PlayerState::Attack_Strong_02, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingStrong02(dt); });
		player_state.AddState(PlayerState::Attack_Strong_03, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingStrong03(dt); });
		player_state.AddState(PlayerState::Attack_Strong_04, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingStrong04(dt); });
		player_state.AddState(PlayerState::Attack_Round_Up_Begin, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackRoundUpBegin(dt); });
		player_state.AddState(PlayerState::Attacking_Round_Up, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingRoundUp(dt); });
		player_state.AddState(PlayerState::Attack_Round_Up_Fall, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackRoundUpFall(dt); });
		player_state.AddState(PlayerState::Attack_Round_Up_Fall, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackRoundUpEnd(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_01, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJump01(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_02, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJump02(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_03, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJump03(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_04, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJump04(dt); });
		player_state.AddState(PlayerState::Attack_Jump_01_End, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackJump01End(dt); });
		player_state.AddState(PlayerState::Attack_Jump_01_End, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackJump02End(dt); });
		player_state.AddState(PlayerState::Attack_Jump_01_End, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackJump03End(dt); });
		player_state.AddState(PlayerState::Attack_Jump_01_End, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackJump04End(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_Strong, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJumpStrong(dt); });
		player_state.AddState(PlayerState::Dash_Attack, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DashAttack(dt); });

		// ����state�̐ݒ�(Idle)
		player_state.SetState(PlayerState::Idle);

		// �A�j���[�V�����̍ŏI�L�[�t���[���̐ݒ�
		SetAdjustKeyFrame("walk_front"			, 32);
		SetAdjustKeyFrame("avoid_dash_begin"	, 24);
		SetAdjustKeyFrame("avoid_dash_end"		, 10);
		SetAdjustKeyFrame("dash"				, 16);
		SetAdjustKeyFrame("attack_normal_01"	, 40);
		SetAdjustKeyFrame("attack_normal_02"	, 36);
		SetAdjustKeyFrame("attack_strong_01"	, 27);
		SetAdjustKeyFrame("attack_strong_02"	, 22);
		SetAdjustKeyFrame("attack_strong_03"	, 22);
		SetAdjustKeyFrame("attack_strong_04"	, 22);
		SetAdjustKeyFrame("jump_start"			, 13);
		SetAdjustKeyFrame("jump_loop"			, 46);
		SetAdjustKeyFrame("jump_end"			, 17);
		SetAdjustKeyFrame("attacking_jump_01"	, 16);
		SetAdjustKeyFrame("attacking_jump_02"	, 16);
		SetAdjustKeyFrame("attacking_jump_03"	, 18);
		SetAdjustKeyFrame("attacking_jump_04"	, 18);
		SetAdjustKeyFrame("attack_jump_01_end"	, 36);
		SetAdjustKeyFrame("attack_jump_02_end"	, 36);
		SetAdjustKeyFrame("attack_jump_03_end"	, 34);
		SetAdjustKeyFrame("attack_jump_04_end"	, 32);

		// ��s���͂ɂ��A�j���[�V�����̒��f�t���[���̐ݒ�
		SetAnimationBreakFrame(AnimationState::Attack_Normal_01 , 16);
		SetAnimationBreakFrame(AnimationState::Attack_Normal_02 , 20);
		SetAnimationBreakFrame(AnimationState::Attack_Normal_03 , 37);
		SetAnimationBreakFrame(AnimationState::Attacking_Jump_01, 17);

		// ���C�L���X�g�Ɋւ���ݒ�
		if (!registry->TryGetComponent<RayCastComponent>(ent))
		{
			registry->AddComponent<RayCastComponent>(ent, CollisionTag::Player);
		}
		registry->GetComponent<RayCastComponent>(ent).SetRayOffset({ 0.0f,0.0f,0.0f });

		// �J�����Ɋւ���ݒ�
		if (!registry->TryGetComponent<CameraComponent>(ent))
		{
			registry->AddComponent<CameraComponent>(ent, true);
		}
		registry->GetComponent<CameraComponent>(ent).SetCameraSpeed({ 0.3f,0.3f });
	}

	void PlayerComponent::NewFrame(float dt)
	{
		//GetRegistry()->GetComponent<TransformComponent>(GetEntity()).AdjustPositionY(-2.f * dt);
	}

	void PlayerComponent::Update(float dt)
	{
		// ���v���C���[��state���������s
		player_state.Update(dt);

		// ���C�L���X�g�Ɏg�p����p�����[�^�̐ݒ�
		SetRayCastParameter(dt);
		// �J�������[�N
		CameraWork();
		// �ړ�
		//Movement(dt);
		Rotation(dt);
	}

	void PlayerComponent::RenderImGui()
	{
		//using namespace locator;
		//DirectX::XMFLOAT2 stick = Locator::GetInput()->GamePad().LeftThumbStick(0);

		//ImGui::Text("Stick X : %f", stick.x);
		//ImGui::Text("Stick Y : %f", stick.y);

		if (ImGui::TreeNode("PlayerComponent"))
		{
			ImGui::DragFloat("Walk Speed", &walk_speed, 0.5f, 0.1f, 100000);
			ImGui::DragFloat("Dash Speed", &dash_speed, 0.5f, 0.1f, 100000);
			ImGui::DragFloat("Dead zone value of pad input", &threshold, 0.01f, 0.0f, 1.0f);

			ImGui::TreePop();
		}
	}

	void PlayerComponent::Save(const std::string& file_path)
	{

	}

	void PlayerComponent::Load(const std::string& file_path_and_name)
	{

	}

	int PlayerComponent::GetAnimStateIndex(AnimationState anim_state) const
	{
		return static_cast<int>(anim_state);
	}

	void PlayerComponent::SetRayCastParameter(const float dt)
	{
		auto* ray_cast_comp = GetRegistry()->TryGetComponent<RayCastComponent>(GetEntity());
		if (!ray_cast_comp)
			return;

		const auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		const DirectX::SimpleMath::Vector3 ray_start = transform_comp.GetPosition() + ray_cast_comp->GetRayOffset();
		ray_cast_comp->SetRayStartPos(ray_start);
		ray_cast_comp->SetRayEndPos(ray_start + DirectX::SimpleMath::Vector3{ velocity.x * dt, -1.f, velocity.z * dt });
	}

	void PlayerComponent::Movement(float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		transform_comp.AdjustPosition(velocity * dt);

		velocity = SimpleMath::Vector3{ 0,0,0 };
	}

	void PlayerComponent::Rotation(float dt)
	{
		const XMFLOAT2 stick_left = locator::Locator::GetInput()->GamePad().LeftThumbStick(0);

		if (IsDeadZone())
			return;

		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& camera_comp = GetRegistry()->GetComponent<CameraComponent>(GetEntity());

		SimpleMath::Vector3 stick_direction = { stick_left.x,0.0f,stick_left.y };
		stick_direction.Normalize();

		// ���f���̊��O���x�N�g��{0,0,1}�ƃX�e�B�b�N���͕����Ƃ̃x�N�g���̊p�x(���W�A��)���Z�o
		float rad = arithmetic::CalcAngleFromTwoVec(stick_direction, { 0,0,1 });
		// ��]����������X�e�B�b�N����(x�l)�ŕ␳
		if (stick_left.x < 0)
			rad *= -1;
		// �J�����̃t�����g�x�N�g����rad����]
		SimpleMath::Vector3 camera_xz_front_vec = camera_comp.GetCameraFront();
		const SimpleMath::Quaternion q = SimpleMath::Quaternion::CreateFromAxisAngle({ 0,1,0 }, rad);
		SimpleMath::Vector3::Transform(camera_xz_front_vec, q, camera_xz_front_vec);
		camera_xz_front_vec.y = 0;
		camera_xz_front_vec.Normalize();

		// ���f���̑O���x�N�g����front_vec�Ƃ̊p�x(���W�A��)���Z�o
		SimpleMath::Vector3 model_xz_front = transform_comp.GetModelFront();
		model_xz_front.y = 0;
		model_xz_front.Normalize();
		if (arithmetic::IsEqual(model_xz_front.x, camera_xz_front_vec.x) &&
			arithmetic::IsEqual(model_xz_front.y, camera_xz_front_vec.y) &&
			arithmetic::IsEqual(model_xz_front.z, camera_xz_front_vec.z))
			return;

		rad = arithmetic::CalcAngleFromTwoVec(camera_xz_front_vec, model_xz_front);
		const SimpleMath::Vector3 cross_vec = model_xz_front.Cross(camera_xz_front_vec);
		if (cross_vec.y < 0)
			rad *= -1;

		transform_comp.AdjustRotationFromAxis({ 0,1,0 }, rad);
	}

	void PlayerComponent::CameraWork()
	{
		using namespace locator;
		const float rad_x = Locator::GetInput()->GamePad().RightThumbStick(0).x;
		const float rad_y = Locator::GetInput()->GamePad().RightThumbStick(0).y;
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& camera_comp = GetRegistry()->GetComponent<CameraComponent>(GetEntity());
		camera_comp.RotationFrontVectorFromUpVector(rad_x * camera_comp.GetCameraSpeed().x);
		camera_comp.RotationFrontVectorFromRightVector(rad_y * camera_comp.GetCameraSpeed().y);
		camera_comp.SetFocusPosition(transform_comp.GetPosition());
	}

	void PlayerComponent::AdjustVelocity(float dt, const DirectX::SimpleMath::Vector3& speed)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		velocity.x += transform_comp.GetModelFront().x * speed.x * dt;
		velocity.y += 1 * speed.y * dt;
		velocity.z += transform_comp.GetModelFront().z * speed.z * dt;
	}

	void PlayerComponent::SetAdjustKeyFrame(const std::string& animation_name, u_int keyframe)
	{
		if (adjust_keyframe_map.contains(animation_name))
		{
			adjust_keyframe_map.at(animation_name) = keyframe;
			return;
		}

		// "animation_name"�����f���̃A�j���[�V�������ƈ�v���邩�̊m�F
		for (auto& animation : GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).GetResource()->GetModelData().animations)
		{
			if (animation_name == animation.animation_name)
			{
				adjust_keyframe_map.insert(std::make_pair(animation_name, keyframe));
				animation.num_key_frame = keyframe;
				animation.seconds_length = keyframe * animation.sampling_time;

				return;
			}
		}
		assert(!"No animation names existed(PlayerComponent::SetAdjustKeyFrame)");
	}

	void PlayerComponent::SetAnimationBreakFrame(AnimationState state, u_int keyframe)
	{
		// �}�b�v�ɑ��݂��Ă���΃L�[�t���[�����㏑������
		if (animation_break_frame.contains(state))
		{
			animation_break_frame.at(state) = keyframe;
			return;
		}

		animation_break_frame.insert(std::make_pair(state, keyframe));
	}

	bool PlayerComponent::IsNextAnimationLongPressAttack() const
	{
		const float frame_rate = GetRegistry()->GetScene()->GetFramework()->GetHighResolutionTimer().GetFrameRate();
		const float fps = 1 / frame_rate;
		return long_press_time > (fps * static_cast<float>(long_press_slot)) ? true : false;
	}

	bool PlayerComponent::IsDeadZone() const
	{
		bool ret_flg = true;
		const XMFLOAT2 stick_left = locator::Locator::GetInput()->GamePad().LeftThumbStick(0);

		if (fabs(stick_left.x) < threshold &&
			fabs(stick_left.y) < threshold)
			return true;

		return false;
	}

	bool PlayerComponent::IsBreakAnimationFrame(AnimationState state) const
	{
		if (!animation_break_frame.contains(state))
			assert((!"Don't have state(PlayerComponent::IsBreakAnimationFrame)"));

		const auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		return animation_break_frame.at(state) < fbx_model_comp.CurrentKeyframe() ? true : false;
	}


	void PlayerComponent::TPose(const float dt)
	{
		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Idle)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::T_Pose), true);
		}

		if (!GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
		{// ��ԑJ��(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
	}

	void PlayerComponent::Idle(float dt)
	{
		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Idle)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Idle), true);
		}

		using namespace locator;
		const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);

		{// �ړ����x�̐ݒ�
			//AdjustVelocity(dt, { .0f,.0f,.0f});
		}

		if (!IsDeadZone())
		{// ��ԑJ��(PlayerState::Walk)
			player_state.SetState(PlayerState::Walk_Front);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// ��ԑJ��(PlayerState::Attack_Normal_01)
			player_state.SetState(PlayerState::Attack_Normal_01);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// ��ԑJ��(PlayerState::Attack_Strong_01)
			player_state.SetState(PlayerState::Attack_Strong_01);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::A))
		{// ��ԑJ��(PlayerState::Jump_Begin)
			player_state.SetState(PlayerState::Jump_Begin);
		}
	}

	void PlayerComponent::WalkFront(float dt)
	{
		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Walk)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Walk_Front), true);
		}

		using namespace locator;
		// �Q�[���p�b�h���͒l�擾(�X�e�B�b�N�A�g���K�[)
		const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
		const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);

		{// �ړ����x�̐ݒ�
			//auto& physics_comp = GetRegistry()->GetComponent<PhysicsComponent>(GetEntity());
			//physics_comp.AddForce({ walk_speed,0.0f,walk_speed });
			auto& movement_comp = GetRegistry()->GetComponent<MovementComponent>(GetEntity());
			movement_comp.AddForce({ walk_speed,0.0f,walk_speed });
			//AdjustVelocity(dt, { walk_speed,0.0f,walk_speed });
		}

		{// �A�j���[�V�����J��
			if (IsDeadZone())
			{// ��ԑJ��(PlayerState::Idle)
				player_state.SetState(PlayerState::Idle);
			}

			if (trigger_right > threshold)
			{// ��ԑJ��(PlayerState::Avoid_Dash_Begin)
				player_state.SetState(PlayerState::Avoid_Dash_Begin);
			}

			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// ��ԑJ��(PlayerState::Attack_Normal_01)
				player_state.SetState(PlayerState::Attack_Normal_01);
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// ��ԑJ��(PlayerState::Attack_Strong_01)
				player_state.SetState(PlayerState::Attack_Strong_01);
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::A))
			{// ��ԑJ��(PlayerState::Jump_Begin)
				player_state.SetState(PlayerState::Jump_Begin);
			}
		}
	}

	void PlayerComponent::WalkBack(float dt)
	{
	}

	void PlayerComponent::KnockDownFrontLoop(float dt)
	{
	}

	void PlayerComponent::KnockDownFrontStandUp(float dt)
	{
	}

	void PlayerComponent::DamageSmall(float dt)
	{
	}

	void PlayerComponent::DamageBig(float dt)
	{
	}


	void PlayerComponent::JumpBegin(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());

		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Jump_Begin)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Jump_Begin), false);
		}

		if (fbx_model_comp.CurrentKeyframe() > 4)
		{
			auto& movement_comp = GetRegistry()->GetComponent<MovementComponent>(GetEntity());
			movement_comp.Jump();
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// ��ԑJ��(PlayerState::Jump_Loop)
		player_state.SetState(PlayerState::Jump_Loop);
	}

	void PlayerComponent::JumpLoop(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		auto& movement_comp = GetRegistry()->GetComponent<MovementComponent>(GetEntity());

		if (player_state.GetInitialize())
		{
			// �A�j���[�V�����Z�b�g(AnimationState::Jump_Loop)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Jump_Loop), true);
		}

		{// �ړ����x�̐ݒ�
			const SimpleMath::Vector2 stick_left = locator::Locator::GetInput()->GamePad().LeftThumbStick(0);
			movement_comp.AddForce({ stick_left.x * jump_movement_speed,0.0f,stick_left.y * jump_movement_speed });
		}

		{// �A�j���[�V�����J��

			if (movement_comp.GetCurrentGravity() < 0)
			{// �A�j���[�V�����J��(PlayerState::Jump_Landing)
				player_state.SetState(PlayerState::Jump_Landing);
			}

			using namespace locator;
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// ��ԑJ��(PlayerState::Attacking_Jump_01)
				player_state.SetState(PlayerState::Attacking_Jump_01);
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// ��ԑJ��(PlayerState::Attacking_Jump_Strong)
				player_state.SetState(PlayerState::Attacking_Jump_Strong);
			}
		}
	}

	void PlayerComponent::JumpLanding(float dt)
	{
		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Jump_Landing)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Jump_Landing), false);
		}

		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		if (transform_comp.GetPosition().y <= 0)
		{
			transform_comp.SetPositionY(0);

			// ��ԑJ��(PlayerState::Jump_End)
			player_state.SetState(PlayerState::Jump_End);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// ��ԑJ��(PlayerState::Attacking_Jump_01)
			player_state.SetState(PlayerState::Attacking_Jump_01);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// ��ԑJ��(PlayerState::Attacking_Jump_Strong)
			player_state.SetState(PlayerState::Attacking_Jump_Strong);
		}
	}

	void PlayerComponent::JumpEnd(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Jump_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Jump_End), false);
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// ��ԑJ��(PlayerState::Idle)
		player_state.SetState(PlayerState::Idle);
	}

	void PlayerComponent::Die(float dt)
	{
	}

	void PlayerComponent::DashAttack(float dt)
	{

	}

	void PlayerComponent::AttackingNormal01(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Normal_01)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_01), false, 0.01f);
		}

		using namespace locator;
		if (!IsBreakAnimationFrame(AnimationState::Attack_Normal_01))
		{
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// ��s���̓Z�b�g(PlayerState::Attacking_Normal_02)
				precede_input = PlayerState::Attack_Normal_02;
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// ��s���̓Z�b�g(PlayerState::Attacking_Strong_02)
				precede_input = PlayerState::Attack_Strong_02;
			}

			if (ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// �ʏ�U��(�㒷����)�p�^�C�}�[�̌v��
				long_press_time += dt;
			}

			// �A�j���[�V�����Đ����Ȃ珈���𒆒f
			return;
		}

		if ((!fbx_model_comp.IsPlayAnimation()))
		{// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}

		if ((precede_input != PlayerState::End))
		{
			if (IsBreakAnimationFrame(AnimationState::Attack_Normal_01))
			{// ��s���͂���
				// ��ԑJ��(��s���͒l)
				player_state.SetState(precede_input);
			}
		}

		if (IsNextAnimationLongPressAttack())
		{// ��ԑJ��(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AttackingNormal02(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Normal_02)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_02), false);
		}

		if (!IsBreakAnimationFrame(AnimationState::Attack_Normal_02))
		{
			using namespace locator;
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// ��s���̓Z�b�g(PlayerState::Attacking_Normal_03)
				precede_input = PlayerState::Attack_Normal_03;
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// ��s���̓Z�b�g(PlayerState::Attacking_Strong_03)
				precede_input = PlayerState::Attack_Strong_03;
			}

			if (ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// �ʏ�U��(�㒷����)�p�^�C�}�[�̌v��
				long_press_time += dt;
			}

			// �A�j���[�V�����Đ����Ȃ珈���𒆒f
			return;
		}

		if ((!fbx_model_comp.IsPlayAnimation()))
		{// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}

		if (precede_input != PlayerState::End)
		{
			if (IsBreakAnimationFrame(AnimationState::Attack_Normal_02))
			{// ��s���͂���
				// ��ԑJ��(��s���͒l)
				player_state.SetState(precede_input);
			}
		}

		if (IsNextAnimationLongPressAttack())
		{// ��ԑJ��(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AttackingNormal03(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Normal_03)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_03), false);
		}

		if (!IsBreakAnimationFrame(AnimationState::Attack_Normal_03))
		{
			using namespace locator;
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// ��s���̓Z�b�g(PlayerState::Attack_Normal_04_Begin)
				precede_input = PlayerState::Attack_Normal_04_Begin;
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// ��s���̓Z�b�g(PlayerState::Attacking_Strong_04)
				precede_input = PlayerState::Attack_Strong_04;
			}

			if (ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// �ʏ�U��(�㒷����)�p�^�C�}�[�̌v��
				long_press_time += dt;
			}

			// �A�j���[�V�����Đ����Ȃ珈���𒆒f
			return;
		}

		if ((!fbx_model_comp.IsPlayAnimation()))
		{// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}

		if (precede_input != PlayerState::End)
		{
			if (IsBreakAnimationFrame(AnimationState::Attack_Normal_03))
			{// ��s���͂���
				// ��ԑJ��(��s���͒l)
				player_state.SetState(precede_input);
			}
		}

		if (IsNextAnimationLongPressAttack())
		{// ��ԑJ��(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AttackNormal04Begin(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Normal_04_Begin)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_04_Begin), false);
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// ��ԑJ��(PlayerState::Attacking_Normal_04)
		player_state.SetState(PlayerState::Attacking_Normal_04);
	}

	void PlayerComponent::AttackingNormal04(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attacking_Normal_04)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Normal_04), false);
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// ��ԑJ��(PlayerState::Attack_Normal_04_End)
		player_state.SetState(PlayerState::Attack_Normal_04_End);
	}

	void PlayerComponent::AttackNormal04End(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Normal_04_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_04_End), false);
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// ��ԑJ��(PlayerState::Idle)
		player_state.SetState(PlayerState::Idle);
	}

	void PlayerComponent::AttackingNormalLongPress(float dt)
	{
	}

	void PlayerComponent::AttackNormalLongPressEnd(float dt)
	{
	}

	void PlayerComponent::AttackingStrong01(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Strong_01)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Strong_01), false);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// ��s���̓Z�b�g(PlayerState::Attacking_Normal_02)
			precede_input = PlayerState::Attack_Normal_02;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// ��s���̓Z�b�g(PlayerState::Attacking_Strong_02)
			precede_input = PlayerState::Attack_Strong_02;
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if (precede_input == PlayerState::End)
		{// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
		else
		{// ��s���͂���
			// ��ԑJ��(��s���͒l)
			player_state.SetState(precede_input);
		}
	}

	void PlayerComponent::AttackingStrong02(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Strong_02)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Strong_02), false);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// ��s���̓Z�b�g(PlayerState::Attacking_Normal_03)
			precede_input = PlayerState::Attack_Normal_03;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// ��s���̓Z�b�g(PlayerState::Attacking_Strong_03)
			precede_input = PlayerState::Attack_Strong_03;
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if (precede_input == PlayerState::End)
		{// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
		else
		{// ��s���͂���
			// ��ԑJ��(��s���͒l)
			player_state.SetState(precede_input);
		}
	}

	void PlayerComponent::AttackingStrong03(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Strong_03)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Strong_03), false);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// ��s���̓Z�b�g(PlayerState::Attack_Normal_04_Begin)
			precede_input = PlayerState::Attack_Normal_04_Begin;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// ��s���̓Z�b�g(PlayerState::Attacking_Strong_04)
			precede_input = PlayerState::Attack_Strong_04;
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if (precede_input == PlayerState::End)
		{// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
		else
		{// ��s���͂���
			// ��ԑJ��(��s���͒l)
			player_state.SetState(precede_input);
		}
	}

	void PlayerComponent::AttackingStrong04(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Strong_04)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Strong_04), false);
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (fbx_model_comp.IsPlayAnimation())
			return;
		// ��ԑJ��(PlayerState::Idle)
		player_state.SetState(PlayerState::Idle);

	}

	void PlayerComponent::AttackRoundUpBegin(float dt)
	{
	}

	void PlayerComponent::AttackingRoundUp(float dt)
	{
	}

	void PlayerComponent::AttackRoundUpFall(float dt)
	{
	}

	void PlayerComponent::AttackRoundUpEnd(float dt)
	{
	}

	void PlayerComponent::AttackingJump01(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		auto& movement_comp  = GetRegistry()->GetComponent<MovementComponent>(GetEntity());
		if(player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attacking_Jump_01)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Jump_01), false);
			// �d�͏��� Off(�󒆂Ŏ~�߂�)
			movement_comp.JumpStop(true);
		}

		using namespace locator;
		if (!IsBreakAnimationFrame(AnimationState::Attacking_Jump_01))
		{
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// ��s���̓Z�b�g(PlayerState::Attacking_Jump_02)
				precede_input = PlayerState::Attacking_Jump_02;
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// ��s���̓Z�b�g(PlayerState::Attack_Jumping_Strong_Begin)
				precede_input = PlayerState::Attack_Jumping_Strong_Begin;
			}
			return;
		}

		if ((!fbx_model_comp.IsPlayAnimation()))
		{// �A�j���[�V�����Đ��I��
			// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Attack_Jump_01_End)
			player_state.SetState(PlayerState::Attack_Jump_01_End);
		}

		if (precede_input != PlayerState::End)
		{
			if (IsBreakAnimationFrame(AnimationState::Attacking_Jump_01))
			{// ��s���͂���
				// ��ԑJ��(��s���͒l)
				player_state.SetState(precede_input);
			}
		}
	}

	void PlayerComponent::AttackJump01End(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Jump_01_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Jump_01_End), false);
		}

		if(!fbx_model_comp.IsPlayAnimation() ||
			IsBreakAnimationFrame(AnimationState::Attack_Jump_01_End))
		{
			// �d�͏��� On
			GetRegistry()->GetComponent<MovementComponent>(GetEntity()).JumpStop(false);
			// ��ԑJ��(PlayerState::Jump_Landing)
			player_state.SetState(PlayerState::Jump_Landing);
		}
	}

	void PlayerComponent::AttackingJump02(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if(player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attacking_Jump_02)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Jump_02), false);
		}

		using namespace locator;
		if (!IsBreakAnimationFrame(AnimationState::Attacking_Jump_02))
		{
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// ��s���̓Z�b�g(PlayerState::Attacking_Jump_03)
				precede_input = PlayerState::Attacking_Jump_03;
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// ��s���̓Z�b�g(PlayerState::Attack_Jumping_Strong_Begin)
				precede_input = PlayerState::Attack_Jumping_Strong_Begin;
			}
			return;
		}

		if ((!fbx_model_comp.IsPlayAnimation()))
		{// �A�j���[�V�����Đ��I��
			// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Attack_Jump_02_End)
			player_state.SetState(PlayerState::Attack_Jump_02_End);
		}

		if (precede_input != PlayerState::End)
		{
			if (IsBreakAnimationFrame(AnimationState::Attacking_Jump_02))
			{// ��s���͂���
				// ��ԑJ��(��s���͒l)
				player_state.SetState(precede_input);
			}
		}
	}

	void PlayerComponent::AttackJump02End(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Jump_02_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Jump_02_End), false);
		}

		if (!fbx_model_comp.IsPlayAnimation() ||
			IsBreakAnimationFrame(AnimationState::Attack_Jump_02_End))
		{
			// �d�͏��� On
			GetRegistry()->GetComponent<MovementComponent>(GetEntity()).JumpStop(false);
			// ��ԑJ��(PlayerState::Jump_Landing)
			player_state.SetState(PlayerState::Jump_Landing);
		}
	}

	void PlayerComponent::AttackingJump03(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if(player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attacking_Jump_03)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Jump_03), false);
		}

		using namespace locator;
		if(!IsBreakAnimationFrame(AnimationState::Attacking_Jump_03))
		{
			if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// ��s���̓Z�b�g(PlayerState::Attacking_Jump_04)
				precede_input = PlayerState::Attacking_Jump_04;
			}
			else if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// ��s���̓Z�b�g(PlayerState::Attack_Jumping_Strong_Begin)
				precede_input = PlayerState::Attack_Jumping_Strong_Begin;
			}
		}

		if ((!fbx_model_comp.IsPlayAnimation()))
		{// �A�j���[�V�����Đ��I��
			// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Attack_Jump_03_End)
			player_state.SetState(PlayerState::Attack_Jump_03_End);
		}

		if (precede_input != PlayerState::End)
		{
			if (IsBreakAnimationFrame(AnimationState::Attacking_Jump_03))
			{// ��s���͂���
				// ��ԑJ��(��s���͒l)
				player_state.SetState(precede_input);
			}
		}
	}

	void PlayerComponent::AttackJump03End(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Jump_03_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Jump_03_End), false);
		}

		if (!fbx_model_comp.IsPlayAnimation() ||
			IsBreakAnimationFrame(AnimationState::Attack_Jump_03_End))
		{
			// �d�͏��� On
			GetRegistry()->GetComponent<MovementComponent>(GetEntity()).JumpStop(false);
			// ��ԑJ��(PlayerState::Jump_Landing)
			player_state.SetState(PlayerState::Jump_Landing);
		}
	}

	void PlayerComponent::AttackingJump04(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attacking_Jump_04)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Jump_04), false);
		}

		if ((!fbx_model_comp.IsPlayAnimation()))
		{// �A�j���[�V�����Đ��I��
			// ��s���͂Ȃ�
			// ��ԑJ��(PlayerState::Attack_Jump_04_End)
			player_state.SetState(PlayerState::Attack_Jump_04_End);
		}

		if (IsBreakAnimationFrame(AnimationState::Attacking_Jump_04))
		{// ��s���͂���
			// ��ԑJ��(��s���͒l)
			player_state.SetState(PlayerState::Attack_Jump_04_End);
		}
	}

	void PlayerComponent::AttackJump04End(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// �A�j���[�V�����Z�b�g(AnimationState::Attack_Jump_04_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Jump_04_End), false);
		}

		if (!fbx_model_comp.IsPlayAnimation() ||
			IsBreakAnimationFrame(AnimationState::Attack_Jump_04_End))
		{
			// �d�͏��� On
			GetRegistry()->GetComponent<MovementComponent>(GetEntity()).JumpStop(false);
			// ��ԑJ��(PlayerState::Jump_Landing)
			player_state.SetState(PlayerState::Jump_Landing);
		}
	}

	void PlayerComponent::AttackingJumpStrong(float dt)
	{
	}

	void PlayerComponent::AttackJumpStrongEnd(float dt)
	{
	}

	void PlayerComponent::AvoidDashBegin(float dt)
	{
		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Avoid_Dash_Begin)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Avoid_Dash_Begin), false);
		}

		{// �ړ����x�̐ݒ�
			auto& movement_comp = GetRegistry()->GetComponent<MovementComponent>(GetEntity());
			movement_comp.AddForce({ avoid_dash_speed,0.0f,avoid_dash_speed });
			//AdjustVelocity(dt, { avoid_dash_speed,0.0f,avoid_dash_speed });
		}

		{// �A�j���[�V�����J��
			// �A�j���[�V�����Đ����Ȃ珈���𒆒f
			if (GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
				return;

			using namespace locator;
			// �Q�[���p�b�h���͒l�擾(�X�e�B�b�N�A�g���K�[)
			const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
			const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
			if (trigger_right < threshold)
			{//	��ԑJ��(PlayerState::Avoid_Dash_End)
				player_state.SetState(PlayerState::Avoid_Dash_End);
			}
			else
			{
				if (stick_left.x < threshold && stick_left.y < threshold)
				{// �X�e�B�b�N���͂Ȃ�
					// ��ԑJ��(PlayerState::Avoid_Dash_End)
					player_state.SetState(PlayerState::Avoid_Dash_End);
				}
				else
				{// �X�e�B�b�N���͂���
					// ��ԑJ��(PlayerState::Dash)
					player_state.SetState(PlayerState::Dash);
				}
			}
		}
	}

	void PlayerComponent::AvoidDashEnd(float dt)
	{
		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Avoid_Dash_End)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Avoid_Dash_End), false);
		}

		// �A�j���[�V�����Đ����Ȃ珈���𒆒f
		if (GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
			return;

		const DirectX::XMFLOAT2 stick_left = locator::Locator::GetInput()->GamePad().LeftThumbStick(0);

		if (stick_left.x > threshold || stick_left.y > threshold)
		{// ��ԑJ��(PlayerState::Walk)
			player_state.SetState(PlayerState::Walk_Front);
		}
		else
		{// ��ԑJ��(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
	}


	void PlayerComponent::Dash(float dt)
	{
		if (player_state.GetInitialize())
		{// �A�j���[�V�����Z�b�g(AnimationState::Dash)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Dash), true);
		}

		{// �ړ����x�̐ݒ�
			auto& movement_comp = GetRegistry()->GetComponent<MovementComponent>(GetEntity());
			movement_comp.AddForce({ dash_speed,0.0f,dash_speed });
			//AdjustVelocity(dt, { dash_speed,0.0f,dash_speed });
		}

		{// �A�j���[�V�����J��
			// �Q�[���p�b�h���͒l�擾(�X�e�B�b�N�A�g���K�[)
			using namespace locator;
			const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
			const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
			if (trigger_right < threshold)
			{
				if (stick_left.x > threshold || stick_left.y > threshold)
				{// ��ԑJ��(PlayerState::Walk)
					player_state.SetState(PlayerState::Walk_Front);
				}
				else
				{// ��ԑJ��(PlayerState::Idle)
					player_state.SetState(PlayerState::Idle);
				}
			}
			else
			{
				if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
				{// ��ԑJ��(PlayerState::Dash_Attack)
					player_state.SetState(PlayerState::Dash_Attack);
				}
				if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::A))
				{// ��ԑJ��(PlayerState::Jump_Begin)
					player_state.SetState(PlayerState::Jump_Begin);
				}
			}
		}
	}


	//void PlayerComponent::TPose(float dt)
	//{

	//}

	//void PlayerComponent::Idle(float dt)
	//{
	//	if(player_state.GetInitialize())
	//	{// �A�j���V�����Z�b�g(Idle)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Idle), true);
	//	}

	//	using namespace locator;
	//	const DirectX::XMFLOAT2 stick = Locator::GetInput()->GamePad().LeftThumbStick(0);
	//	if(stick.x > threshold || stick.y > threshold)
	//	{// Walk��ԂɑJ��
	//		player_state.SetState(PlayerState::Walk);
	//	}

	//	if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//	{// Attack_01��ԂɑJ��
	//		player_state.SetState(PlayerState::Attack_01);
	//	}

	//}

	//void PlayerComponent::Walk(float dt)
	//{
	//	if (player_state.GetInitialize())
	//	{// �A�j���[�V�����Z�b�g(Walk)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Walk_Front), true);
	//	}

	//	using namespace locator;
	//	const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
	//	if(stick_left.x < threshold && stick_left.y < threshold)
	//	{// Idle��ԂɑJ��
	//		player_state.SetState(PlayerState::Idle);
	//	}

	//	const float trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
	//	if(trigger_right > threshold)
	//	{// Run��ԂɑJ��
	//		player_state.SetState(PlayerState::Run);
	//	}

	//	if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//	{// Attack_01��ԂɑJ��
	//		player_state.SetState(PlayerState::Attack_01);
	//	}
	//}

	//void PlayerComponent::Run(float dt)
	//{
	//	using namespace locator;

	//	if (player_state.GetInitialize())
	//	{// �A�j���[�V�����Z�b�g(Run)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Run), true);
	//	}

	//	if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//	{// Run_Attack��ԂɑJ��
	//		player_state.SetState(PlayerState::Run_Attack);
	//	}

	//	const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
	//	if (stick_left.x < threshold && stick_left.y < threshold)
	//	{// Idle��ԂɑJ��
	//		player_state.SetState(PlayerState::Idle);
	//	}
	//	else
	//	{
	//		const float trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
	//		if (trigger_right < threshold)
	//		{// Walk��ԂɑJ��
	//			player_state.SetState(PlayerState::Walk);
	//		}
	//	}
	//}

	//void PlayerComponent::RunAttack(float dt)
	//{
	//	if (player_state.GetInitialize())
	//	{// �A�j���[�V�����Z�b�g(RunAttack)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Run_Attack), false, 0.01f);
	//	}

	//	// �A�j���[�V�������I����
	//	if(!GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
	//	{// Walk��ԂɑJ��
	//		player_state.SetState(PlayerState::Walk);
	//	}
	//}

	//void PlayerComponent::Evasion(float dt)
	//{
	//}

	//void PlayerComponent::Attack01(float dt)
	//{
	//	if(player_state.GetInitialize())
	//	{// �A�j���[�V�����Z�b�g(Attack01)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_01), false, 0.01f);
	//	}

	//	// �A�j���[�V�������Đ�����
	//	if(GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
	//	{
	//		if(ButtonState::Press == locator::Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//		{
	//			// Attack_02��ԂɑJ��
	//			player_state.SetState(PlayerState::Attack_02);
	//		}
	//	}
	//	else
	//	{
	//		// Idle��ԂɑJ��
	//		player_state.SetState(PlayerState::Idle);
	//	}
	//}

	//void PlayerComponent::Attack02(float dt)
	//{
	//	if(player_state.GetInitialize())
	//	{// �A�j���[�V�����Z�b�g(Attack_02)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_02), false);
	//	}

	//	// �A�j���[�V�������Đ�����
	//	if (GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
	//	{
	//		if (ButtonState::Press == locator::Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//		{
	//			// Attack_03��ԂɑJ��
	//			player_state.SetState(PlayerState::Attack_03);
	//		}
	//	}
	//	else
	//	{
	//		// Idle��ԂɑJ��
	//		player_state.SetState(PlayerState::Idle);
	//	}
	//}

	//void PlayerComponent::Attack03(float dt)
	//{
	//	if (player_state.GetInitialize())
	//	{// �A�j���[�V�����Z�b�g(Attack_03)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_03), false);
	//	}

	//	// �A�j���[�V�������Đ�����
	//	if (!GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
	//	{
	//		// Idle��ԂɑJ��
	//		player_state.SetState(PlayerState::Idle);
	//	}
	//}

	//void PlayerComponent::Damage(float dt)
	//{
	//}

	//void PlayerComponent::RevengeGuard(float dt)
	//{
	//}

	//void PlayerComponent::Death(float dt)
	//{
	//}
} // cumulonimbus::component
