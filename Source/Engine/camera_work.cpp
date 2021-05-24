#include "camera_work.h"

#include <imgui.h>

#include "arithmetic.h"
#include "locator.h"

using namespace DirectX;


void CameraWork::Update(bool is_debug)
{
	CalcCameraDirectionalVector();
	CalcCameraAngle(); //�I�C���[�p��(����)�v�Z���Ă���̂ō��͎g��Ȃ�

	const auto& mouse = Locator::GetInput()->Mouse();
	const auto& key   = Locator::GetInput()->Keyboard();

	if(mouse.GetState(MouseButton::Left) == ButtonState::Held)
	{
		DollyInOut(static_cast<float>(mouse.DeltaY()));
		Pan(static_cast<float>(mouse.DeltaX()));

		if (key.GetState(Keycode::D) == ButtonState::Held)
			Track(camera_right, camera_speed.x);
		if (key.GetState(Keycode::A) == ButtonState::Held)
			Track(camera_right, -camera_speed.x);
		if (key.GetState(Keycode::W) == ButtonState::Held)
			Track(front_vec, camera_speed.y);
		if (key.GetState(Keycode::S) == ButtonState::Held)
			Track(front_vec, -camera_speed.y);
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
			Track(front_vec, camera_speed.y);
		if (key.GetState(Keycode::S) == ButtonState::Held)
			Track(front_vec, -camera_speed.y);
	}
}

void CameraWork::RenderImGui()
{
	ImGui::Text("focus_position x : %f\nfocus_position y : %f\nfocus_position z : %f", focus_position.x, focus_position.y, focus_position.z);
	ImGui::Text("angle x  : %f\nangle y  : %f\nangle z  : %f", camera_angle.x, camera_angle.y, camera_angle.z);
	ImGui::Text("Pos x  : %f\n Pos y  : %f\n Pos z  : %f", eye_position.x, eye_position.y, eye_position.z);

	ImGui::Text("Up x  : %f\n Up y  : %f\n Up z  : %f", current_camera_up.x, current_camera_up.y, current_camera_up.z);

	ImGui::DragFloat2("CameraSpeed",(float*)&camera_speed, 0.5f, 1, 10);
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
	// HACK: �C�R�[���֐����������ύX����
	if (this->up_vec.x == this->up_vec.y == this->up_vec.z == 0.0f)
		assert(!"Vector is zero");

	this->up_vec = up;
}


/*
 * brief :  �J�����̍��E�̌X��(�ʒu�͌Œ�)
 */
void CameraWork::Pan(const float velocity)
{
	// �N�H�[�^�j�I�� Ver
	DirectX::SimpleMath::Quaternion q{};
	q = SimpleMath::Quaternion::Identity;
	q.Normalize();
	q = q.CreateFromAxisAngle(up_vec, DirectX::XMConvertToRadians(velocity * camera_speed.x * 0.1f));
	front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
	front_vec.Normalize();

	// �g��Ȃ�����
	focus_position = eye_position + front_vec;

	//// �I�C���[�p Ver
	//camera_angle.y += velocity * camera_speed.x * 0.1f;
	//DirectX::SimpleMath::Matrix m = DirectX::SimpleMath::Matrix::Identity;
	//const DirectX::SimpleMath::Vector3 radian = { DirectX::XMConvertToRadians(camera_angle.x),DirectX::XMConvertToRadians(camera_angle.y) ,DirectX::XMConvertToRadians(camera_angle.z) };
	//m = DirectX::XMMatrixRotationRollPitchYaw(radian.x, radian.y, radian.z);
	//front_vec = DirectX::SimpleMath::Vector3::TransformNormal({ 0,0,1 }, m);

	CalcCameraDirectionalVector();
}

/*
 * brief : �J�����̏㉺�̌X��(�ʒu�͌Œ�)
 *       : �㉺��+-90�x�܂ŌX���悤�ɂ���
 */
void CameraWork::Tilt(const float velocity)
{
	// �N�H�[�^�j�I�� Ver
	DirectX::SimpleMath::Quaternion q{};
	q = SimpleMath::Quaternion::Identity;
	q.Normalize();
	q = q.CreateFromAxisAngle(right_vec, DirectX::XMConvertToRadians(velocity * camera_speed.y * 0.1f));
	front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
	CalcCameraDirectionalVector();

	{//�@�J�����̊p�x�ێ�(�㉺��+-90�x�܂�)
		DirectX::SimpleMath::Vector3 v1 = { 0,up_vec.y,0 };
		v1.Normalize();
		DirectX::SimpleMath::Vector3 v2 = { 0,current_camera_up.y,current_camera_up.z };
		v2.Normalize();

		const float angle = arithmetic::CalcAngleFromTwoVec(v1, v2);
		//if (v1.Cross(v2).x < 0)
		//	angle *= -1;

		//if (up_vec.y > 0 && front_vec.z < 0)
		//	angle *= -1;

		if (angle > 85)
		{
			q = SimpleMath::Quaternion::Identity;
			q.Normalize();
			q = q.CreateFromAxisAngle(right_vec, DirectX::XMConvertToRadians(-2));
			front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		}
		else if (angle < -85)
		{
			q = SimpleMath::Quaternion::Identity;
			q.Normalize();
			q = q.CreateFromAxisAngle(right_vec, DirectX::XMConvertToRadians(2));
			front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		}
		ImGui::Text("Angle %f", angle);
	}

	//// �I�C���[�p Ver
	//DirectX::SimpleMath::Matrix m = DirectX::SimpleMath::Matrix::Identity;
	//camera_angle.x += velocity * camera_speed.y * 0.1f;
	//if (camera_angle.x > 85)
	//	camera_angle.x = 85;
	//else if (camera_angle.x < -85)
	//	camera_angle.x = -85;

	//const DirectX::SimpleMath::Vector3 radian = { DirectX::XMConvertToRadians(camera_angle.x),DirectX::XMConvertToRadians(camera_angle.y) ,DirectX::XMConvertToRadians(camera_angle.z) };
	//m = DirectX::XMMatrixRotationRollPitchYaw(radian.x, radian.y, radian.z);
	//front_vec = DirectX::SimpleMath::Vector3::TransformNormal({ 0,0,1 }, m);
	CalcCameraDirectionalVector();
}

/*
 * brief :�J�����̑O��̓���
 */
void CameraWork::DollyInOut(const float velocity)
{
	const float v = -velocity * camera_speed.y;

	eye_position += front_vec * v;
}

void CameraWork::Track(const DirectX::SimpleMath::Vector3& direction, const float velocity)
{
	DirectX::SimpleMath::Vector3 normal = direction;
	normal.Normalize();
	eye_position += normal * velocity;
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

	front_vec = DirectX::SimpleMath::Vector3::TransformNormal(front_vec, m);


}


/*
 * brief   : �J������Right,Up�x�N�g�����Z�o
 * caution : �J������Front�x�N�g������ɎZ�o����̂�Front�x�N�g���̌v�Z�͂��Ă��Ȃ�
 */
void CameraWork::CalcCameraDirectionalVector()
{
	camera_right = arithmetic::CalcRightVec(current_camera_up, front_vec);
	current_camera_up = arithmetic::CalcUpVec(front_vec, camera_right);
	//up_vec	 = SimpleMath::Vector3{ 0,1,0 };

	front_vec.Normalize();
	camera_right.Normalize();
	current_camera_up.Normalize();
}

/*
 * brief : ������W������ɂ��ẴJ�����̊p�x(Degree)�v�Z
 */
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



