#include "player_component.h"

#include <DirectXMath.h>
#include <SimpleMath.h>

#include "arithmetic.h"
#include "collision_name_mapping.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "locator.h"
#include "model_loader.h"
#include "effekseer_loader.h"
// components(engine)
#include "hierarchy_component.h"
#include "camera_component.h"
#include "model_component.h"
#include "raycast_component.h"
#include "rigid_body_component.h"
#include "scene.h"
#include "scene_manager.h"
#include "transform_component.h"
#include "damageable_component.h"
#include "sphere_collision_component.h"
// components(game)
#include "capsule_collison_component.h"
#include "player_sword_component.h"
#include "time_scale.h"
#include "player_avoid_effect_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::PlayerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::Actor3DComponent, cumulonimbus::component::PlayerComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::PlayerComponent, 1);

namespace keyframe_event
{
	const std::string event_collision{ "Event_Collision" };
	const std::string event_move{ "Event_Move" };
	const std::string event_animation{ "Event_Animation" };
} // keyframe_event

// プレイヤーアニメーションのキーフレーム調整値
namespace adjust_key_frame
{

} // adjust_slot

namespace cumulonimbus::component
{
	template <class Archive>
	void PlayerComponent::load(Archive&& archive, const uint32_t version)
	{
		if(version == 0)
		{
			archive(
				cereal::base_class<Actor3DComponent>(this),
				CEREAL_NVP(player_state),
				CEREAL_NVP(precede_input),
				CEREAL_NVP(adjust_keyframe_map),
				CEREAL_NVP(animation_break_frame),
				CEREAL_NVP(walk_speed),
				CEREAL_NVP(dash_speed),
				CEREAL_NVP(attack_04_speed),
				CEREAL_NVP(avoid_dash_speed),
				CEREAL_NVP(jump_movement_speed),
				CEREAL_NVP(threshold),
				CEREAL_NVP(long_press_time),
				CEREAL_NVP(long_press_slot),
				CEREAL_NVP(attack_04_jump_strength)
			);
		}

		if(version == 1)
		{
			archive(
				cereal::base_class<Actor3DComponent>(this),
				CEREAL_NVP(player_state),
				CEREAL_NVP(precede_input),
				CEREAL_NVP(adjust_keyframe_map),
				CEREAL_NVP(animation_break_frame),
				CEREAL_NVP(walk_speed),
				CEREAL_NVP(dash_speed),
				CEREAL_NVP(attack_04_speed),
				CEREAL_NVP(avoid_dash_speed),
				CEREAL_NVP(jump_movement_speed),
				CEREAL_NVP(threshold),
				CEREAL_NVP(long_press_time),
				CEREAL_NVP(long_press_slot),
				CEREAL_NVP(attack_04_jump_strength),
				CEREAL_NVP(hit_effect_s),
				CEREAL_NVP(hit_effect_m),
				CEREAL_NVP(hit_effect_l)
			);
		}
	}

	template <class Archive>
	void PlayerComponent::save(Archive&& archive, const uint32_t version) const
	{
		if(version >= 0)
		{
			archive(
				cereal::base_class<Actor3DComponent>(this),
				CEREAL_NVP(player_state),
				CEREAL_NVP(precede_input),
				CEREAL_NVP(adjust_keyframe_map),
				CEREAL_NVP(animation_break_frame),
				CEREAL_NVP(walk_speed),
				CEREAL_NVP(dash_speed),
				CEREAL_NVP(attack_04_speed),
				CEREAL_NVP(avoid_dash_speed),
				CEREAL_NVP(jump_movement_speed),
				CEREAL_NVP(threshold),
				CEREAL_NVP(long_press_time),
				CEREAL_NVP(long_press_slot),
				CEREAL_NVP(attack_04_jump_strength),
				CEREAL_NVP(hit_effect_s),
				CEREAL_NVP(hit_effect_m),
				CEREAL_NVP(hit_effect_l)
			);
		}
	}

	PlayerComponent::PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:Actor3DComponent{ registry, ent }
	{
		Initialize(registry, ent);
	}

	PlayerComponent::PlayerComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const PlayerComponent& copy_comp)
		:Actor3DComponent{registry,ent}
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);

		Initialize(registry, ent);
	}

	void PlayerComponent::Initialize(ecs::Registry* registry, const mapping::rename_type::Entity ent)
	{
		InitializeMoveState(registry, ent);
		InitializeKeyframeEvent();

		// 先行入力によるアニメーションの中断フレームの設定
		SetAnimationBreakFrame(AnimationData::Atk_N1, 16);
		SetAnimationBreakFrame(AnimationData::Atk_N2, 20);
		SetAnimationBreakFrame(AnimationData::Atk_N3, 37);

		registry->GetOrEmplaceComponent<DamageableComponent>(ent).RegistryDamageEvent(ent, [ent, registry](const DamageData& damage_data, const collision::HitResult& hit_result) {registry->GetComponent<PlayerComponent>(ent).OnHit(damage_data, hit_result); });

		registry->GetOrEmplaceComponent<RigidBodyComponent>(ent);
		registry->GetComponent<RigidBodyComponent>(ent).SetIsActiveTimeScale(false);
		// レイキャストに関する設定
		if (!registry->TryGetComponent<RayCastComponent>(ent))
		{
			registry->AddComponent<RayCastComponent>(ent, collision::CollisionTag::Player);
		}
		// 床(floor)用rayの追加 & 設定
		registry->GetComponent<RayCastComponent>(ent).AddRay(mapping::collision_name::ray::ForFloor(), {});
		registry->GetComponent<RayCastComponent>(ent).SetRayOffset(mapping::collision_name::ray::ForFloor(), { 0.0f,20.0f,0.0f });
		// 壁(wall)用rayの追加 & 設定
		registry->GetComponent<RayCastComponent>(ent).AddRay(mapping::collision_name::ray::ForWall(), {});
		registry->GetComponent<RayCastComponent>(ent).SetRayOffset(mapping::collision_name::ray::ForWall(), { 0.0f,20.0f,0.0f });

		// カメラに関する設定
		if (!registry->TryGetComponent<CameraComponent>(ent))
		{
			registry->AddComponent<CameraComponent>(ent, true);
		}
		registry->GetComponent<CameraComponent>(ent).GetCamera()->SetCameraSpeed({ 0.05f,0.05f });
		registry->GetComponent<CameraComponent>(ent).GetCamera()->SetFocusOffset({ 0.0f,50.0f,0.0f });
	}

	void PlayerComponent::InitializeMoveState(ecs::Registry* registry, const mapping::rename_type::Entity& ent)
	{
		// stateのリセット
		player_state.ClearState();
		// stateに応じての処理の登録
		player_state.AddState(PlayerState::T_Pose						, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).TPose(dt); });
		player_state.AddState(PlayerState::Idle							, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Idle(dt); });
		player_state.AddState(PlayerState::Walk_Front					, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).WalkFront(dt); });
		player_state.AddState(PlayerState::Dash							, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Dash(dt); });
		player_state.AddState(PlayerState::Dodge						, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Dodge(dt); });
		player_state.AddState(PlayerState::Avoid_Dash_Begin				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AvoidDashBegin(dt); });
		player_state.AddState(PlayerState::Avoid_Dash_End				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AvoidDashEnd(dt); });
		player_state.AddState(PlayerState::Damage_Small					, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DamageSmall(dt); });
		player_state.AddState(PlayerState::Damage_Big					, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DamageBig(dt); });
		player_state.AddState(PlayerState::Knock_Down_Front_Loop		, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).KnockDownFrontLoop(dt); });
		player_state.AddState(PlayerState::Knock_Down_Front_Stand_Up	, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).KnockDownFrontStandUp(dt); });
		player_state.AddState(PlayerState::Die							, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).Die(dt); });
		player_state.AddState(PlayerState::Attack_Normal_01				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AtkNormal01(dt); });
		player_state.AddState(PlayerState::Attack_Normal_02				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AtkNormal02(dt); });
		player_state.AddState(PlayerState::Attack_Normal_03				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AtkNormal03(dt); });
		player_state.AddState(PlayerState::Attacking_Normal_Long_Press	, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackingNormalLongPress(dt); });
		player_state.AddState(PlayerState::Attack_Normal_Long_Press_End	, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AttackNormalLongPressEnd(dt); });
		player_state.AddState(PlayerState::Attack_Strong_01				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AtkStrong01(dt); });
		player_state.AddState(PlayerState::Attack_Strong_02				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AtkStrong02(dt); });
		player_state.AddState(PlayerState::Attack_Strong_03				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AtkStrong03(dt); });
		player_state.AddState(PlayerState::Attack_Strong_04				, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).AtkStrong04(dt); });
		player_state.AddState(PlayerState::Dash_Attack					, [ent, registry](const float dt) {registry->GetComponent<PlayerComponent>(ent).DashAttack(dt); });
		// 初期stateの設定(PlayerState::Idle)
		player_state.SetState(PlayerState::Idle);
	}

	void PlayerComponent::InitializeKeyframeEvent()
	{
		//-- キーフレームイベント登録 --//
		// 弱攻撃(当たり判定)
		RegistryKeyframeEvent(AnimationData::Atk_N1, keyframe_event::event_collision);
		RegistryKeyframeEvent(AnimationData::Atk_N2, keyframe_event::event_collision);
		RegistryKeyframeEvent(AnimationData::Atk_N3, keyframe_event::event_collision);
		// 強攻撃(当たり判定)
		RegistryKeyframeEvent(AnimationData::Atk_S1, keyframe_event::event_collision);
		RegistryKeyframeEvent(AnimationData::Atk_S2, keyframe_event::event_collision);
		RegistryKeyframeEvent(AnimationData::Atk_S3, keyframe_event::event_collision);
		RegistryKeyframeEvent(AnimationData::Atk_S4, keyframe_event::event_collision);
		// 強攻撃(移動距離)
		RegistryKeyframeEvent(AnimationData::Atk_S4, keyframe_event::event_move);
		// バックステップ(移動速度)
		RegistryKeyframeEvent(AnimationData::Dodge, keyframe_event::event_move);
		// ダッシュ攻撃(アニメーション)
		RegistryKeyframeEvent(AnimationData::Dash_Attack, keyframe_event::event_animation);

		//-- キーフレームイベントの範囲指定 --//
		// 弱攻撃(当たり判定)
		GetKeyframeEvent(AnimationData::Atk_N1).SetKeyEvent(keyframe_event::event_collision, 10, 16);
		GetKeyframeEvent(AnimationData::Atk_N2).SetKeyEvent(keyframe_event::event_collision,  0, 16);
		GetKeyframeEvent(AnimationData::Atk_N3).SetKeyEvent(keyframe_event::event_collision, 20, 30);
		// 強攻撃(当たり判定)
		GetKeyframeEvent(AnimationData::Atk_S1).SetKeyEvent(keyframe_event::event_collision, 15, 26);
		GetKeyframeEvent(AnimationData::Atk_S2).SetKeyEvent(keyframe_event::event_collision,  8, 17);
		GetKeyframeEvent(AnimationData::Atk_S3).SetKeyEvent(keyframe_event::event_collision, 10, 20);
		GetKeyframeEvent(AnimationData::Atk_S4).SetKeyEvent(keyframe_event::event_collision,  2, 18);
		// 強攻撃(移動距離)
		GetKeyframeEvent(AnimationData::Atk_S4).SetKeyEvent(keyframe_event::event_move,  5, 10);
		// バックステップ(移動距離)
		GetKeyframeEvent(AnimationData::Dodge).SetKeyEvent(keyframe_event::event_move, 1, 15);
		// ダッシュ攻撃(アニメーション)
		GetKeyframeEvent(AnimationData::Dash_Attack).SetKeyEvent(keyframe_event::event_animation, 15, 17);
	}

	void PlayerComponent::Start()
	{
		// 初期stateの設定(PlayerState::Idle)
		player_state.SetState(PlayerState::Idle);

		const mapping::rename_type::Entity entity = GetEntity();
		ecs::Registry* registry = GetRegistry();
		// ヒット時イベントの登録
		if (auto* damageable_comp = GetRegistry()->TryGetComponent<DamageableComponent>(GetEntity());
			damageable_comp)
		{
			damageable_comp->RegistryDamageEvent(GetEntity(), [=](const DamageData& damage_data, const collision::HitResult& hit_result) { registry->GetComponent<PlayerComponent>(entity).OnHit(damage_data, hit_result); });
		}
		// ジャスト回避イベントの登録
		if (auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
			sphere_collision_comp)
		{
			auto ent = GetEntity();
			sphere_collision_comp->RegisterEventEnter(GetEntity(), [=](const collision::HitResult& hit_result) {registry->GetComponent<PlayerComponent>(GetEntity()).OnHitAvoidRange(hit_result); });
		}
		// リジッドボディ
		if (auto* rigid_body_comp = GetRegistry()->TryGetComponent<RigidBodyComponent>(GetEntity());
			rigid_body_comp)
		{
			rigid_body_comp->GravityStop(true);
		}
		// モデルコンポーネント
		if (auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());
			model_comp)
		{
			model_comp->SetIsUseTimeScale(false);
		}
		// カプセルコリジョン
		if(auto* capsule_collision_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(GetEntity());
		   capsule_collision_comp)
		{
			//capsule_collision_comp->RegisterEventEnter(GetEntity(),[=](const collision::HitResult& hit_result){registry})
		}
		// 子階層からPlayerSwordComponentを持つエンティティの取得
		if (auto* hierarchy_comp = GetRegistry()->TryGetComponent<HierarchyComponent>(GetEntity());
			hierarchy_comp)
		{
			const auto sub_hierarchies = hierarchy_comp->GetSubHierarchies();
			for(const auto& sub_hierarchy : sub_hierarchies)
			{
				if (!GetRegistry()->HasComponent<PlayerSwordComponent>(sub_hierarchy))
					continue;

				sword_ent = sub_hierarchy;
				break;
			}
		}

		//GetRegistry()->GetOrEmplaceComponent<DamageableComponent>(GetEntity()).RegistryDamageEvent(GetEntity(), [=](const DamageData& damage_data, const collision::HitResult& hit_result) { registry->GetComponent<PlayerComponent>(entity).OnHit(damage_data, hit_result); });
	}

	void PlayerComponent::PreGameUpdate(float dt)
	{
	}

	void PlayerComponent::GameUpdate(const float dt)
	{
		Collision();

		// 現プレイヤーのstate処理を実行
		player_state.Update(dt);

		// レイキャストに使用するパラメータの設定
		//SetRayCastParameter(dt);
		// カメラワーク
		CameraWork();
		// 移動
		Movement(dt);
		Rotation(dt);
	}

	void PlayerComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<PlayerComponent>(GetEntity(), "Player"))
		{
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& effect_loader = *asset_manager.GetLoader<asset::EffekseerLoader>();

			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Dead zone value of pad input", &threshold		, 0.01f	, 0.0f, 1.);
			ImGui::Separator();
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Walk Speed		"	, &walk_speed		, 0.5f	, 0.1f, FLT_MAX);
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Dash Speed		"	, &dash_speed		, 0.5f	, 0.1f, FLT_MAX);
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Avoid Dash Speed"	, &avoid_dash_speed , 0.5f	, 0.1f, FLT_MAX);
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Dash Atk Speed"		, &dash_attack_speed, 0.5f	, 0.1f, FLT_MAX);
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Counter Speed	"	, &attack_04_speed	, 0.5f	, 0.1f, FLT_MAX);
			ImGui::Separator();
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Avoid Stop Time", &avoid_stop_time, 0.5f, 0.1f, 10.f);
			ImGui::Text("Is Avoid %d", is_avoid);
			ImGui::Separator();
			ImGui::Text("Hit Effect S");
			effect_loader.ImSelectableEffect(asset_manager, hit_effect_s);
			ImGui::Text("Hit Effect M");
			effect_loader.ImSelectableEffect(asset_manager, hit_effect_m);
			ImGui::Text("Hit Effect L");
			effect_loader.ImSelectableEffect(asset_manager, hit_effect_l);
		}
	}

	void PlayerComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
		Initialize(registry, GetEntity());
	}

	int PlayerComponent::GetAnimDataIndex(AnimationData anim_state) const
	{
		return static_cast<int>(anim_state);
	}

	void PlayerComponent::SetRayCastParameter(const float dt)
	{
		auto* ray_cast_comp = GetRegistry()->TryGetComponent<RayCastComponent>(GetEntity());
		if (!ray_cast_comp)
			return;

		const auto& transform_comp   = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		const auto& rigid_body_comp  = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());

		{// 床(floor)用rayの設定
			const DirectX::SimpleMath::Vector3 ray_start = transform_comp.GetPosition() + ray_cast_comp->GetRayOffset(mapping::collision_name::ray::ForFloor());
			ray_cast_comp->SetRayStartPos(mapping::collision_name::ray::ForFloor(), ray_start);
			ray_cast_comp->SetBlockPos(mapping::collision_name::ray::ForFloor(), transform_comp.GetPosition());
			if (rigid_body_comp.GetIsGravity())
			{
				ray_cast_comp->SetRayEndPos(mapping::collision_name::ray::ForFloor(), ray_start + DirectX::SimpleMath::Vector3{ 0,rigid_body_comp.GetVelocity().y * 50,0 });
			}
			else
			{
				ray_cast_comp->SetRayEndPos(mapping::collision_name::ray::ForFloor(), ray_start + DirectX::SimpleMath::Vector3{ 0,-50,0 });
			}
		}

		{// 壁(wall)用rayの設定
			const DirectX::SimpleMath::Vector3 ray_start = transform_comp.GetPosition() + ray_cast_comp->GetRayOffset(mapping::collision_name::ray::ForWall());
			const DirectX::SimpleMath::Vector3 ray_end	 = ray_start + DirectX::SimpleMath::Vector3{ transform_comp.GetModelFront().x * 70,0,transform_comp.GetModelFront().z * 70 };
			DirectX::SimpleMath::Vector3 ray_vec{ ray_end - ray_start };
			ray_vec.Normalize();
			ray_cast_comp->SetRayStartPos(mapping::collision_name::ray::ForWall(), ray_start);
			ray_cast_comp->SetRayEndPos(mapping::collision_name::ray::ForWall(), ray_end);
			ray_cast_comp->SetBlockPos(mapping::collision_name::ray::ForWall(), ray_start + ray_vec * 30);
		}
	}

	void PlayerComponent::Movement(const float dt)
	{
		if(auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		   transform_comp.GetPosition().y < 0)
		{
			auto& rigid_body_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
			transform_comp.SetPosition_Y(.0f);
			is_jumping = false;
		}

		if(is_avoid)
		{
			elapsed_time += locator::Locator::GetSystem()->GetTime().GetUnscaledDeltaTime();
			locator::Locator::GetSystem()->GetTime().SetScale(0.1f);

			if(elapsed_time > avoid_stop_time)
			{
				elapsed_time = 0;
				is_avoid	 = false;
				locator::Locator::GetSystem()->GetTime().SetScale(1.0f);

				if (auto* player_avoid_comp = GetRegistry()->TryGetComponent<PlayerAvoidEffectComponent>(GetEntity());
					player_avoid_comp)
				{
					player_avoid_comp->SetEffectState(PlayerAvoidEffectComponent::EffectState::End_Avoid);
				}
			}
		}
		else
		{
			elapsed_time = 0;
			locator::Locator::GetSystem()->GetTime().SetScale(1.0f);
		}
	}

	void PlayerComponent::Rotation(float dt)
	{
		const DirectX::XMFLOAT2 stick_left = locator::Locator::GetInput()->GamePad().LeftThumbStick(0);

		if (IsDeadZoneStickLeft())
			return;

		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& camera_comp	 = GetRegistry()->GetComponent<CameraComponent>(GetEntity());

		DirectX::SimpleMath::Vector3 stick_direction = { stick_left.x,0.0f,stick_left.y };
		stick_direction.Normalize();

		// モデルの基底前方ベクトル{0,0,1}とスティック入力方向とのベクトルの角度(ラジアン)を算出
		float rad = arithmetic::CalcAngleFromTwoVec(stick_direction, { 0,0,1 });
		// 回転する方向をスティック入力(x値)で補正
		if (stick_left.x < 0)
			rad *= -1;
		// カメラのフロントベクトルをrad分回転
		DirectX::SimpleMath::Vector3 camera_xz_front_vec = camera_comp.GetCamera()->GetCameraFront();
		const DirectX::SimpleMath::Quaternion q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0,1,0 }, rad);
		DirectX::SimpleMath::Vector3::Transform(camera_xz_front_vec, q, camera_xz_front_vec);
		camera_xz_front_vec.y = 0;
		camera_xz_front_vec.Normalize();


		if (const PlayerState state = player_state.GetState();
			state == PlayerState::Walk_Front ||
			state == PlayerState::Dash ||
			state == PlayerState::Avoid_Dash_Begin)
		{
			// モデルの前方ベクトルとfront_vecとの角度(ラジアン)を算出
			DirectX::SimpleMath::Vector3 model_xz_front = transform_comp.GetModelFront();
			model_xz_front.y = 0;
			model_xz_front.Normalize();
			if (arithmetic::IsEqual(model_xz_front.x, camera_xz_front_vec.x) &&
				arithmetic::IsEqual(model_xz_front.y, camera_xz_front_vec.y) &&
				arithmetic::IsEqual(model_xz_front.z, camera_xz_front_vec.z))
				return;

			rad = arithmetic::CalcAngleFromTwoVec(camera_xz_front_vec, model_xz_front);
			const DirectX::SimpleMath::Vector3 cross_vec = model_xz_front.Cross(camera_xz_front_vec);
			if (cross_vec.y < 0)
				rad *= -1;
			// モデルの回転
			transform_comp.AdjustRotationFromAxis({ 0,1,0 }, rad);
		}
	}

	void PlayerComponent::Collision() const
	{
		auto* ray_cast_comp = GetRegistry()->TryGetComponent<RayCastComponent>(GetEntity());
		if (!ray_cast_comp)
			return;

		auto* rigid_body_comp = GetRegistry()->TryGetComponent<RigidBodyComponent>(GetEntity());
		if (!rigid_body_comp)
			return;

		if (ray_cast_comp->GetIsBlockHit(mapping::collision_name::ray::ForFloor()))
		{
			rigid_body_comp->SetCurrentGravity(0);
		}
	}

	void PlayerComponent::CameraWork()
	{
		using namespace locator;
		const float rad_x		= Locator::GetInput()->GamePad().RightThumbStick(0).x;
		const float rad_y		= Locator::GetInput()->GamePad().RightThumbStick(0).y;
		auto& transform_comp	= GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& camera_comp		= GetRegistry()->GetComponent<CameraComponent>(GetEntity());
		camera_comp.GetCamera()->RotationTPSYaw(rad_x * camera_comp.GetCamera()->GetCameraSpeed().x);
		camera_comp.GetCamera()->RotationTPSPitch(rad_y * camera_comp.GetCamera()->GetCameraSpeed().y);
		camera_comp.GetCamera()->SetFocusPosition(transform_comp.GetPosition());
	}

	void PlayerComponent::RegistryKeyframeEvent(const AnimationData anim_data, const std::string& key_name)
	{
		if (keyframe_events.contains(anim_data))
		{
			keyframe_events.at(anim_data).RegistryEvent(key_name);
			return;
		}

		keyframe_events.emplace(anim_data, system::KeyframeEvent{});
		keyframe_events.at(anim_data).RegistryEvent(key_name);
	}

	system::KeyframeEvent& PlayerComponent::GetKeyframeEvent(const AnimationData anim_data)
	{
		if (!keyframe_events.contains(anim_data))
			assert(!"Don't have KeyframeEvent(PlayerComponent::GetKeyframeEvent)");

		return keyframe_events.at(anim_data);
	}

	void PlayerComponent::SetAdjustKeyFrame(const std::string& animation_name, u_int keyframe)
	{
		if (adjust_keyframe_map.contains(animation_name))
		{
			adjust_keyframe_map.at(animation_name) = keyframe;
			return;
		}

		// "animation_name"がモデルのアニメーション名と一致するかの確認
		const auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		auto& model_loader = *locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>();
		if (!model_loader.HasModel(model_comp.GetModelID()))
			return;

		for (auto& animation : model_loader.GetModel(model_comp.GetModelID()).GetModelData().GetAnimations())
		{
			if (animation_name == animation.animation_name)
			{
				adjust_keyframe_map.insert(std::make_pair(animation_name, keyframe));
				animation.num_key_frame  = keyframe;
				animation.seconds_length = keyframe * animation.sampling_time;

				return;
			}
		}
		assert(!"No animation names existed(PlayerComponent::SetAdjustKeyFrame)");
	}

	void PlayerComponent::SetAnimationBreakFrame(AnimationData state, u_int keyframe)
	{
		// マップに存在していればキーフレームを上書きする
		if (animation_break_frame.contains(state))
		{
			animation_break_frame.at(state) = keyframe;
			return;
		}

		animation_break_frame.insert(std::make_pair(state, keyframe));
	}

	bool PlayerComponent::IsNextAnimationLongPressAttack() const
	{
		const float frame_rate = GetRegistry()->GetScene()->GetSceneManager()->GetFramework()->GetHighResolutionTimer().GetFrameRate();
		const float fps = 1 / frame_rate;
		return long_press_time > (fps * static_cast<float>(long_press_slot)) ? true : false;
	}

	bool PlayerComponent::IsDeadZoneStickLeft() const
	{
		const DirectX::XMFLOAT2 stick_left = locator::Locator::GetInput()->GamePad().LeftThumbStick(0);

		if (fabs(stick_left.x) < threshold &&
			fabs(stick_left.y) < threshold)
			return true;

		return false;
	}

	bool PlayerComponent::IsDeadZoneTriggerRight() const
	{
		const float	trigger_right = locator::Locator::GetInput()->GamePad().RightTrigger(0);
		if (trigger_right < threshold)
			return true;

		return false;
	}

	bool PlayerComponent::IsBreakAnimationFrame(AnimationData state) const
	{
		if (!animation_break_frame.contains(state))
			assert((!"Don't have state(PlayerComponent::IsBreakAnimationFrame)"));

		const auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		return static_cast<int>(animation_break_frame.at(state)) < model_comp.CurrentKeyframe() ? true : false;
	}

	void PlayerComponent::OnDamaged(const component::DamageData& damage_data)
	{

	}

	void PlayerComponent::OnHit(const DamageData& damage_data, const collision::HitResult& hit_result)
	{

		if (player_state.GetState() == PlayerState::Avoid_Dash_Begin ||
			player_state.GetState() == PlayerState::Dodge)
			return;

		//hp -= damage_data.damage_amount;

		if(hp <= 0)
		{
			player_state.SetState(PlayerState::Die);
			return;
		}

		player_state.SetState(PlayerState::Damage_Small);

		//is_avoid = false;

		//if(player_state.GetState() == PlayerState::Avoid_Dash_Begin)
		//{
		//	is_avoid = true;
		//	this->hit_result = hit_result;
		//}
		//else
		//{
		//	//player_state.SetState(PlayerState::Damage_Small);
		//}
	}

	void PlayerComponent::OnHitAvoidRange(const collision::HitResult& hit_result)
	{
		// 回避状態以外の状態の場合処理を抜ける
		if (player_state.GetState() != PlayerState::Avoid_Dash_Begin &&
			player_state.GetState() != PlayerState::Dodge)
			return;

		// ヒット先のタグがEnemy_Weapon以外の場合処理を抜ける
		if (hit_result.collision_tag != collision::CollisionTag::Enemy_Weapon)
			return;

		// ヒット先のWeaponが持つ親エンティティの取得
		mapping::rename_type::Entity parent_ent{};
		parent_ent = hit_result.registry->GetComponent<HierarchyComponent>(hit_result.entity).GetParentEntity();
		//if (parent_ent.empty())
		//	return;

		this->hit_result		= hit_result;
		this->hit_result.entity = parent_ent;

		is_avoid = true;


		if (auto* player_avoid_comp = GetRegistry()->TryGetComponent<PlayerAvoidEffectComponent>(GetEntity());
			player_avoid_comp)
		{
			if(player_avoid_comp->GetEffectState() == PlayerAvoidEffectComponent::EffectState::Stop)
			{
				player_avoid_comp->SetEffectState(PlayerAvoidEffectComponent::EffectState::Begin_Avoid);
				player_avoid_comp->SetAvoidingTime(avoid_stop_time);
			}
		}
	}

	float PlayerComponent::GetAndResetAnimSwitchTime(const float reset_time)
	{
		const float ret_val = anim_switch_time;
		anim_switch_time = reset_time;
		return ret_val;
	}

	void PlayerComponent::TPose(const float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationData::Idle)
			GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::T_Pose), true,1.f);
		}

		if (!GetRegistry()->GetComponent<ModelComponent>(GetEntity()).IsPlayAnimation())
		{// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}
	}

	void PlayerComponent::Idle(const float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationData::Idle)
			GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Idle), true, GetAndResetAnimSwitchTime());
		}

		using namespace locator;
		const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);

		{// 移動速度の設定
			//AdjustVelocity(dt, { .0f,.0f,.0f});
		}

		const float	trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);

		if (!IsDeadZoneStickLeft())
		{// 状態遷移(PlayerState::Walk)
			player_state.SetState(PlayerState::Walk_Front);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 状態遷移(PlayerState::Attack_Normal_01)
			player_state.SetState(PlayerState::Attack_Normal_01);
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 状態遷移(PlayerState::Attack_Strong_01)
			player_state.SetState(PlayerState::Attack_Strong_01);
		}
		else if(locator::Locator::GetInput()->GamePad().GetState(GamePadButton::RightShoulder) ==
				ButtonState::Press)
		{
			player_state.SetState(PlayerState::Dodge);
		}
	}

	void PlayerComponent::WalkFront(float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationData::Walk)
			GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Walk_Front), true);
		}

		using namespace locator;
		// ゲームパッド入力値取得(スティック、トリガー)
		const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
		const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);

		{// 移動速度の設定
			//physics_comp.AddForce({ walk_speed,0.0f,walk_speed });
			auto& rigid_body_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
			rigid_body_comp.AddForce({ walk_speed,0.0f,walk_speed });
			//AdjustVelocity(dt, { walk_speed,0.0f,walk_speed });
		}

		{// アニメーション遷移
			if (IsDeadZoneStickLeft())
			{// 状態遷移(PlayerState::Idle)
				player_state.SetState(PlayerState::Idle);
			}

			//if (trigger_right > threshold)
			if(locator::Locator::GetInput()->GamePad().GetState(GamePadButton::RightShoulder) ==
			   ButtonState::Press)
			{// 状態遷移(PlayerState::Avoid_Dash_Begin)
				player_state.SetState(PlayerState::Avoid_Dash_Begin);
			}

			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 状態遷移(PlayerState::Attack_Normal_01)
				player_state.SetState(PlayerState::Attack_Normal_01);
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// 状態遷移(PlayerState::Attack_Strong_01)
				player_state.SetState(PlayerState::Attack_Strong_01);
			}
		}
	}

	void PlayerComponent::Dodge(float dt)
	{
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Dodge)
			GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Dodge), false);

			if (auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
				sphere_collision_comp)
			{
				sphere_collision_comp->SetAllCollisionEnable(true);
			}
		}


		GetKeyframeEvent(AnimationData::Dodge).Update(GetRegistry(), GetEntity(), keyframe_event::event_move);
		if (GetKeyframeEvent(AnimationData::Dodge).GetKeyEvent(keyframe_event::event_move).key_state
			== system::KeyframeEvent::KeyState::OnKeyRangeStay)
		{// 移動中(バックステップ中)
			{// 移動速度の設定
				auto& rigid_body_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
				rigid_body_comp.AddForce({ -dodge_speed,0.0f,-dodge_speed });
			}
		}
		else
		{// 移動中でない(バックステップ中でない)
			// 左スティックの入力があれば
			if (!IsDeadZoneStickLeft() &&
				!IsDeadZoneTriggerRight())
			{
				player_state.SetState(PlayerState::Avoid_Dash_Begin);
			}
		}

		if (is_avoid)
		{
			if (locator::Locator::GetInput()->GamePad().GetState(GamePadButton::X) == ButtonState::Press)
			{
				player_state.SetState(PlayerState::Attack_Strong_04);
				is_avoid = false;
				if (auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
					sphere_collision_comp)
				{
					sphere_collision_comp->SetAllCollisionEnable(false);
				}

				return;
			}
		}

		using namespace locator;

		{// アニメーション遷移
			// アニメーション再生中なら処理を中断
			if (GetRegistry()->GetComponent<ModelComponent>(GetEntity()).IsPlayAnimation())
				return;

			player_state.SetState(PlayerState::Idle);

			if (auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
				sphere_collision_comp)
			{
				sphere_collision_comp->SetAllCollisionEnable(false);
			}
		}
	}

	void PlayerComponent::AvoidDashBegin(float dt)
	{
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Avoid_Dash_Begin)
			GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Avoid_Dash_Begin), false);

			if (auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
				sphere_collision_comp)
			{
				sphere_collision_comp->SetAllCollisionEnable(true);
			}
		}

		{// 移動速度の設定
			auto& movement_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
			movement_comp.AddForce({ avoid_dash_speed,0.0f,avoid_dash_speed });
			//AdjustVelocity(dt, { avoid_dash_speed,0.0f,avoid_dash_speed });
		}

		using namespace locator;

		if (is_avoid)
		{
			if (Locator::GetInput()->GamePad().GetState(GamePadButton::X) == ButtonState::Press)
			{
				is_avoid = false;
				if (auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
					sphere_collision_comp)
				{
					sphere_collision_comp->SetAllCollisionEnable(false);
				}

				return;
			}
		}

		{// アニメーション遷移
			// アニメーション再生中なら処理を中断
			if (GetRegistry()->GetComponent<ModelComponent>(GetEntity()).IsPlayAnimation())
				return;

			//is_avoid = false;

			if (auto* sphere_collision_comp = GetRegistry()->TryGetComponent<SphereCollisionComponent>(GetEntity());
				sphere_collision_comp)
			{
				sphere_collision_comp->SetAllCollisionEnable(false);
			}

			// ゲームパッド入力値取得(スティック、トリガー)
			const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
			const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);

			if (const auto right_shoulder_state = locator::Locator::GetInput()->GamePad().GetState(GamePadButton::RightShoulder);
				right_shoulder_state == ButtonState::Release ||
				right_shoulder_state == ButtonState::None)
			{//	状態遷移(PlayerState::Avoid_Dash_End)
				player_state.SetState(PlayerState::Avoid_Dash_End);
			}
			else
			{
				if ((stick_left.x < threshold && -stick_left.x < threshold) &&
					(stick_left.y < threshold && -stick_left.y < threshold))
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
	}

	void PlayerComponent::Dash(float dt)
	{
		if (player_state.GetInitialize())
		{// アニメーションセット(AnimationData::Dash)
			GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Dash), true);
		}

		{// 移動速度の設定
			auto& movement_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
			movement_comp.AddForce({ dash_speed,0.0f,dash_speed });
		}

		{// アニメーション遷移
			// ゲームパッド入力値取得(スティック、トリガー)
			using namespace locator;
			const DirectX::XMFLOAT2 stick_left = Locator::GetInput()->GamePad().LeftThumbStick(0);
			const float				trigger_right = Locator::GetInput()->GamePad().RightTrigger(0);
			if (locator::Locator::GetInput()->GamePad().GetState(GamePadButton::RightShoulder) ==
				ButtonState::Release)
			{
				player_state.SetState(PlayerState::Avoid_Dash_End);

				//if (stick_left.x > threshold || stick_left.y > threshold)
				//{// 状態遷移(PlayerState::Walk)
				//	//player_state.SetState(PlayerState::Walk_Front);
				//	player_state.SetState(PlayerState::Avoid_Dash_End);
				//}
				//else
				//{// 状態遷移(PlayerState::Idle)
				//	//player_state.SetState(PlayerState::Idle);
				//	player_state.SetState(PlayerState::Avoid_Dash_End);
				//}
			}
			else
			{
				if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
				{// 状態遷移(PlayerState::Dash_Attack)
					player_state.SetState(PlayerState::Dash_Attack);
				}
			}
		}
	}


	void PlayerComponent::AvoidDashEnd(float dt)
	{
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Avoid_Dash_End)
			GetRegistry()->GetComponent<ModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Avoid_Dash_End), false);
		}

		{// 移動速度の設定
			auto& movement_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
			movement_comp.AddForce({ avoid_dash_speed * 0.7f,0.0f,avoid_dash_speed * 0.7f });
			//AdjustVelocity(dt, { avoid_dash_speed,0.0f,avoid_dash_speed });
		}

		// アニメーション再生中なら処理を中断
		if (GetRegistry()->GetComponent<ModelComponent>(GetEntity()).IsPlayAnimation())
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

	void PlayerComponent::KnockDownFrontLoop(float dt)
	{
	}

	void PlayerComponent::KnockDownFrontStandUp(float dt)
	{
	}

	void PlayerComponent::DamageSmall(float dt)
	{

		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());

		if(player_state.GetInitialize())
		{
			// アニメーションセット(AnimationData::Walk)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Damage_Small), false);
		}

		if (model_comp.IsPlayAnimation())
			return;

		player_state.SetState(PlayerState::Idle);
	}

	void PlayerComponent::DamageBig(float dt)
	{
	}

	void PlayerComponent::Die(float dt)
	{
	}

	void PlayerComponent::DashAttack(const float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if(player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Dash_Attack)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Dash_Attack));

			anim_stop_timer = 0;
		}

		//if(anim_stop_timer < anim_stop_dash_atk)
		//{
		//	auto& keyframe_event = GetKeyframeEvent(AnimationData::Dash_Attack);
		//	keyframe_event.Update(GetRegistry(), GetEntity(), keyframe_event::event_animation);
		//	if(keyframe_event.GetKeyEvent(keyframe_event::event_animation).key_state ==
		//	   system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		//	{
		//		model_comp.SetIsStopAnimation(true);
		//	}
		//}
		//else
		//{
		//	model_comp.SetIsStopAnimation(false);
		//}

		if(model_comp.GetIsStopAnimation())
		{
			anim_stop_timer += dt;
		}
		else
		{
			auto& rigid_body_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
			rigid_body_comp.AddForce({ dash_attack_speed,0,dash_attack_speed });
		}

		//model_comp.Get

		using namespace locator;
		const auto& game_pad = Locator::GetInput()->GamePad();
		if(ButtonState::Press == game_pad.GetState(GamePadButton::X))
		{

		}

		if (model_comp.IsPlayAnimation())
			return;

		// 遷移時間の設定
		anim_switch_time = switch_dash_attack_to_idle;
		// 状態遷移(PlayerState::Idle)
		player_state.SetState(PlayerState::Idle);
	}

	void PlayerComponent::AtkNormal01(const float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Attack_Normal_01)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Atk_N1), false, 0.01f);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}

			current_hit_effect_id = hit_effect_s;
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		GetKeyframeEvent(AnimationData::Atk_N1).Update(GetRegistry(), GetEntity(), keyframe_event::event_collision);
		if (GetKeyframeEvent(AnimationData::Atk_N1).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}

		if (GetKeyframeEvent(AnimationData::Atk_N1).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		using namespace locator;
		if (!IsBreakAnimationFrame(AnimationData::Atk_N1))
		{
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 先行入力セット(PlayerState::Attacking_Normal_02)
				precede_input = PlayerState::Attack_Normal_02;
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// 先行入力セット(PlayerState::Attacking_Strong_02)
				precede_input = PlayerState::Attack_Strong_01;
			}

			if (ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 通常攻撃(弱長押し)用タイマーの計測
				long_press_time += dt;
			}

			// アニメーション再生中なら処理を中断
			return;
		}

		if ((!model_comp.IsPlayAnimation()))
		{// 先行入力なし
			// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}

		if ((precede_input != PlayerState::End))
		{
			if (IsBreakAnimationFrame(AnimationData::Atk_N1))
			{// 先行入力あり
				// 状態遷移(先行入力値)
				player_state.SetState(precede_input);
			}
		}

		if (IsNextAnimationLongPressAttack())
		{// 状態遷移(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AtkNormal02(const float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Attack_Normal_02)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Atk_N2), false);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		GetKeyframeEvent(AnimationData::Atk_N2).Update(GetRegistry(), GetEntity(), keyframe_event::event_collision);
		if (GetKeyframeEvent(AnimationData::Atk_N2).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}

		if (GetKeyframeEvent(AnimationData::Atk_N2).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		if (!IsBreakAnimationFrame(AnimationData::Atk_N2))
		{
			using namespace locator;
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 先行入力セット(PlayerState::Attacking_Normal_03)
				precede_input = PlayerState::Attack_Normal_03;
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// 先行入力セット(PlayerState::Attacking_Strong_03)
				precede_input = PlayerState::Attack_Strong_01;
			}

			if (ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 通常攻撃(弱長押し)用タイマーの計測
				long_press_time += dt;
			}

			// アニメーション再生中なら処理を中断
			return;
		}

		if ((!model_comp.IsPlayAnimation()))
		{// 先行入力なし
			// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}

		if (precede_input != PlayerState::End)
		{
			if (IsBreakAnimationFrame(AnimationData::Atk_N2))
			{// 先行入力あり
				// 状態遷移(先行入力値)
				player_state.SetState(precede_input);
			}
		}

		if (IsNextAnimationLongPressAttack())
		{// 状態遷移(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AtkNormal03(const float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Attack_Normal_03)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Atk_N3), false);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		GetKeyframeEvent(AnimationData::Atk_N3).Update(GetRegistry(), GetEntity(), keyframe_event::event_collision);
		if (GetKeyframeEvent(AnimationData::Atk_N3).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}

		if (GetKeyframeEvent(AnimationData::Atk_N3).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		if (!IsBreakAnimationFrame(AnimationData::Atk_N3))
		{
			using namespace locator;
			if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 先行入力セット(PlayerState::Attack_Normal_02)
				precede_input = PlayerState::Attack_Normal_02;
			}
			else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
			{// 先行入力セット(PlayerState::Attacking_Strong_04)
				precede_input = PlayerState::Attack_Strong_01;
			}

			if (ButtonState::Held == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
			{// 通常攻撃(弱長押し)用タイマーの計測
				long_press_time += dt;
			}

			// アニメーション再生中なら処理を中断
			return;
		}

		if ((!model_comp.IsPlayAnimation()))
		{// 先行入力なし
			// 状態遷移(PlayerState::Idle)
			player_state.SetState(PlayerState::Idle);
		}

		if (precede_input != PlayerState::End)
		{
			if (IsBreakAnimationFrame(AnimationData::Atk_N3))
			{// 先行入力あり
				// 状態遷移(先行入力値)
				player_state.SetState(precede_input);
			}
		}

		if (IsNextAnimationLongPressAttack())
		{// 状態遷移(PlayerState::Attacking_Normal_Long_Press)
			player_state.SetState(PlayerState::Attacking_Normal_Long_Press);
		}
	}

	void PlayerComponent::AttackingNormalLongPress(float dt)
	{
	}

	void PlayerComponent::AttackNormalLongPressEnd(float dt)
	{
	}

	void PlayerComponent::AtkStrong01(float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Attack_Strong_01)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Atk_S1), false);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		GetKeyframeEvent(AnimationData::Atk_S1).Update(GetRegistry(), GetEntity(), keyframe_event::event_collision);
		if (GetKeyframeEvent(AnimationData::Atk_S1).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}

		if (GetKeyframeEvent(AnimationData::Atk_S1).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attack_Normal_01)
			precede_input = PlayerState::Attack_Normal_01;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_02)
			precede_input = PlayerState::Attack_Strong_02;
		}

		// アニメーション再生中なら処理を中断
		if (model_comp.IsPlayAnimation())
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

	void PlayerComponent::AtkStrong02(float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Attack_Strong_02)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Atk_S2), false);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		GetKeyframeEvent(AnimationData::Atk_S2).Update(GetRegistry(), GetEntity(), keyframe_event::event_collision);
		if (GetKeyframeEvent(AnimationData::Atk_S2).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}

		if (GetKeyframeEvent(AnimationData::Atk_S2).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attacking_Normal_03)
			precede_input = PlayerState::Attack_Normal_01;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_03)
			precede_input = PlayerState::Attack_Strong_04;
		}

		// アニメーション再生中なら処理を中断
		if (model_comp.IsPlayAnimation())
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

	// 使用しない
	void PlayerComponent::AtkStrong03(float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Attack_Strong_03)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Atk_S3), false);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		GetKeyframeEvent(AnimationData::Atk_S3).Update(GetRegistry(), GetEntity(), keyframe_event::event_collision);
		if (GetKeyframeEvent(AnimationData::Atk_S3).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}

		if (GetKeyframeEvent(AnimationData::Atk_S3).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attack_Normal_01)
			precede_input = PlayerState::Attack_Normal_01;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attacking_Strong_04)
			precede_input = PlayerState::Attack_Strong_04;
		}

		// アニメーション再生中なら処理を中断
		if (model_comp.IsPlayAnimation())
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

	void PlayerComponent::AtkStrong04(float dt)
	{
		auto& model_comp = GetRegistry()->GetComponent<ModelComponent>(GetEntity());
		if (player_state.GetInitialize())
		{
			InitializeAnimationVariable();
			// アニメーションセット(AnimationData::Attack_Strong_04)
			model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Atk_S4), false);
			// 剣の当たり判定をなくす
			if (auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);
				capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}

		auto* capsule_comp = GetRegistry()->TryGetComponent<CapsuleCollisionComponent>(sword_ent);

		// キーフレームイベントの登録
		GetKeyframeEvent(AnimationData::Atk_S4).Update(GetRegistry(), GetEntity(), keyframe_event::event_collision);
		GetKeyframeEvent(AnimationData::Atk_S4).Update(GetRegistry(), GetEntity(), keyframe_event::event_move);

		// 当たり判定をOn
		if (GetKeyframeEvent(AnimationData::Atk_S4).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeEnter)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(true);
			}
		}
		// 当たり判定をOff
		if (GetKeyframeEvent(AnimationData::Atk_S4).GetKeyEvent(keyframe_event::event_collision).key_state ==
			system::KeyframeEvent::KeyState::OnKeyRangeExit)
		{
			if (capsule_comp)
			{
				capsule_comp->SetAllCollisionEnable(false);
			}
		}
		// 前方へ移動
		if(GetKeyframeEvent(AnimationData::Atk_S4).GetKeyEvent(keyframe_event::event_move).key_state ==
		   system::KeyframeEvent::KeyState::OnKeyRangeStay)
		{
			{// 移動速度の設定
				auto& rigid_body_comp = GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
				rigid_body_comp.AddForce({ attack_strong_04_speed,0.0f,attack_strong_04_speed });
			}
		}

		using namespace locator;
		if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::X))
		{// 先行入力セット(PlayerState::Attack_Normal_01)
			precede_input = PlayerState::Attack_Normal_01;
		}
		else if (ButtonState::Press == Locator::GetInput()->GamePad().GetState(GamePadButton::Y))
		{// 先行入力セット(PlayerState::Attack_Strong_01)
			precede_input = PlayerState::Attack_Strong_01;
		}

		// アニメーション再生中なら処理を中断
		if (model_comp.IsPlayAnimation())
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
} // cumulonimbus::component
