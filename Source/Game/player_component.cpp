#include "player_component.h"

#include "locator.h"
#include "cum_imgui_helper.h"
#include "ecs.h"

#include "fbx_model_component.h"
#include "raycast_component.h"
#include "transform_component.h"

namespace cumulonimbus::component
{
	PlayerComponent::PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:Actor3DComponent{registry, ent}
	{
		// stateに応じての処理の登録
		player_state.AddState(PlayerState::T_Pose		, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).TPose(dt);			});
		player_state.AddState(PlayerState::Idle			, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Idle(dt);			});
		player_state.AddState(PlayerState::Walk			, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Walk(dt);			});
		player_state.AddState(PlayerState::Run			, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Run(dt);			});
		player_state.AddState(PlayerState::Evasion		, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Evasion(dt);		});
		player_state.AddState(PlayerState::Attack_01	, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Attack01(dt);		});
		player_state.AddState(PlayerState::Attack_02	, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Attack02(dt);		});
		player_state.AddState(PlayerState::Attack_03	, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Attack03(dt);		});
		player_state.AddState(PlayerState::Run_Attack	, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).RunAttack(dt);		});
		player_state.AddState(PlayerState::Damage		, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Damage(dt);		});
		player_state.AddState(PlayerState::Revenge_Guard, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).RevengeGuard(dt);	});
		player_state.AddState(PlayerState::Die			, [ent, registry](const float dt) { registry->GetComponent<PlayerComponent>(ent).Death(dt);			});
		// 初期stateの設定(Idle)
		player_state.SetState(PlayerState::Idle);

		// レイキャストに関する設定
		if(!registry->TryGetComponent<RayCastComponent>(ent))
		{
			registry->AddComponent<RayCastComponent>(ent, CollisionTag::Player);
		}
		registry->GetComponent<RayCastComponent>(ent).SetRayOffset({ 0.0f,0.0f,0.0f });
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

	void PlayerComponent::TPose(float dt)
	{

	}

	void PlayerComponent::Idle(float dt)
	{
		if(player_state.GetInitialize())
		{// アニメションセット(Idle)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Idle), true);
		}

		using namespace locator;
		const DirectX::XMFLOAT2 stick = Locator::GetInput()->GamePad().LeftThumbStick(0);
		if(stick.x > threshold || stick.y > threshold)
		{// Walk状態に遷移
			player_state.SetState(PlayerState::Walk);
		}

		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// Attack_01状態に遷移
			player_state.SetState(PlayerState::Attack_01);
		}

	}

	void PlayerComponent::Walk(float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(Walk)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Walk_Front), true);
		}

		using namespace locator;
		const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
		if(stick_left.x < threshold && stick_left.y < threshold)
		{// Idle状態に遷移
			player_state.SetState(PlayerState::Idle);
		}

		const float trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
		if(trigger_right > threshold)
		{// Run状態に遷移
			player_state.SetState(PlayerState::Run);
		}

		if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// Attack_01状態に遷移
			player_state.SetState(PlayerState::Attack_01);
		}
	}

	void PlayerComponent::Run(float dt)
	{
		using namespace locator;

		if (player_state.GetInitialize())
		{// アニメーションセット(Run)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Run), true);
		}

		if(ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// Run_Attack状態に遷移
			player_state.SetState(PlayerState::Run_Attack);
		}

		const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
		if (stick_left.x < threshold && stick_left.y < threshold)
		{// Idle状態に遷移
			player_state.SetState(PlayerState::Idle);
		}
		else
		{
			const float trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
			if (trigger_right < threshold)
			{// Walk状態に遷移
				player_state.SetState(PlayerState::Walk);
			}
		}
	}

	void PlayerComponent::RunAttack(float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(RunAttack)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Run_Attack), false, 0.01f);
		}

		// アニメーションが終われば
		if(!GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
		{// Walk状態に遷移
			player_state.SetState(PlayerState::Walk);
		}
	}

	void PlayerComponent::Evasion(float dt)
	{
	}

	void PlayerComponent::Attack01(float dt)
	{
		if(player_state.GetInitialize())
		{// アニメーションセット(Attack01)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_01), false, 0.01f);
		}

		// アニメーションが再生中か
		if(GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
		{
			if(ButtonState::Press == locator::Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{
				// Attack_02状態に遷移
				player_state.SetState(PlayerState::Attack_02);
			}
		}
		else
		{
			// Idle状態に遷移
			player_state.SetState(PlayerState::Idle);
		}
	}

	void PlayerComponent::Attack02(float dt)
	{
		if(player_state.GetInitialize())
		{// アニメーションセット(Attack_02)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_02), false);
		}

		// アニメーションが再生中か
		if (GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
		{
			if (ButtonState::Press == locator::Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{
				// Attack_03状態に遷移
				player_state.SetState(PlayerState::Attack_03);
			}
		}
		else
		{
			// Idle状態に遷移
			player_state.SetState(PlayerState::Idle);
		}
	}

	void PlayerComponent::Attack03(float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(Attack_03)
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimStateIndex(AnimationState::Attack_03), false);
		}

		// アニメーションが再生中か
		if (!GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).IsPlayAnimation())
		{
			// Idle状態に遷移
			player_state.SetState(PlayerState::Idle);
		}
	}

	void PlayerComponent::Damage(float dt)
	{
	}

	void PlayerComponent::RevengeGuard(float dt)
	{
	}

	void PlayerComponent::Death(float dt)
	{
	}
} // cumulonimbus::component
