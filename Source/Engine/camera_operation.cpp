#include "camera_operation.h"

#include <cassert>
#include <cmath>

#include "arithmetic.h"
#include "scene.h"
#include "view.h"
#include "transform.h"
#include "transform_component.h"
#include "file_path_helper.h"

using namespace DirectX;

CameraOperationComponent::CameraOperationComponent(Entity* entity, EntityTag tag, XMFLOAT3 camera_pos, float default_camera_length)
	: Component{ entity }
	, my_target_tag{ tag }
	, camera_position{ camera_pos }
	, default_camera_length{ default_camera_length }
{
	auto* view = GetEntity()->GetScene()->GetView();
	if (!view)
		assert(!"Not found View");

	//XMFLOAT3 focus_position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	XMFLOAT3 target = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<TransformComponent>()->GetPosition();

	if (!std::isfinite(target.x) || !std::isfinite(target.y) || !std::isfinite(target.z))
	{
		assert(!"Camera forward vec is nan or infinity");
	}
	// カメラ位置のセット
	view->SetCameraPos(camera_pos, XMFLOAT3{ target.x,target.y,target.z }, XMFLOAT3{ 0,1,0 });

	// Stateの追加
	camera_state.AddState(CameraState::DefaultTargetCamera, [this](const float delta_time) { TargetTagCamera(delta_time); });
	camera_state.AddState(CameraState::LockOnCamera, [this](const float delta_time) { LockOnCamera(delta_time); });
	camera_state.AddState(CameraState::ResetCamera, [this](const float delta_time) { ResetCamera(delta_time); });
	camera_state.AddState(CameraState::Correction_Free_LockOn, [this](const float delta_time) { Correction_Free_LockOn(delta_time); });
	camera_state.AddState(CameraState::Correction_LockOn_Free, [this](const float delta_time) {Correction_LockOn_Free(delta_time); });

	camera_state.SetState(CameraState::DefaultTargetCamera);

	// For Imgui
	state_name.push_back("DefaultTargetCamera");
	state_name.push_back("LockOnCamera");
	state_name.push_back("ResetCamera");
	state_name.push_back("Correction_Free_LockOn");
	state_name.push_back("Correction_Free_LockOn");
}

void CameraOperationComponent::NewFrame(const float delta_time)
{
	camera_angle.x = arithmetic::NormalizeAngle(camera_angle.x);
	camera_angle.y = arithmetic::NormalizeAngle(camera_angle.y);
	delta_position = camera_position - prev_camera_position;
	prev_camera_position = camera_position;
}

void CameraOperationComponent::Update(const float delta_time)
{
	auto* view = GetEntity()->GetScene()->GetView();
	if (!view)
		assert(!"Not found View");

	if (!std::isfinite(view->GetFocusPosition().x) || !std::isfinite(view->GetFocusPosition().y) || !std::isfinite(view->GetFocusPosition().z))
	{
		assert(!"Camera focus_position value is nan or infinity");
	}

	if (view->IsDebugCamera())
		return;

	FreeCamera(delta_time);

	camera_state.Update(delta_time);
}

void CameraOperationComponent::RenderImGui()
{
	ImGui::Text("Camera Stete");
	ImGui::Text("------------------");
	ImGui::Text("%s", state_name.at(static_cast<int>(camera_state.GetState())).c_str());
	ImGui::Text("------------------");
	ImGui::Text("Camera Angle");
	ImGui::Text("------------------");
	ImGui::Text("X : %.3f", camera_angle.x);
	ImGui::Text("Y : %.3f", camera_angle.y);
	ImGui::Text("------------------");
	ImGui::Text("Camera Angle");
	ImGui::Text("------------------");
	ImGui::Text("LockOn Length %.3f", lockon_length);
	ImGui::Text("------------------");
	ImGui::Text("Delta Positon");
	ImGui::Text("X :  %.3f", delta_position.x);
	ImGui::Text("Y :  %.3f", delta_position.y);
	ImGui::Text("Z :  %.3f", delta_position.z);
}

void CameraOperationComponent::CalcCameraAngle()
{
	// 初期の前方ベクトル
	const SimpleMath::Vector3 origine_front{ .0f,.0f,1.f };

	auto* view = GetEntity()->GetScene()->GetView();
	if (!view)
		assert(!"Not found View");

	{// X軸の角度算出
		SimpleMath::Vector3 vec_x{ 0,view->GetCameraFront().y,view->GetCameraFront().z };
		vec_x.Normalize();

		//float angle = arithmetic::CalcAngleFromTwoVec(origine_front, vec_x);
		//origine_front.Cross(vec_x, vec_x);
		//if (vec_x.x < 0)	camera_angle.x = -angle;
		//else				camera_angle.x = angle;

		// X軸の角度の補正要らない??
		camera_angle.x = 0;
	}

	{// Y軸の角度算出
		SimpleMath::Vector3 vec_y{ view->GetCameraFront().x,0,view->GetCameraFront().z };
		vec_y.Normalize();

		float angle = arithmetic::CalcAngleFromTwoVec(origine_front, vec_y);
		origine_front.Cross(vec_y, vec_y);
		if (vec_y.y < 0)	camera_angle.y = -angle;
		else				camera_angle.y = angle;
	}
}

void CameraOperationComponent::FreeCamera(const float delta_time)
{
	auto* view = GetEntity()->GetScene()->GetView();
	if (!view)
		assert(!"Not found View");

	auto* default_target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>();
}

// brief : 登録したEntityを中心に回るカメラ
void CameraOperationComponent::TargetTagCamera(const float delta_time)
{
	// Dodo::Input

//	auto* view = GetEntity()->GetScene()->GetView();
//	if (!view)
//		assert(!"Not found View");
//
//	auto* target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>()->GetTransform();
//
//	if (!std::isfinite(target_transform->GetPosition().x) || !std::isfinite(target_transform->GetPosition().y) || !std::isfinite(target_transform->GetPosition().z))
//	{
//		assert(!"target_transform positon is nan or infinity");
//	}
//
//	auto input = GetEntity()->GetScene()->GetPadLink();
//
//	// 回転調節値(Game Pad)
//	float stick_x = pad_input::GetAnalogStickVal(0, PadInput::AnalogStickType::Right_X_Thumb);
//	float stick_y = pad_input::GetAnalogStickVal(0, PadInput::AnalogStickType::Right_Y_Thumb);
//
//	camera_angle.x += stick_y * camera_speed_gamepad.x;
//	camera_angle.y += stick_x * camera_speed_gamepad.y;
//
//	// 回転調節値(Mouse)
//	int mouse_x = mouse_input::GetMouseVelocityX();
//	int mouse_y = mouse_input::GetMouseVelocityY();
//
//	camera_angle.x += mouse_y * camera_speed_mouse.y;
//	camera_angle.y += mouse_x * camera_speed_mouse.x;
//
//	if (camera_angle.x < camera_angle_min.x)
//	{
//		camera_angle.x = camera_angle_min.x;
//	}
//	if (camera_angle.x > camera_angle_max.x)
//	{
//		camera_angle.x = camera_angle_max.x;
//	}
//
//	SimpleMath::Vector2 sin;
//	SimpleMath::Vector2 cos;
//
//	sin.x = sinf(XMConvertToRadians(camera_angle.x));
//	cos.x = cosf(XMConvertToRadians(camera_angle.x));
//	sin.y = sinf(XMConvertToRadians(camera_angle.y));
//	cos.y = cosf(XMConvertToRadians(camera_angle.y));
//
//	// 回転後のベクトルを算出
//	SimpleMath::Vector3 front{ -cos.x * sin.y,-sin.x,-cos.x * cos.y };	// 回転後のベクトル
//	SimpleMath::Vector3 right{ cos.y,0.0f,-sin.y };
//	SimpleMath::Vector3 up = arithmetic::CalcUpVec(-front, right);
//
//	if (!std::isfinite(front.x) || !std::isfinite(front.y) || !std::isfinite(front.z))
//	{
//		assert(!"Camera forward vec is nan or infinity");
//	}
//
//	SimpleMath::Vector3 zero;
//	zero.Zero;
//
//	if (up == zero || front == zero || right == zero)
//	{
//		up.y = 1;
//	}
//
//	view->SetCameraFront(-front);
//	view->SetCameraRight(right);
//	view->SetCameraUp(up);
//
//	if (!std::isfinite(front.x) || !std::isfinite(front.y) || !std::isfinite(front.z))
//	{
//		assert(!"Camera forward vec is nan or infinity");
//	}
//
//	if (input->Trg(InputType::GamePad::Left_Shoulder, 0))
//	{
//		camera_state.SetState(CameraState::Correction_Free_LockOn);
//		//GetEntity()->GetScene()->GetOtherEntity(EntityTag::UI_TargetLockOn)->GetComponent<UI_LockOnComponent>()->ActiveLockOn();
//	}
//
//	// カメラ位置設定
//	auto& target_pos = target_transform->GetPosition();
//	camera_position = target_pos + front * default_camera_length;
//
//	view->SetEyePosition(camera_position);
//
//	if (!std::isfinite(camera_position.x) || !std::isfinite(camera_position.y) || !std::isfinite(camera_position.z))
//	{
//		assert(!"Camera forward vec is nan or infinity");
//	}
//
//	// カメラの注視点設定
//	auto* default_target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>()->GetTransform();
//	SimpleMath::Vector3 camera_target{ default_target_transform->GetPosition() + adjust_target_position };
//	view->SetFocusPosition(camera_target);
}
//
void CameraOperationComponent::LockOnCamera(const float delta_time)
{
//	auto* view = GetEntity()->GetScene()->GetView();
//	if (!view)
//		assert(!"Not found View");
//
//	auto* default_target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>()->GetTransform();
//
//	auto* target_transform = GetEntity()->GetScene()->GetOtherEntity(other_target_tag)->GetComponent<TransformComponent>()->GetTransform();
//
//	if (!std::isfinite(default_target_transform->GetPosition().x) || !std::isfinite(default_target_transform->GetPosition().y) || !std::isfinite(default_target_transform->GetPosition().z))
//	{
//		assert(!"target_transform positon is nan or infinity");
//	}
//
//	// Target tag → Default focus_position tag
//	SimpleMath::Vector3 vec_from_target_to_default
//	{ default_target_transform->GetPosition().x - target_transform->GetPosition().x,default_target_transform->GetPosition().y - target_transform->GetPosition().y,default_target_transform->GetPosition().z - target_transform->GetPosition().z };
//
//	lockon_length = vec_from_target_to_default.Length();
//	vec_from_target_to_default.Normalize();
//	//SimpleMath::Vector3 ajust_pos = vec_from_target_to_default * default_camera_length + ajust_camera_position;	// カメラ位置の調整
//	SimpleMath::Vector3 ajust_pos = default_target_transform->GetPosition() + vec_from_target_to_default * default_camera_length + ajust_camera_position;	// カメラ位置の調整
//
//	// 調整後のベクトル
//	SimpleMath::Vector3 ajust_vec
//	{ ajust_pos.x - default_target_transform->GetPosition().x, ajust_pos.y - default_target_transform->GetPosition().y, ajust_pos.z - default_target_transform->GetPosition().z };
//	ajust_vec.Normalize();
//
//	// カメラ位置設定
//	camera_position = default_target_transform->GetPosition() + ajust_vec * default_camera_length;
//
//	// カメラの注視点設定
//	SimpleMath::Vector3 camera_target{ target_transform->GetPosition() };
//	view->SetFocusPosition(camera_target);
//
//	// 前のフレームのカメラ位置との角度を算出
//	float dot_val = camera_position.Dot(prev_camera_position);
//	if (dot_val < 0)
//	{
//		view->SetEyePosition(prev_camera_position);
//	}
//	else
//	{
//		view->SetEyePosition(camera_position);
//	}
//
//	if (!std::isfinite(prev_camera_position.x) || !std::isfinite(prev_camera_position.y) || !std::isfinite(prev_camera_position.z))
//	{
//		assert(!"Camera forward vector is nan or infinity");
//	}
//
//	if (!std::isfinite(camera_position.x) || !std::isfinite(camera_position.y) || !std::isfinite(camera_position.z))
//	{
//		assert(!"Camera forward vector is nan or infinity");
//	}
//
//	if (lockon_length < lock_on_cancel_length)
//	{
//		camera_state.SetState(CameraState::DefaultTargetCamera);
//	}
//
//	// カメラの位置をY-UP固定
//	view->SetCameraUp(XMFLOAT3{ 0,1,0 });
//
//	auto input = GetEntity()->GetScene()->GetPadLink();
//	if (input->Trg(InputType::GamePad::Left_Shoulder, 0))
//	{
//		camera_state.SetState(CameraState::Correction_LockOn_Free);
//	}
//
//	CalcCameraAngle();
}

void CameraOperationComponent::ResetCamera(const float delta_time)
{
	// Todo::Input
	//auto* view = GetEntity()->GetScene()->GetView();
	//if (!view)
	//	assert(!"Not found View");

	//auto input = GetEntity()->GetScene()->GetPadLink();
}

// フリーカメラ → ロックオンカメラの補完
void CameraOperationComponent::Correction_Free_LockOn(const float delta_time)
{
	//auto* view = GetEntity()->GetScene()->GetView();
	//if (!view)
	//	assert(!"Not found View");

	//auto* default_target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>()->GetTransform();
	//if (!default_target_transform)
	//	assert(!"Not found Component");

	//auto* target_transform = GetEntity()->GetScene()->GetOtherEntity(other_target_tag)->GetComponent<TransformComponent>()->GetTransform();
	//if (!target_transform)
	//	assert(!"Not found Component");

	//SimpleMath::Vector3 before_correction_vec	// 補完前のベクトル
	//	= { camera_position - default_target_transform->GetPosition() };
	//SimpleMath::Vector3 after_correction_vec;	// 補完後のベクトル
	//{
	//	SimpleMath::Vector3 vec	// other_target_tag → my_target_tag (vector)
	//		= { default_target_transform->GetPosition() - target_transform->GetPosition() };
	//	vec.Normalize();

	//	SimpleMath::Vector3 ajust_pos	// 変換後の位置(長さの調整前)
	//		= default_target_transform->GetPosition() + (vec * default_camera_length) + ajust_camera_position;

	//	SimpleMath::Vector3 after_vec	// 変換後の位置(長さの調整済み)
	//		= ajust_pos - default_target_transform->GetPosition();
	//	after_vec.Normalize();

	//	after_correction_vec = after_vec * default_camera_length;
	//}

	//before_correction_vec.Normalize();
	//after_correction_vec.Normalize();
	//SimpleMath::Vector3 current_vec = before_correction_vec.SmoothStep(before_correction_vec, after_correction_vec, current_timer);
	//camera_position = default_target_transform->GetPosition() + current_vec * default_camera_length;
	//view->SetEyePosition(camera_position);

	//current_timer += delta_time / correction_timer;

	//if (current_timer > 1)
	//{
	//	current_timer = 0;
	//	camera_state.SetState(CameraState::LockOnCamera);
	//}

	//if (!std::isfinite(camera_position.x) || !std::isfinite(camera_position.y) || !std::isfinite(camera_position.z))
	//{
	//	assert(!"Camera forward vector is nan or infinity");
	//}

	//// カメラの位置を+ ajust_target_positionY-UP固定
	//view->SetCameraUp(XMFLOAT3{ 0,1,0 });

	//CalcCameraAngle();

	//auto input = GetEntity()->GetScene()->GetPadLink();
	//if (input->Trg(InputType::GamePad::Left_Shoulder, 0))
	//{
	//	camera_state.SetState(CameraState::DefaultTargetCamera);
	//	current_timer = 0;
	//}

	//// カメラの注視点設定
	//SimpleMath::Vector3 camera_target{ target_transform->GetPosition() };
	//view->SetFocusPosition(target_transform->GetPosition());
}

// ロックオンカメラ → フリーカメラの補完
void CameraOperationComponent::Correction_LockOn_Free(const float delta_time)
{
	SimpleMath::Vector2 sin;
	SimpleMath::Vector2 cos;

	sin.x = sinf(XMConvertToRadians(camera_angle_max.x));
	cos.x = cosf(XMConvertToRadians(camera_angle_max.x));
	sin.y = sinf(XMConvertToRadians(camera_angle.y));
	cos.y = cosf(XMConvertToRadians(camera_angle.y));

	// 回転後のベクトルを算出
	SimpleMath::Vector3 front{ -cos.x * sin.y,-sin.x,-cos.x * cos.y };	// 回転後のベクトル
	SimpleMath::Vector3 right{ cos.y,0.0f,-sin.y };
	SimpleMath::Vector3 up = arithmetic::CalcUpVec(-front, right);

	if (!std::isfinite(front.x) || !std::isfinite(front.y) || !std::isfinite(front.z))
	{
		assert(!"Camera forward vec is nan or infinity");
	}

	SimpleMath::Vector3 zero;
	zero.Zero;

	if (up == zero || front == zero || right == zero)
	{
		up.y = 1;
	}

	auto* view = GetEntity()->GetScene()->GetView();
	if (!view)
		assert(!"Not found View");
	view->SetCameraFront(-front);
	view->SetCameraRight(right);
	view->SetCameraUp(up);

	auto* default_target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>();
	SimpleMath::Vector3 before_correction_vec	// 補完前のベクトル
		= { camera_position - default_target_transform->GetPosition() };
	before_correction_vec.Normalize();
	SimpleMath::Vector3 after_correction_vec;	// 補完後のベクトル
	{
		SimpleMath::Vector3 after_vec	// 変換後の位置(長さの調整済み)
			= front;
		after_vec.Normalize();

		after_correction_vec = after_vec;
		after_correction_vec.Normalize();
	}

	// カメラ位置設定
	SimpleMath::Vector3 current_vec = after_correction_vec.SmoothStep(before_correction_vec, after_correction_vec, current_timer);
	current_vec.Normalize();
	camera_position = default_target_transform->GetPosition() + current_vec * default_camera_length;

	view->SetEyePosition(camera_position);

	current_timer += delta_time / correction_timer;

	if (current_timer > 1)
	{
		current_timer = 0;
		camera_state.SetState(CameraState::DefaultTargetCamera);
	}

	if (!std::isfinite(camera_position.x) || !std::isfinite(camera_position.y) || !std::isfinite(camera_position.z))
	{
		assert(!"Camera forward vec is nan or infinity");
	}

	CalcCameraAngle();

	// カメラの注視点設定
	SimpleMath::Vector3 camera_target{ default_target_transform->GetPosition() + adjust_target_position };
	view->SetFocusPosition(camera_target);
}

bool CameraOperationComponent::MovingRestriction(DirectX::SimpleMath::Vector3 position)
{
	auto* view = GetEntity()->GetScene()->GetView();
	if (!view)
		assert(!"Not found View");

	const float dis_x = view->GetEyePosition().x - 0;
	const float dis_y = view->GetEyePosition().y - 0;
	const float dis_z = view->GetEyePosition().z - 0;

	const float dis = sqrtf(dis_x * dis_x + dis_y * dis_y + dis_z * dis_z) + camera_radius;

	if (dis < moving_restriction_length)
		return false;

	return true;
}


namespace cumulonimbus::component
{
	CameraOperationComponent::CameraOperationComponent(ecs::Registry* const registry, const ecs::Entity ent,
													   EntityTag tag, XMFLOAT3 camera_pos, float default_camera_length)
		: ComponentBase{ registry, ent }
		, my_target_tag{ tag }
		, camera_position{ camera_pos }
		, default_camera_length{ default_camera_length }
	{
		auto* view = GetRegistry()->GetScene()->GetView();
		if (!view)
			assert(!"Not found View");

		//XMFLOAT3 focus_position = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
		XMFLOAT3 target = GetRegistry()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<TransformComponent>()->GetPosition();

		if (!std::isfinite(target.x) || !std::isfinite(target.y) || !std::isfinite(target.z))
		{
			assert(!"Camera forward vec is nan or infinity");
		}
		// カメラ位置のセット
		view->SetCameraPos(camera_pos, XMFLOAT3{ target.x,target.y,target.z }, XMFLOAT3{ 0,1,0 });

		// Stateの追加
		camera_state.AddState(CameraState::DefaultTargetCamera, [this](const float delta_time) { TargetTagCamera(delta_time); });
		camera_state.AddState(CameraState::LockOnCamera, [this](const float delta_time) { LockOnCamera(delta_time); });
		camera_state.AddState(CameraState::ResetCamera, [this](const float delta_time) { ResetCamera(delta_time); });
		camera_state.AddState(CameraState::Correction_Free_LockOn, [this](const float delta_time) { Correction_Free_LockOn(delta_time); });
		camera_state.AddState(CameraState::Correction_LockOn_Free, [this](const float delta_time) {Correction_LockOn_Free(delta_time); });

		camera_state.SetState(CameraState::DefaultTargetCamera);

		// For Imgui
		state_name.push_back("DefaultTargetCamera");
		state_name.push_back("LockOnCamera");
		state_name.push_back("ResetCamera");
		state_name.push_back("Correction_Free_LockOn");
		state_name.push_back("Correction_Free_LockOn");
	}

	void CameraOperationComponent::NewFrame(const float delta_time)
	{
		camera_angle.x = arithmetic::NormalizeAngle(camera_angle.x);
		camera_angle.y = arithmetic::NormalizeAngle(camera_angle.y);
		delta_position = camera_position - prev_camera_position;
		prev_camera_position = camera_position;
	}

	void CameraOperationComponent::Update(const float delta_time)
	{
		auto* view = GetRegistry()->GetScene()->GetView();
		if (!view)
			assert(!"Not found View");

		if (!std::isfinite(view->GetFocusPosition().x) || !std::isfinite(view->GetFocusPosition().y) || !std::isfinite(view->GetFocusPosition().z))
		{
			assert(!"Camera focus_position value is nan or infinity");
		}

		if (view->IsDebugCamera())
			return;

		FreeCamera(delta_time);

		camera_state.Update(delta_time);
	}

	void CameraOperationComponent::RenderImGui()
	{
		ImGui::Text("Camera Stete");
		ImGui::Text("------------------");
		ImGui::Text("%s", state_name.at(static_cast<int>(camera_state.GetState())).c_str());
		ImGui::Text("------------------");
		ImGui::Text("Camera Angle");
		ImGui::Text("------------------");
		ImGui::Text("X : %.3f", camera_angle.x);
		ImGui::Text("Y : %.3f", camera_angle.y);
		ImGui::Text("------------------");
		ImGui::Text("Camera Angle");
		ImGui::Text("------------------");
		ImGui::Text("LockOn Length %.3f", lockon_length);
		ImGui::Text("------------------");
		ImGui::Text("Delta Positon");
		ImGui::Text("X :  %.3f", delta_position.x);
		ImGui::Text("Y :  %.3f", delta_position.y);
		ImGui::Text("Z :  %.3f", delta_position.z);
	}

	void CameraOperationComponent::CalcCameraAngle()
	{
		// 初期の前方ベクトル
		const SimpleMath::Vector3 origine_front{ .0f,.0f,1.f };

		auto* view = GetRegistry()->GetScene()->GetView();
		if (!view)
			assert(!"Not found View");

		{// X軸の角度算出
			SimpleMath::Vector3 vec_x{ 0,view->GetCameraFront().y,view->GetCameraFront().z };
			vec_x.Normalize();

			//float angle = arithmetic::CalcAngleFromTwoVec(origine_front, vec_x);
			//origine_front.Cross(vec_x, vec_x);
			//if (vec_x.x < 0)	camera_angle.x = -angle;
			//else				camera_angle.x = angle;

			// X軸の角度の補正要らない??
			camera_angle.x = 0;
		}

		{// Y軸の角度算出
			SimpleMath::Vector3 vec_y{ view->GetCameraFront().x,0,view->GetCameraFront().z };
			vec_y.Normalize();

			float angle = arithmetic::CalcAngleFromTwoVec(origine_front, vec_y);
			origine_front.Cross(vec_y, vec_y);
			if (vec_y.y < 0)	camera_angle.y = -angle;
			else				camera_angle.y = angle;
		}
	}

	void CameraOperationComponent::FreeCamera(const float delta_time)
	{
		auto* view = GetRegistry()->GetScene()->GetView();
		if (!view)
			assert(!"Not found View");

		auto* default_target_transform = GetRegistry()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>();
	}

	// brief : 登録したEntityを中心に回るカメラ
	void CameraOperationComponent::TargetTagCamera(const float delta_time)
	{
		// Dodo::Input

	//	auto* view = GetEntity()->GetScene()->GetView();
	//	if (!view)
	//		assert(!"Not found View");
	//
	//	auto* target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>()->GetTransform();
	//
	//	if (!std::isfinite(target_transform->GetPosition().x) || !std::isfinite(target_transform->GetPosition().y) || !std::isfinite(target_transform->GetPosition().z))
	//	{
	//		assert(!"target_transform positon is nan or infinity");
	//	}
	//
	//	auto input = GetEntity()->GetScene()->GetPadLink();
	//
	//	// 回転調節値(Game Pad)
	//	float stick_x = pad_input::GetAnalogStickVal(0, PadInput::AnalogStickType::Right_X_Thumb);
	//	float stick_y = pad_input::GetAnalogStickVal(0, PadInput::AnalogStickType::Right_Y_Thumb);
	//
	//	camera_angle.x += stick_y * camera_speed_gamepad.x;
	//	camera_angle.y += stick_x * camera_speed_gamepad.y;
	//
	//	// 回転調節値(Mouse)
	//	int mouse_x = mouse_input::GetMouseVelocityX();
	//	int mouse_y = mouse_input::GetMouseVelocityY();
	//
	//	camera_angle.x += mouse_y * camera_speed_mouse.y;
	//	camera_angle.y += mouse_x * camera_speed_mouse.x;
	//
	//	if (camera_angle.x < camera_angle_min.x)
	//	{
	//		camera_angle.x = camera_angle_min.x;
	//	}
	//	if (camera_angle.x > camera_angle_max.x)
	//	{
	//		camera_angle.x = camera_angle_max.x;
	//	}
	//
	//	SimpleMath::Vector2 sin;
	//	SimpleMath::Vector2 cos;
	//
	//	sin.x = sinf(XMConvertToRadians(camera_angle.x));
	//	cos.x = cosf(XMConvertToRadians(camera_angle.x));
	//	sin.y = sinf(XMConvertToRadians(camera_angle.y));
	//	cos.y = cosf(XMConvertToRadians(camera_angle.y));
	//
	//	// 回転後のベクトルを算出
	//	SimpleMath::Vector3 front{ -cos.x * sin.y,-sin.x,-cos.x * cos.y };	// 回転後のベクトル
	//	SimpleMath::Vector3 right{ cos.y,0.0f,-sin.y };
	//	SimpleMath::Vector3 up = arithmetic::CalcUpVec(-front, right);
	//
	//	if (!std::isfinite(front.x) || !std::isfinite(front.y) || !std::isfinite(front.z))
	//	{
	//		assert(!"Camera forward vec is nan or infinity");
	//	}
	//
	//	SimpleMath::Vector3 zero;
	//	zero.Zero;
	//
	//	if (up == zero || front == zero || right == zero)
	//	{
	//		up.y = 1;
	//	}
	//
	//	view->SetCameraFront(-front);
	//	view->SetCameraRight(right);
	//	view->SetCameraUp(up);
	//
	//	if (!std::isfinite(front.x) || !std::isfinite(front.y) || !std::isfinite(front.z))
	//	{
	//		assert(!"Camera forward vec is nan or infinity");
	//	}
	//
	//	if (input->Trg(InputType::GamePad::Left_Shoulder, 0))
	//	{
	//		camera_state.SetState(CameraState::Correction_Free_LockOn);
	//		//GetEntity()->GetScene()->GetOtherEntity(EntityTag::UI_TargetLockOn)->GetComponent<UI_LockOnComponent>()->ActiveLockOn();
	//	}
	//
	//	// カメラ位置設定
	//	auto& target_pos = target_transform->GetPosition();
	//	camera_position = target_pos + front * default_camera_length;
	//
	//	view->SetEyePosition(camera_position);
	//
	//	if (!std::isfinite(camera_position.x) || !std::isfinite(camera_position.y) || !std::isfinite(camera_position.z))
	//	{
	//		assert(!"Camera forward vec is nan or infinity");
	//	}
	//
	//	// カメラの注視点設定
	//	auto* default_target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>()->GetTransform();
	//	SimpleMath::Vector3 camera_target{ default_target_transform->GetPosition() + adjust_target_position };
	//	view->SetFocusPosition(camera_target);
	}
	//
	void CameraOperationComponent::LockOnCamera(const float delta_time)
	{
		//	auto* view = GetEntity()->GetScene()->GetView();
		//	if (!view)
		//		assert(!"Not found View");
		//
		//	auto* default_target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>()->GetTransform();
		//
		//	auto* target_transform = GetEntity()->GetScene()->GetOtherEntity(other_target_tag)->GetComponent<TransformComponent>()->GetTransform();
		//
		//	if (!std::isfinite(default_target_transform->GetPosition().x) || !std::isfinite(default_target_transform->GetPosition().y) || !std::isfinite(default_target_transform->GetPosition().z))
		//	{
		//		assert(!"target_transform positon is nan or infinity");
		//	}
		//
		//	// Target tag → Default focus_position tag
		//	SimpleMath::Vector3 vec_from_target_to_default
		//	{ default_target_transform->GetPosition().x - target_transform->GetPosition().x,default_target_transform->GetPosition().y - target_transform->GetPosition().y,default_target_transform->GetPosition().z - target_transform->GetPosition().z };
		//
		//	lockon_length = vec_from_target_to_default.Length();
		//	vec_from_target_to_default.Normalize();
		//	//SimpleMath::Vector3 ajust_pos = vec_from_target_to_default * default_camera_length + ajust_camera_position;	// カメラ位置の調整
		//	SimpleMath::Vector3 ajust_pos = default_target_transform->GetPosition() + vec_from_target_to_default * default_camera_length + ajust_camera_position;	// カメラ位置の調整
		//
		//	// 調整後のベクトル
		//	SimpleMath::Vector3 ajust_vec
		//	{ ajust_pos.x - default_target_transform->GetPosition().x, ajust_pos.y - default_target_transform->GetPosition().y, ajust_pos.z - default_target_transform->GetPosition().z };
		//	ajust_vec.Normalize();
		//
		//	// カメラ位置設定
		//	camera_position = default_target_transform->GetPosition() + ajust_vec * default_camera_length;
		//
		//	// カメラの注視点設定
		//	SimpleMath::Vector3 camera_target{ target_transform->GetPosition() };
		//	view->SetFocusPosition(camera_target);
		//
		//	// 前のフレームのカメラ位置との角度を算出
		//	float dot_val = camera_position.Dot(prev_camera_position);
		//	if (dot_val < 0)
		//	{
		//		view->SetEyePosition(prev_camera_position);
		//	}
		//	else
		//	{
		//		view->SetEyePosition(camera_position);
		//	}
		//
		//	if (!std::isfinite(prev_camera_position.x) || !std::isfinite(prev_camera_position.y) || !std::isfinite(prev_camera_position.z))
		//	{
		//		assert(!"Camera forward vector is nan or infinity");
		//	}
		//
		//	if (!std::isfinite(camera_position.x) || !std::isfinite(camera_position.y) || !std::isfinite(camera_position.z))
		//	{
		//		assert(!"Camera forward vector is nan or infinity");
		//	}
		//
		//	if (lockon_length < lock_on_cancel_length)
		//	{
		//		camera_state.SetState(CameraState::DefaultTargetCamera);
		//	}
		//
		//	// カメラの位置をY-UP固定
		//	view->SetCameraUp(XMFLOAT3{ 0,1,0 });
		//
		//	auto input = GetEntity()->GetScene()->GetPadLink();
		//	if (input->Trg(InputType::GamePad::Left_Shoulder, 0))
		//	{
		//		camera_state.SetState(CameraState::Correction_LockOn_Free);
		//	}
		//
		//	CalcCameraAngle();
	}

	void CameraOperationComponent::ResetCamera(const float delta_time)
	{
		// Todo::Input
		//auto* view = GetEntity()->GetScene()->GetView();
		//if (!view)
		//	assert(!"Not found View");

		//auto input = GetEntity()->GetScene()->GetPadLink();
	}

	// フリーカメラ → ロックオンカメラの補完
	void CameraOperationComponent::Correction_Free_LockOn(const float delta_time)
	{
		//auto* view = GetEntity()->GetScene()->GetView();
		//if (!view)
		//	assert(!"Not found View");

		//auto* default_target_transform = GetEntity()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>()->GetTransform();
		//if (!default_target_transform)
		//	assert(!"Not found Component");

		//auto* target_transform = GetEntity()->GetScene()->GetOtherEntity(other_target_tag)->GetComponent<TransformComponent>()->GetTransform();
		//if (!target_transform)
		//	assert(!"Not found Component");

		//SimpleMath::Vector3 before_correction_vec	// 補完前のベクトル
		//	= { camera_position - default_target_transform->GetPosition() };
		//SimpleMath::Vector3 after_correction_vec;	// 補完後のベクトル
		//{
		//	SimpleMath::Vector3 vec	// other_target_tag → my_target_tag (vector)
		//		= { default_target_transform->GetPosition() - target_transform->GetPosition() };
		//	vec.Normalize();

		//	SimpleMath::Vector3 ajust_pos	// 変換後の位置(長さの調整前)
		//		= default_target_transform->GetPosition() + (vec * default_camera_length) + ajust_camera_position;

		//	SimpleMath::Vector3 after_vec	// 変換後の位置(長さの調整済み)
		//		= ajust_pos - default_target_transform->GetPosition();
		//	after_vec.Normalize();

		//	after_correction_vec = after_vec * default_camera_length;
		//}

		//before_correction_vec.Normalize();
		//after_correction_vec.Normalize();
		//SimpleMath::Vector3 current_vec = before_correction_vec.SmoothStep(before_correction_vec, after_correction_vec, current_timer);
		//camera_position = default_target_transform->GetPosition() + current_vec * default_camera_length;
		//view->SetEyePosition(camera_position);

		//current_timer += delta_time / correction_timer;

		//if (current_timer > 1)
		//{
		//	current_timer = 0;
		//	camera_state.SetState(CameraState::LockOnCamera);
		//}

		//if (!std::isfinite(camera_position.x) || !std::isfinite(camera_position.y) || !std::isfinite(camera_position.z))
		//{
		//	assert(!"Camera forward vector is nan or infinity");
		//}

		//// カメラの位置を+ ajust_target_positionY-UP固定
		//view->SetCameraUp(XMFLOAT3{ 0,1,0 });

		//CalcCameraAngle();

		//auto input = GetEntity()->GetScene()->GetPadLink();
		//if (input->Trg(InputType::GamePad::Left_Shoulder, 0))
		//{
		//	camera_state.SetState(CameraState::DefaultTargetCamera);
		//	current_timer = 0;
		//}

		//// カメラの注視点設定
		//SimpleMath::Vector3 camera_target{ target_transform->GetPosition() };
		//view->SetFocusPosition(target_transform->GetPosition());
	}

	// ロックオンカメラ → フリーカメラの補完
	void CameraOperationComponent::Correction_LockOn_Free(const float delta_time)
	{
		SimpleMath::Vector2 sin;
		SimpleMath::Vector2 cos;

		sin.x = sinf(XMConvertToRadians(camera_angle_max.x));
		cos.x = cosf(XMConvertToRadians(camera_angle_max.x));
		sin.y = sinf(XMConvertToRadians(camera_angle.y));
		cos.y = cosf(XMConvertToRadians(camera_angle.y));

		// 回転後のベクトルを算出
		SimpleMath::Vector3 front{ -cos.x * sin.y,-sin.x,-cos.x * cos.y };	// 回転後のベクトル
		SimpleMath::Vector3 right{ cos.y,0.0f,-sin.y };
		SimpleMath::Vector3 up = arithmetic::CalcUpVec(-front, right);

		if (!std::isfinite(front.x) || !std::isfinite(front.y) || !std::isfinite(front.z))
		{
			assert(!"Camera forward vec is nan or infinity");
		}

		SimpleMath::Vector3 zero;
		zero.Zero;

		if (up == zero || front == zero || right == zero)
		{
			up.y = 1;
		}

		auto* view = GetRegistry()->GetScene()->GetView();
		if (!view)
			assert(!"Not found View");
		view->SetCameraFront(-front);
		view->SetCameraRight(right);
		view->SetCameraUp(up);

		auto* default_target_transform = GetRegistry()->GetScene()->GetOtherEntity(my_target_tag)->GetComponent<TransformComponent>();
		SimpleMath::Vector3 before_correction_vec	// 補完前のベクトル
			= { camera_position - default_target_transform->GetPosition() };
		before_correction_vec.Normalize();
		SimpleMath::Vector3 after_correction_vec;	// 補完後のベクトル
		{
			SimpleMath::Vector3 after_vec	// 変換後の位置(長さの調整済み)
				= front;
			after_vec.Normalize();

			after_correction_vec = after_vec;
			after_correction_vec.Normalize();
		}

		// カメラ位置設定
		SimpleMath::Vector3 current_vec = after_correction_vec.SmoothStep(before_correction_vec, after_correction_vec, current_timer);
		current_vec.Normalize();
		camera_position = default_target_transform->GetPosition() + current_vec * default_camera_length;

		view->SetEyePosition(camera_position);

		current_timer += delta_time / correction_timer;

		if (current_timer > 1)
		{
			current_timer = 0;
			camera_state.SetState(CameraState::DefaultTargetCamera);
		}

		if (!std::isfinite(camera_position.x) || !std::isfinite(camera_position.y) || !std::isfinite(camera_position.z))
		{
			assert(!"Camera forward vec is nan or infinity");
		}

		CalcCameraAngle();

		// カメラの注視点設定
		SimpleMath::Vector3 camera_target{ default_target_transform->GetPosition() + adjust_target_position };
		view->SetFocusPosition(camera_target);
	}

	bool CameraOperationComponent::MovingRestriction(DirectX::SimpleMath::Vector3 position)
	{
		auto* view = GetRegistry()->GetScene()->GetView();
		if (!view)
			assert(!"Not found View");

		const float dis_x = view->GetEyePosition().x - 0;
		const float dis_y = view->GetEyePosition().y - 0;
		const float dis_z = view->GetEyePosition().z - 0;

		const float dis = sqrtf(dis_x * dis_x + dis_y * dis_y + dis_z * dis_z) + camera_radius;

		if (dis < moving_restriction_length)
			return false;

		return true;
	}
}