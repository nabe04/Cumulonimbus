#include "camera_work.h"

#include <Windows.h>
#include <imgui.h>

#include "arithmetic.h"
#include "locator.h"

using namespace DirectX;

CameraWork::CameraWork()
{

}

void CameraWork::Update(bool is_debug)
{
	CalcCameraDirectionalVector();
	CalcCameraAngle(); //オイラー角で(現在)計算しているので今は使わない

	const auto& mouse = Locator::GetInput()->Mouse();
	const auto& key = Locator::GetInput()->Keyboard();

	if(mouse.GetState(MouseButton::Left) == ButtonState::Held)
	{
		DollyInOut(static_cast<float>(mouse.DeltaY()));
		Pan(static_cast<float>(mouse.DeltaX()));

		if (key.GetState(Keycode::D) == ButtonState::Held)
			Track(camera_right, camera_speed.x);
		if (key.GetState(Keycode::A) == ButtonState::Held)
			Track(camera_right, -camera_speed.x);
		if (key.GetState(Keycode::W) == ButtonState::Held)
			Track(camera_front, camera_speed.y);
		if (key.GetState(Keycode::S) == ButtonState::Held)
			Track(camera_front, -camera_speed.y);
	}
	else if(mouse.GetState(MouseButton::Right) == ButtonState::Held)
	{
		Pan(static_cast<float>(mouse.DeltaX()));
		Tilt(static_cast<float>(mouse.DeltaY()));

		//PanAndTilt(0, { static_cast<float>(mouse.DeltaX()) ,static_cast<float>(mouse.DeltaY()) ,.0f });

		if (key.GetState(Keycode::D) == ButtonState::Held)
			Track(camera_right, camera_speed.x);
		if (key.GetState(Keycode::A) == ButtonState::Held)
			Track(camera_right, -camera_speed.x);
		if (key.GetState(Keycode::W) == ButtonState::Held)
			Track(camera_front, camera_speed.y);
		if (key.GetState(Keycode::S) == ButtonState::Held)
			Track(camera_front, -camera_speed.y);
	}
}

void CameraWork::RenderImGui()
{
	ImGui::Text("target x : %f\ntarget y : %f\ntarget z : %f", target_vec.x, target_vec.y, target_vec.z);
	ImGui::Text("angle x  : %f\nangle y  : %f\nangle z  : %f", camera_angle.x, camera_angle.y, camera_angle.z);
	ImGui::Text("Pos x  : %f\n Pos y  : %f\n Pos z  : %f", position.x, position.y, position.z);

	ImGui::Text("Up x  : %f\n Up y  : %f\n Up z  : %f", current_camerea_up.x, current_camerea_up.y, current_camerea_up.z);

	ImGui::DragFloat2("CameraSpeed",(float*)&camera_speed, 0.5f, 1, 10);
}

void CameraWork::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	this->position = position; 
}

void CameraWork::SetTargetVec(const DirectX::SimpleMath::Vector3& target)
{
	if (this->target_vec.x == this->target_vec.y == this->target_vec.z == 0.0f)
		assert(!"Vector is zero");
	
	this->target_vec = target;
}

void CameraWork::SetCameraUp(const DirectX::SimpleMath::Vector3& up)
{
	if (this->camera_up.x == this->camera_up.y == this->camera_up.z == 0.0f)
		assert(!"Vector is zero");
	
	this->camera_up = up;
}



/*
 * brief :  カメラの左右の傾き(位置は固定)
 */
void CameraWork::Pan(const float velocity)
{
	// クォータニオン Ver
	DirectX::SimpleMath::Quaternion q{};
	q = SimpleMath::Quaternion::Identity;
	q.Normalize();
	q = q.CreateFromAxisAngle(camera_up, DirectX::XMConvertToRadians(velocity * camera_speed.x * 0.1f));
	camera_front = DirectX::SimpleMath::Vector3::Transform(camera_front, q);
	target_vec = position + camera_front;

	camera_front.Normalize();


	//// オイラー角 Ver
	//camera_angle.y += velocity * camera_speed.x * 0.1f;
	//DirectX::SimpleMath::Matrix m = DirectX::SimpleMath::Matrix::Identity;
	//const DirectX::SimpleMath::Vector3 radian = { DirectX::XMConvertToRadians(camera_angle.x),DirectX::XMConvertToRadians(camera_angle.y) ,DirectX::XMConvertToRadians(camera_angle.z) };
	//m = DirectX::XMMatrixRotationRollPitchYaw(radian.x, radian.y, radian.z);
	//camera_front = DirectX::SimpleMath::Vector3::TransformNormal({ 0,0,1 }, m);

	CalcCameraDirectionalVector();
}

/*
 * brief : カメラの上下の傾き(位置は固定)
 *       : 上下に+-90度まで傾くようにする
 */
void CameraWork::Tilt(const float velocity)
{
	// クォータニオン Ver
	//DirectX::SimpleMath::Quaternion q{};
	//q = SimpleMath::Quaternion::Identity;
	//q.Normalize();
	//q = q.CreateFromAxisAngle(camera_right, DirectX::XMConvertToRadians(velocity * camera_speed.y * 0.1f));
	//camera_front = DirectX::SimpleMath::Vector3::Transform(camera_front, q);
	//CalcCameraDirectionalVector();

	//{//　カメラの角度保持(上下に+-90度まで)
	//	DirectX::SimpleMath::Vector3 v1 = { 0,camera_up.y,0 };
	//	v1.Normalize();
	//	DirectX::SimpleMath::Vector3 v2 = { 0,current_camerea_up.y,current_camerea_up.z };
	//	v2.Normalize();

	//	float angle = arithmetic::CalcAngleFromTwoVec(v1, v2);
	//	//if (v1.Cross(v2).x < 0)
	//	//	angle *= -1;

	//	//if (camera_up.y > 0 && camera_front.z < 0)
	//	//	angle *= -1;

	//	if (angle > 85)
	//	{
	//		q = SimpleMath::Quaternion::Identity;
	//		q.Normalize();
	//		q = q.CreateFromAxisAngle(camera_right, DirectX::XMConvertToRadians(-2));
	//		camera_front = DirectX::SimpleMath::Vector3::Transform(camera_front, q);
	//	}
	//	else if (angle < -85)
	//	{
	//		q = SimpleMath::Quaternion::Identity;
	//		q.Normalize();
	//		q = q.CreateFromAxisAngle(camera_right, DirectX::XMConvertToRadians(2));
	//		camera_front = DirectX::SimpleMath::Vector3::Transform(camera_front, q);
	//	}
	//	ImGui::Text("Angle %f", angle);
	//}

	// オイラー角 Ver
	DirectX::SimpleMath::Matrix m = DirectX::SimpleMath::Matrix::Identity;
	camera_angle.x += velocity * camera_speed.y * 0.1f;
	if (camera_angle.x > 85)
		camera_angle.x = 85;
	else if (camera_angle.x < -85)
		camera_angle.x = -85;

	const DirectX::SimpleMath::Vector3 radian = { DirectX::XMConvertToRadians(camera_angle.x),DirectX::XMConvertToRadians(camera_angle.y) ,DirectX::XMConvertToRadians(camera_angle.z) };
	m = DirectX::XMMatrixRotationRollPitchYaw(radian.x, radian.y, radian.z);
	camera_front = DirectX::SimpleMath::Vector3::TransformNormal({ 0,0,1 }, m);
	CalcCameraDirectionalVector();
}

/*
 * brief : 被写体に対しカメラが前後に移動する事
 */
void CameraWork::DollyInOut(const float velocity)
{
	const float v = -velocity * camera_speed.y;

	position += camera_front * v;
}

void CameraWork::Track(const DirectX::SimpleMath::Vector3& direction, const float velocity)
{
	DirectX::SimpleMath::Vector3 normal = direction;
	normal.Normalize();
	position += normal * velocity;
}

void CameraWork::PanAndTilt(const float velocity, const DirectX::SimpleMath::Vector3& add_angle)
{
	const auto& key = Locator::GetInput()->Keyboard();

	DirectX::SimpleMath::Vector3 front_vec{ 0,0,1 };
	static DirectX::SimpleMath::Vector3 angle{};
	//angle += add_angle;

	if (key.GetState(Keycode::Right) == ButtonState::Held)
		angle.y += 0.5f;

	if (key.GetState(Keycode::Left) == ButtonState::Held)
		angle.y -= 0.5f;


	if (key.GetState(Keycode::Up) == ButtonState::Held)
		angle.x += 0.5f;


	if (key.GetState(Keycode::Down) == ButtonState::Held)
		angle.x -= 0.5f;

	//ImGui::Text("Angle : X %f", angle.x);
	//ImGui::Text("Angle : Y %f", angle.y);
	//ImGui::Text("Angle : Z %f", angle.z);

	DirectX::XMMATRIX m;
	m = DirectX::XMMatrixIdentity();
	static float a;
	a += 0.0001f;
	m = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle.x), DirectX::XMConvertToRadians(angle.y), 0);

	camera_front = DirectX::SimpleMath::Vector3::TransformNormal(front_vec, m);


}


/*
 * brief   : カメラのRight,Upベクトルを算出
 * caution : カメラのFrontベクトルを基準に算出するのでFrontベクトルの計算はしていない
 */
void CameraWork::CalcCameraDirectionalVector()
{
	camera_right = arithmetic::CalcRightVec(current_camerea_up, camera_front);
	current_camerea_up = arithmetic::CalcUpVec(camera_front, camera_right);
	//camera_up	 = SimpleMath::Vector3{ 0,1,0 };

	camera_front.Normalize();
	camera_right.Normalize();
	current_camerea_up.Normalize();
}

/*
 * brief : 左手座標軸を基準にしてのカメラの角度(Degree)計算
 */
void CameraWork::CalcCameraAngle()
{
	using namespace DirectX::SimpleMath;

	const Vector3 right{ 1,0,0 };
	const Vector3 up{ 0,1,0 };
	const Vector3 front{ 0,0,1 };

	camera_angle.x = DirectX::XMConvertToDegrees(acosf(up.Dot(camera_up)));
	if (up.Cross(camera_up).x < 0)
		camera_angle.x *= -1;
	camera_angle.y = DirectX::XMConvertToDegrees(acosf(front.Dot(camera_front)));
	if (front.Cross(camera_front).y < 0)
		camera_angle.y *= -1;

	camera_angle.z = arithmetic::CalcAngle_Z(camera_front);
}



