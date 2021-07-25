#include "player_component.h"

#include "locator.h"
#include "cum_imgui_helper.h"
#include "ecs.h"

#include "arithmetic.h"
#include "camera_component.h"
#include "fbx_model_component.h"
#include "raycast_component.h"
#include "scene.h"
#include "transform_component.h"

// プレイヤーアニメーションのキーフレーム調整値
namespace adjust_key_frame
{

} // adjust_slot

namespace cumulonimbus::component
{
	PlayerComponent::PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:Actor3DComponent{registry, ent}
	{
		// stateに応じての処理の登録
		player_state.AddState(PlayerState::T_Pose					    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).TPose(dt); });
		player_state.AddState(PlayerState::Idle						    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Idle(dt); });
		player_state.AddState(PlayerState::Walk_Front				    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).WalkFront(dt); });
		player_state.AddState(PlayerState::Walk_Back				    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).WalkBack(dt); });
		player_state.AddState(PlayerState::Dash						    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Dash(dt); });
		player_state.AddState(PlayerState::Avoid_Dash_Begin			    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AvoidDashBegin(dt); });
		player_state.AddState(PlayerState::Avoid_Dash_End			    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AvoidDashEnd(dt); });
		player_state.AddState(PlayerState::Damage_Small				    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DamageSmall(dt); });
		player_state.AddState(PlayerState::Damage_Big				    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DamageBig(dt); });
		player_state.AddState(PlayerState::Jump_Begin				    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).JumpBegin(dt); });
		player_state.AddState(PlayerState::Jump_Loop			        , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).JumpLoop(dt); });
		player_state.AddState(PlayerState::Jump_Landing				    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).JumpLanding(dt); });
		player_state.AddState(PlayerState::Jump_End					    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).JumpEnd(dt); });
		player_state.AddState(PlayerState::Knock_Down_Front_Loop	    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).KnockDownFrontLoop(dt); });
		player_state.AddState(PlayerState::Knock_Down_Front_Stand_Up    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).KnockDownFrontStandUp(dt); });
		player_state.AddState(PlayerState::Die						    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Die(dt); });
		player_state.AddState(PlayerState::Attacking_Normal_01		    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormal01(dt); });
		player_state.AddState(PlayerState::Attacking_Normal_02		    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormal02(dt); });
		player_state.AddState(PlayerState::Attacking_Normal_03		    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormal03(dt); });
		player_state.AddState(PlayerState::Attacking_Normal_04		    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormal04(dt); });
		player_state.AddState(PlayerState::Attack_Normal_01_End		    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormal01End(dt); });
		player_state.AddState(PlayerState::Attack_Normal_02_End		    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormal02End(dt); });
		player_state.AddState(PlayerState::Attack_Normal_04_Begin	    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormal04Begin(dt); });
		player_state.AddState(PlayerState::Attack_Normal_04_End		    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormal04End(dt); });
		player_state.AddState(PlayerState::Attacking_Normal_Long_Press  , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormalLongPress(dt); });
		player_state.AddState(PlayerState::Attack_Normal_Long_Press_End , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormalLongPressEnd(dt); });
		player_state.AddState(PlayerState::Attacking_Strong_01			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingStrong01(dt); });
		player_state.AddState(PlayerState::Attacking_Strong_02			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingStrong02(dt); });
		player_state.AddState(PlayerState::Attacking_Strong_03			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingStrong03(dt); });
		player_state.AddState(PlayerState::Attacking_Strong_04			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingStrong04(dt); });
		player_state.AddState(PlayerState::Attack_Round_Up_Begin	    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackRoundUpBegin(dt); });
		player_state.AddState(PlayerState::Attacking_Round_Up    	    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingRoundUp(dt); });
		player_state.AddState(PlayerState::Attack_Round_Up_Fall			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackRoundUpFall(dt); });
		player_state.AddState(PlayerState::Attack_Round_Up_Fall			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackRoundUpEnd(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_01			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJump01(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_02			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJump02(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_03			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJump03(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_04			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJump04(dt); });
		player_state.AddState(PlayerState::Attack_Jump_01_End			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackJump01End(dt); });
		player_state.AddState(PlayerState::Attack_Jump_01_End			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackJump02End(dt); });
		player_state.AddState(PlayerState::Attack_Jump_01_End			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackJump03End(dt); });
		player_state.AddState(PlayerState::Attack_Jump_01_End			, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackJump04End(dt); });
		player_state.AddState(PlayerState::Attacking_Jump_Strong		, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingJumpStrong(dt); });
		player_state.AddState(PlayerState::Dash_Attack				    , [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DashAttack(dt); });

		// 初期stateの設定(Idle)
		player_state.SetState(PlayerState::Idle);

		// レイキャストに関する設定
		if(!registry->TryGetComponent<RayCastComponent>(ent))
		{
			registry->AddComponent<RayCastComponent>(ent, CollisionTag::Player);
		}
		registry->GetComponent<RayCastComponent>(ent).SetRayOffset({ 0.0f,0.0f,0.0f });

		// カメラに関する設定
		if(!registry->TryGetComponent<CameraComponent>(ent))
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
		// 現プレイヤーのstate処理を実行
		player_state.Update(dt);

		// レイキャストに使用するパラメータの設定
		SetRayCastParameter(dt);
		// カメラワーク
		CameraWork();
		// 移動
		Movement(dt);

		// テスト
		//velocity.y -= dt * 2;
	}

	void PlayerComponent::RenderImGui()
	{
		//using namespace locator;
		//DirectX::XMFLOAT2 stick = Locator::GetInput()->GamePad().LeftThumbStick(0);

		//ImGui::Text("Stick X : %f", stick.x);
		//ImGui::Text("Stick Y : %f", stick.y);

		if(ImGui::TreeNode("PlayerComponent"))
		{
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
		ray_cast_comp->SetRayEndPos(ray_start + DirectX::SimpleMath::Vector3{velocity.x* dt, -1.f, velocity.z* dt});
	}

	void PlayerComponent::Movement(float dt)
	{
		// 移動方向の決定
		const DirectX::XMFLOAT2 stick_left = locator::Locator::GetInput()->GamePad().LeftThumbStick(0);
		SimpleMath::Vector3 direction{ stick_left.x ,.0f,stick_left.y };
		direction.Normalize();
		const SimpleMath::Vector3 z_front{ .0f,.0f,1.f };
		const float dot = direction.Dot(z_front);

		if(stick_left.x < 0 && stick_left.y <0)
		{
			int a;
			a = 0;
		}

		float ep = FLT_EPSILON;
		float val = (dot - 0.0f);
		bool f = (val <= FLT_EPSILON);
		bool flg = arithmetic::IsEqual(dot, 0.0f);

		if (arithmetic::IsEqual(dot, 0.0f))
			return;

		SimpleMath::Vector3 camera_front = GetRegistry()->GetComponent<CameraComponent>(GetEntity()).GetCameraFront();
		SimpleMath::Matrix rotation_mat = SimpleMath::Matrix::Identity;
		rotation_mat = rotation_mat.CreateRotationY(dot);
		SimpleMath::Vector3::Transform(camera_front, rotation_mat, direction);

		if (player_state.GetState() == PlayerState::Walk_Front ||
			player_state.GetState() == PlayerState::Walk_Back)
		{
			//velocity.x = direction.x * walk_speed;
			//velocity.z = direction.z * walk_speed;
		}

		velocity.x = direction.x * walk_speed;
		velocity.z = direction.z * walk_speed;

		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		transform_comp.AdjustPosition(velocity * dt);
	}

	void PlayerComponent::CameraWork()
	{
		using namespace locator;
		const float rad_x = Locator::GetInput()->GamePad().RightThumbStick(0).x;
		const float rad_y = Locator::GetInput()->GamePad().RightThumbStick(0).y;
		auto& camera_comp = GetRegistry()->GetComponent<CameraComponent>(GetEntity());
		camera_comp.RotationFrontVectorFromUpVector(rad_x * camera_comp.GetCameraSpeed().x);
		camera_comp.RotationFrontVectorFromRightVector(rad_y * camera_comp.GetCameraSpeed().y);
	}


	bool PlayerComponent::IsNextAnimationLongPressAttack() const
	{
		const float frame_rate	= GetRegistry()->GetScene()->GetFramework()->GetHighResolutionTimer().GetFrameRate();
		const float fps			= 1 / frame_rate;
		return long_press_time > (fps * static_cast<float>(long_press_slot)) ? true : false;
	}


	void PlayerComponent::TPose(const float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationState::Idle)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::T_Pose), true);
		}

		if(!GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
		{// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
	}

	void PlayerComponent::Idle(float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationState::Idle)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Idle), true);
		}

		using namespace locator;
		const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);

		if (stick_left.x > threshold || stick_left.y > threshold)
		{// 状態遷移(PlayerState::Walk)
			player_state.SetState(PlayerState::Walk_Front);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 状態遷移(PlayerState::Attack_Normal_01)
			player_state.SetState(PlayerState::Attacking_Normal_01);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 状態遷移(PlayerState::Attack_Strong_01)
			player_state.SetState(PlayerState::Attacking_Strong_01);
		}
		else if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::A))
		{// 状態遷移(PlayerState::Jump_Begin)
			player_state.SetState(PlayerState::Jump_Begin);
		}
	}

	void PlayerComponent::WalkFront(float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationState::Walk)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Walk_Front), true);
		}

		using namespace locator;
		// ゲームパッド入力値取得(スティック、トリガー)
		const DirectX::XMFLOAT2 stick_left	  = Locator::GetInput()->GamePad().LeftThumbStick(0);
		const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);

		{// アニメーション遷移
			if (stick_left.x < threshold && stick_left.y < threshold)
			{// 状態遷移(PlayerState::Idle)
				player_state.SetState(PlayerState::Idle);
			}

			if (trigger_right > threshold)
			{// 状態遷移(PlayerState::Avoid_Dash_Begin)
				player_state.SetState(PlayerState::Avoid_Dash_Begin);
			}

			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 状態遷移(PlayerState::Attack_Normal_01)
				player_state.SetState(PlayerState::Attacking_Normal_01);
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// 状態遷移(PlayerState::Attack_Strong_01)
				player_state.SetState(PlayerState::Attacking_Strong_01);
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
		{// アニメーションセット(AnimationState::Jump_Begin)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Jump_Begin), false);
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(PlayerState::Jump_Loop)
		player_state.SetState(PlayerState::Jump_Loop);
	}

	void PlayerComponent::JumpLoop(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());

		if (player_state.GetInitialize())
		{
			// アニメーションセット(AnimationState::Jump_Loop)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Jump_Loop), true);
			// ジャンプの速度設定
			velocity.y = 10;
		}

		if (velocity.y < 0)
		{// アニメーション遷移(PlayerState::Jump_Landing)
			player_state.SetState(PlayerState::Jump_Landing);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 状態遷移(PlayerState::Attacking_Jump_01)
			player_state.SetState(PlayerState::Attacking_Jump_01);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 状態遷移(PlayerState::Attacking_Jump_Strong)
			player_state.SetState(PlayerState::Attacking_Jump_Strong);
		}
	}

	void PlayerComponent::JumpLanding(float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationState::Jump_Landing)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Jump_Landing), false);
		}

		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		if (transform_comp.GetPosition().y <= 0)
		{
			transform_comp.SetPositionY(0);

			// 状態遷移(PlayerState::Jump_End)
			player_state.SetState(PlayerState::Jump_End);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 状態遷移(PlayerState::Attacking_Jump_01)
			player_state.SetState(PlayerState::Attacking_Jump_01);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 状態遷移(PlayerState::Attacking_Jump_Strong)
			player_state.SetState(PlayerState::Attacking_Jump_Strong);
		}
	}

	void PlayerComponent::JumpEnd(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationState::Jump_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Jump_End), false);
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(PlayerState::Idle)
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
		if(player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attacking_Normal_01)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Normal_01), false, 0.01f);
		}

		using namespace locator;
		if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attacking_Normal_02)
			precede_input = PlayerState::Attacking_Normal_02;
		}
		else if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_02)
			precede_input = PlayerState::Attacking_Strong_02;
		}

		if(ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 通常攻撃(弱長押し)用タイマーの計測
			long_press_time += dt;
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if(precede_input == PlayerState::End)
		{// 先行入力なし
			// 状態遷移(PlayerState::Attack_Normal_01_End)
			player_state.SetState(PlayerState::Attack_Normal_01_End);
		}
		else
		{// 先行入力あり
			// 状態遷移(先行入力値)
			player_state.SetState(precede_input);
		}

		if (IsNextAnimationLongPressAttack())
		{// 状態遷移(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AttackNormal01End(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attack_Normal_01_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_01_End), false);
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(PlayerState::Idle)
		player_state.SetState(PlayerState::Idle);
	}

	void PlayerComponent::AttackingNormal02(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attacking_Normal_02)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Normal_02), false);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attacking_Normal_03)
			precede_input = PlayerState::Attacking_Normal_03;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_03)
			precede_input = PlayerState::Attacking_Strong_03;
		}

		if (ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 通常攻撃(弱長押し)用タイマーの計測
			long_press_time += dt;
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if (precede_input == PlayerState::End)
		{// 先行入力なし
			// 状態遷移(PlayerState::Attack_Normal_02_End)
			player_state.SetState(PlayerState::Attack_Normal_02_End);
		}
		else
		{// 先行入力あり
			// 状態遷移(先行入力値)
			player_state.SetState(precede_input);
		}

		if (IsNextAnimationLongPressAttack())
		{// 状態遷移(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AttackNormal02End(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attack_Normal_02_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_02_End), false);
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(PlayerState::Idle)
		player_state.SetState(PlayerState::Idle);
	}

	void PlayerComponent::AttackingNormal03(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attacking_Normal_03)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Normal_03), false);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attack_Normal_04_Begin)
			precede_input = PlayerState::Attack_Normal_04_Begin;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_04)
			precede_input = PlayerState::Attacking_Strong_04;
		}

		if (ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 通常攻撃(弱長押し)用タイマーの計測
			long_press_time += dt;
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if (precede_input == PlayerState::End)
		{// 先行入力なし
			// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
		else
		{// 先行入力あり
			// 状態遷移(先行入力値)
			player_state.SetState(precede_input);
		}

		if (IsNextAnimationLongPressAttack())
		{// 状態遷移(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AttackNormal04Begin(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attack_Normal_04_Begin)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_04_Begin), false);
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(PlayerState::Attacking_Normal_04)
		player_state.SetState(PlayerState::Attacking_Normal_04);
	}

	void PlayerComponent::AttackingNormal04(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if(player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attacking_Normal_04)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Normal_04), false);
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(PlayerState::Attack_Normal_04_End)
		player_state.SetState(PlayerState::Attack_Normal_04_End);
	}

	void PlayerComponent::AttackNormal04End(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attack_Normal_04_End)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_Normal_04_End), false);
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		// 状態遷移(PlayerState::Idle)
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
		if(player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attacking_Strong_01)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Strong_01), false);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attacking_Normal_02)
			precede_input = PlayerState::Attacking_Normal_02;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_02)
			precede_input = PlayerState::Attacking_Strong_02;
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if (precede_input == PlayerState::End)
		{// 先行入力なし
			// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
		else
		{// 先行入力あり
			// 状態遷移(先行入力値)
			player_state.SetState(precede_input);
		}
	}

	void PlayerComponent::AttackingStrong02(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attacking_Strong_02)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Strong_02), false);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attacking_Normal_03)
			precede_input = PlayerState::Attacking_Normal_03;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_03)
			precede_input = PlayerState::Attacking_Strong_03;
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if (precede_input == PlayerState::End)
		{// 先行入力なし
			// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
		else
		{// 先行入力あり
			// 状態遷移(先行入力値)
			player_state.SetState(precede_input);
		}
	}

	void PlayerComponent::AttackingStrong03(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attacking_Strong_03)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Strong_03), false);
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attack_Normal_04_Begin)
			precede_input = PlayerState::Attack_Normal_04_Begin;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_04)
			precede_input = PlayerState::Attacking_Strong_04;
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;

		if (precede_input == PlayerState::End)
		{// 先行入力なし
			// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
		else
		{// 先行入力あり
			// 状態遷移(先行入力値)
			player_state.SetState(precede_input);
		}
	}

	void PlayerComponent::AttackingStrong04(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationState::Attacking_Strong_03)
			fbx_model_comp.SwitchAnimation(GetAnimStateIndex(AnimationState::Attacking_Strong_04), false);
		}

		// アニメーション再生中なら処理を中断
		if (fbx_model_comp.IsPlayAnimation())
			return;
		// 状態遷移(PlayerState::Idle)
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
	}

	void PlayerComponent::AttackJump01End(float dt)
	{
	}

	void PlayerComponent::AttackingJump02(float dt)
	{
	}

	void PlayerComponent::AttackJump02End(float dt)
	{
	}

	void PlayerComponent::AttackingJump03(float dt)
	{
	}

	void PlayerComponent::AttackJump03End(float dt)
	{
	}

	void PlayerComponent::AttackingJump04(float dt)
	{
	}

	void PlayerComponent::AttackJump04End(float dt)
	{
	}

	void PlayerComponent::AttackingJumpStrong(float dt)
	{
	}

	void PlayerComponent::AttackJumpStrongEnd(float dt)
	{
	}

	void PlayerComponent::AvoidDashBegin(float dt)
	{
		if(player_state.GetInitialize())
		{// アニメーションセット(AnimationState::Avoid_Dash_Begin)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Avoid_Dash_Begin), false);
		}

		// アニメーション再生中なら処理を中断
		if (GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
			return;

		using namespace locator;
		// ゲームパッド入力値取得(スティック、トリガー)
		const DirectX::XMFLOAT2 stick_left    = Locator::GetInput()->GamePad().LeftThumbStick(0);
		const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
		if(trigger_right < threshold)
		{//	状態遷移(PlayerState::Avoid_Dash_End)
			player_state.SetState(PlayerState::Avoid_Dash_End);
		}
		else
		{
			if (stick_left.x < threshold && stick_left.y < threshold)
			{// スティック入力なし
				// 状態遷移(PlayerState::Avoid_Dash_End)
				player_state.SetState(PlayerState::Avoid_Dash_End);
			}
			else
			{// スティック入力あり
				// 状態遷移(PlayerState::Dash)
				player_state.SetState(PlayerState::Dash);
			}
		}
	}

	void PlayerComponent::AvoidDashEnd(float dt)
	{
		if(player_state.GetInitialize())
		{// アニメーションセット(AnimationState::Avoid_Dash_End)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Avoid_Dash_End), false);
		}

		// アニメーション再生中なら処理を中断
		if (GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
			return;

		const DirectX::XMFLOAT2 stick_left = locator::Locator::GetInput()->GamePad().LeftThumbStick(0);

		if (stick_left.x > threshold || stick_left.y > threshold)
		{// 状態遷移(PlayerState::Walk)
			player_state.SetState(PlayerState::Walk_Front);
		}
		else
		{// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
	}


	void PlayerComponent::Dash(float dt)
	{
		if(player_state.GetInitialize())
		{// アニメーションセット(AnimationState::Dash)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Dash), true);
		}

		// ゲームパッド入力値取得(スティック、トリガー)
		using namespace locator;
		const DirectX::XMFLOAT2 stick_left    = Locator::GetInput()->GamePad().LeftThumbStick(0);
		const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
		if(trigger_right < threshold)
		{
			if (stick_left.x > threshold || stick_left.y > threshold)
			{// 状態遷移(PlayerState::Walk)
				player_state.SetState(PlayerState::Walk_Front);
			}
			else
			{// 状態遷移(PlayerState::Idle)
				player_state.SetState(PlayerState::Idle);
			}
		}
		else
		{
			if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 状態遷移(PlayerState::Dash_Attack)
				player_state.SetState(PlayerState::Dash_Attack);
			}
			if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::A))
			{// 状態遷移(PlayerState::Jump_Begin)
				player_state.SetState(PlayerState::Jump_Begin);
			}
		}
	}


	//void PlayerComponent::TPose(float dt)
	//{

	//}

	//void PlayerComponent::Idle(float dt)
	//{
	//	if(player_state.GetInitialize())
	//	{// アニメションセット(Idle)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Idle), true);
	//	}

	//	using namespace locator;
	//	const DirectX::XMFLOAT2 stick = Locator::GetInput()->GamePad().LeftThumbStick(0);
	//	if(stick.x > threshold || stick.y > threshold)
	//	{// Walk状態に遷移
	//		player_state.SetState(PlayerState::Walk);
	//	}

	//	if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//	{// Attack_01状態に遷移
	//		player_state.SetState(PlayerState::Attack_01);
	//	}

	//}

	//void PlayerComponent::Walk(float dt)
	//{
	//	if (player_state.GetInitialize())
	//	{// アニメーションセット(Walk)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Walk_Front), true);
	//	}

	//	using namespace locator;
	//	const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
	//	if(stick_left.x < threshold && stick_left.y < threshold)
	//	{// Idle状態に遷移
	//		player_state.SetState(PlayerState::Idle);
	//	}

	//	const float trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
	//	if(trigger_right > threshold)
	//	{// Run状態に遷移
	//		player_state.SetState(PlayerState::Run);
	//	}

	//	if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//	{// Attack_01状態に遷移
	//		player_state.SetState(PlayerState::Attack_01);
	//	}
	//}

	//void PlayerComponent::Run(float dt)
	//{
	//	using namespace locator;

	//	if (player_state.GetInitialize())
	//	{// アニメーションセット(Run)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Run), true);
	//	}

	//	if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//	{// Run_Attack状態に遷移
	//		player_state.SetState(PlayerState::Run_Attack);
	//	}

	//	const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
	//	if (stick_left.x < threshold && stick_left.y < threshold)
	//	{// Idle状態に遷移
	//		player_state.SetState(PlayerState::Idle);
	//	}
	//	else
	//	{
	//		const float trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
	//		if (trigger_right < threshold)
	//		{// Walk状態に遷移
	//			player_state.SetState(PlayerState::Walk);
	//		}
	//	}
	//}

	//void PlayerComponent::RunAttack(float dt)
	//{
	//	if (player_state.GetInitialize())
	//	{// アニメーションセット(RunAttack)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Run_Attack), false, 0.01f);
	//	}

	//	// アニメーションが終われば
	//	if(!GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
	//	{// Walk状態に遷移
	//		player_state.SetState(PlayerState::Walk);
	//	}
	//}

	//void PlayerComponent::Evasion(float dt)
	//{
	//}

	//void PlayerComponent::Attack01(float dt)
	//{
	//	if(player_state.GetInitialize())
	//	{// アニメーションセット(Attack01)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_01), false, 0.01f);
	//	}

	//	// アニメーションが再生中か
	//	if(GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
	//	{
	//		if(ButtonState::Press == locator::Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//		{
	//			// Attack_02状態に遷移
	//			player_state.SetState(PlayerState::Attack_02);
	//		}
	//	}
	//	else
	//	{
	//		// Idle状態に遷移
	//		player_state.SetState(PlayerState::Idle);
	//	}
	//}

	//void PlayerComponent::Attack02(float dt)
	//{
	//	if(player_state.GetInitialize())
	//	{// アニメーションセット(Attack_02)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_02), false);
	//	}

	//	// アニメーションが再生中か
	//	if (GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
	//	{
	//		if (ButtonState::Press == locator::Locator::GetInput()->GamePad().GetState(GamePadButton::X))
	//		{
	//			// Attack_03状態に遷移
	//			player_state.SetState(PlayerState::Attack_03);
	//		}
	//	}
	//	else
	//	{
	//		// Idle状態に遷移
	//		player_state.SetState(PlayerState::Idle);
	//	}
	//}

	//void PlayerComponent::Attack03(float dt)
	//{
	//	if (player_state.GetInitialize())
	//	{// アニメーションセット(Attack_03)
	//		GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_03), false);
	//	}

	//	// アニメーションが再生中か
	//	if (!GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
	//	{
	//		// Idle状態に遷移
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
