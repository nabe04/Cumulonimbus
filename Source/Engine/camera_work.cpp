#include "camera_work.h"

#include <imgui.h>

#include "arithmetic.h"
#include "locator.h"
#include "ecs.h"
#include "transform_component.h"

CameraWork::CameraWork(cumulonimbus::ecs::Registry* registry)
	:registry{ registry }
{

}


void CameraWork::Update(float dt)
{
	CalcCameraDirectionalVector();
	CalcCameraAngle(); //オイラー角で(現在)計算しているので今は使わない

	if(is_use_camera_for_object)
	{
		UpdateObjectCamera(dt);
	}
	else
	{
		UpdateDefaultCamera(dt);
	}
}

void CameraWork::RenderImGui()
{
	ImGui::Text("focus_position x : %f\nfocus_position y : %f\nfocus_position z : %f", focus_position.x, focus_position.y, focus_position.z);
	ImGui::Text("angle x  : %f\nangle y  : %f\nangle z  : %f", camera_angle.x, camera_angle.y, camera_angle.z);
	ImGui::Text("Pos x  : %f\n Pos y  : %f\n Pos z  : %f", eye_position.x, eye_position.y, eye_position.z);

	ImGui::DragFloat2("CameraSpeed", (float*)&camera_velocity, 0.5f, 1, 10);
}

void CameraWork::UpdateDefaultCamera(float dt)
{
	const auto& mouse = cumulonimbus::locator::Locator::GetInput()->Mouse();
	const auto& key = cumulonimbus::locator::Locator::GetInput()->Keyboard();

	if (mouse.GetState(MouseButton::Left) == ButtonState::Held &&
		mouse.GetState(MouseButton::Right) == ButtonState::Held)
	{
		Track(static_cast<float>(mouse.DeltaX()), right_vec);
		Crane(static_cast<float>(-mouse.DeltaY()), { 0,1,0 });
	}
	else if (mouse.GetState(MouseButton::Left) == ButtonState::Held)
	{
		DollyInOut(static_cast<float>(mouse.DeltaY()));
		Pan(static_cast<float>(mouse.DeltaX()));

		if (key.GetState(Keycode::D) == ButtonState::Held)
			Track(camera_velocity.x, right_vec);
		if (key.GetState(Keycode::A) == ButtonState::Held)
			Track(-camera_velocity.x, right_vec);
		if (key.GetState(Keycode::W) == ButtonState::Held)
			Crane(camera_velocity.y, up_vec);
		if (key.GetState(Keycode::S) == ButtonState::Held)
			Crane(-camera_velocity.y, up_vec);
	}
	else if (mouse.GetState(MouseButton::Right) == ButtonState::Held)
	{
		Pan(static_cast<float>(mouse.DeltaX()));
		Tilt(static_cast<float>(mouse.DeltaY()));

		if (key.GetState(Keycode::D) == ButtonState::Held)
			Track(camera_velocity.x, right_vec);
		if (key.GetState(Keycode::A) == ButtonState::Held)
			Track(-camera_velocity.x, right_vec);
		if (key.GetState(Keycode::W) == ButtonState::Held)
			Crane(camera_velocity.y, up_vec);
		if (key.GetState(Keycode::S) == ButtonState::Held)
			Crane(-camera_velocity.y, up_vec);
	}
}

void CameraWork::UpdateObjectCamera(float dt)
{

}

void CameraWork::SetCameraUpRightFrontVector(const DirectX::SimpleMath::Vector3& up,
											 const DirectX::SimpleMath::Vector3& right,
											 const DirectX::SimpleMath::Vector3& front)
{
	up_vec    = up;
	right_vec = right;
	front_vec = front;
}


void CameraWork::SetCameraInfo( const DirectX::SimpleMath::Vector3& eye_position,
								const DirectX::SimpleMath::Vector3& target,
								const DirectX::SimpleMath::Vector3& up_vec)
{
	this->eye_position   = eye_position;
	this->focus_position = target;
	this->up_vec		 = up_vec;
}

void CameraWork::SetCameraInfo(const View& v)
{
	eye_position   = v.GetEyePosition();
	focus_position = v.GetFocusPosition();

	SetCameraUpRightFrontVector(v.GetCameraUp(), v.GetCameraRight(), v.GetCameraFront());
}

void CameraWork::SetPosition(const DirectX::SimpleMath::Vector3& eye_position)
{
	this->eye_position = eye_position;
}

void CameraWork::SetTargetVec(const DirectX::SimpleMath::Vector3& target)
{
	if (this->focus_position.x == this->focus_position.y == this->focus_position.z == 0.0f)
		assert(!"Vector is zero");

	this->focus_position = target;
}

void CameraWork::SetCameraUp(const DirectX::SimpleMath::Vector3& up)
{
	// HACK: イコール関数を作ったら変更する
	if (this->up_vec.x == this->up_vec.y == this->up_vec.z == 0.0f)
		assert(!"Vector is zero");

	this->up_vec = up;
}

void CameraWork::AttachObject(
	cumulonimbus::mapping::rename_type::Entity ent,
	bool switch_object_camera)
{
	attach_entity = ent;
	is_use_camera_for_object = switch_object_camera;
}

void CameraWork::InitializeObjectCameraParam(float camera_length)
{
	this->camera_length = camera_length;
	auto& transform_comp = registry->GetComponent<cumulonimbus::component::TransformComponent>(attach_entity);
	// カメラの位置をエンティティの持つオブジェクトの後方にセット
	eye_position   = transform_comp.GetPosition() + (transform_comp.GetModelFront() * -1 * camera_length);
	// 注視点をエンティティの位置にセット
	focus_position = transform_comp.GetPosition();
}


void CameraWork::CalcCameraDirectionalVector()
{
	front_vec	 = DirectX::SimpleMath::Vector3{ focus_position - eye_position };
	camera_right = arithmetic::CalcRightVec(up_vec, front_vec);
	up_vec		 = arithmetic::CalcUpVec(front_vec, camera_right);

	front_vec.Normalize();
	camera_right.Normalize();
	current_camera_up.Normalize();
}

void CameraWork::CalcCameraAngle()
{
	using namespace DirectX::SimpleMath;

	const Vector3 right{ 1,0,0 };
	const Vector3 up{ 0,1,0 };
	const Vector3 front{ 0,0,1 };

	camera_angle.x = DirectX::XMConvertToDegrees(acosf(up.Dot(up_vec)));
	if (up.Cross(up_vec).x < 0)
		camera_angle.x *= -1;
	camera_angle.y = DirectX::XMConvertToDegrees(acosf(front.Dot(front_vec)));
	if (front.Cross(front_vec).y < 0)
		camera_angle.y *= -1;

	camera_angle.z = arithmetic::CalcAngle_Z(front_vec);
}


/*
 * brief :  カメラの左右の傾き(位置は固定)
 */
void CameraWork::Pan(const float velocity)
{
	// クォータニオン Ver
	DirectX::SimpleMath::Quaternion q{};
	q = DirectX::SimpleMath::Quaternion::Identity;
	q.Normalize();
	q = q.CreateFromAxisAngle(up_vec, DirectX::XMConvertToRadians(velocity * camera_velocity.x * 0.1f));
	front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
	front_vec.Normalize();

	focus_position = eye_position + front_vec;

	CalcCameraDirectionalVector();
}

/*
 * brief : カメラの上下の傾き(位置は固定)
 *       : 上下に+-max_camera_angle.x度(85度)まで傾くようにする
 */
void CameraWork::Tilt(const float velocity)
{
	// クォータニオン Ver
	DirectX::SimpleMath::Quaternion q{};
	q = DirectX::SimpleMath::Quaternion::Identity;
	q.Normalize();
	q = q.CreateFromAxisAngle(right_vec, DirectX::XMConvertToRadians(velocity * camera_velocity.y * 0.1f));
	front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
	front_vec.Normalize();
	focus_position = eye_position + front_vec;
	CalcCameraDirectionalVector();

	{//　カメラの角度保持(上下に+-90度まで)

		if(front_vec.y > 0)
		{
			// カメラのフロントベクトルとy_up{0,1,0 }との射影ベクトルを算出
			const DirectX::SimpleMath::Vector3 v = arithmetic::CalcProjectVector(front_vec, { 0.0f,1.0f,0.0f });
			// カメラのフロントベクトルと平面との射影ベクトルを算出
			DirectX::SimpleMath::Vector3 project_vec = front_vec - v;
			project_vec.Normalize();

			const float d = front_vec.Dot(project_vec);
			const float rad = acosf(d);
			if (rad > DirectX::XMConvertToRadians(max_camera_angle.x))
			{// +90度以上回転しているので戻す
				const float diff = rad - DirectX::XMConvertToRadians(max_camera_angle.x);
				q = q.CreateFromAxisAngle(right_vec, diff);
				front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
				front_vec.Normalize();
				focus_position = eye_position + front_vec;
			}
		}
		else
		{
			// カメラのフロントベクトルとy_up{0,-1,0 }との射影ベクトルを算出
			const DirectX::SimpleMath::Vector3 v = arithmetic::CalcProjectVector(front_vec, { 0.0f,-1.0f,0.0f });
			// カメラのフロントベクトルと平面との射影ベクトルを算出
			const DirectX::SimpleMath::Vector3 project_vec = front_vec - v;

			const float d = front_vec.Dot(project_vec);
			const float rad = acosf(d);
			if (rad > DirectX::XMConvertToRadians(max_camera_angle.x))
			{// -90度以上回転しているので戻す
				const float diff = rad - DirectX::XMConvertToRadians(max_camera_angle.x);
				q = q.CreateFromAxisAngle(right_vec, -diff);
				front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
				front_vec.Normalize();
				focus_position = eye_position + front_vec;
			}
		}
	}
	CalcCameraDirectionalVector();
}

/*
 * brief :カメラの前後の動き
 */
void CameraWork::DollyInOut(const float velocity)
{
	const float v = -velocity * camera_velocity.y;

	focus_position += right_vec * velocity;
	eye_position += front_vec * v;
}

/*
 * brief :  カメラの左右移動(向きは固定)
 */
void CameraWork::Track( float velocity, const DirectX::SimpleMath::Vector3& axis /*基準軸*/)
{
	// カメラの注視点と位置を同じだけ移動
	focus_position += axis * velocity;
	eye_position   += axis * velocity;
}

/*
 *  brief : カメラの上下移動(向きは固定)
 */
void CameraWork::Crane( float velocity, const DirectX::SimpleMath::Vector3& axis)
{
	// カメラの注視点と位置を同じだけ移動
	focus_position += axis * velocity;
	eye_position   += axis * velocity;
}



