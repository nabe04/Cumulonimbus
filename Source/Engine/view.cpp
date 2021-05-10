#include "view.h"

#include <assert.h>
#include <cmath>

#include <wrl.h>
using namespace DirectX;

#include "imgui.h"
#include "framework.h"
#include "locator.h"


View::View()
{
	ZeroMemory(this, 0);
	view_f4x4 = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	projection_f4x4 = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	width  = static_cast<float>(Locator::GetWindow()->Width());
	height = static_cast<float>(Locator::GetWindow()->Height());
}

View::~View()
{

}

void View::CalcCameraOrthogonalVector()
{
	if (!std::isfinite(target.x) || !std::isfinite(target.y) || !std::isfinite(target.z))
	{
		assert(!"Camera target val is nan or infinity");
	}

	camera_forcus = DirectX::SimpleMath::Vector3{ target.x - position.x,target.y - position.y,target.z - position.z };

	XMVECTOR up, right, front;
	up = XMLoadFloat3(&camera_up);
	front = XMLoadFloat3(&camera_forcus);
	front = XMVector3Normalize(front);

	right = XMVector3Cross(up, front);
	up	= XMVector3Cross(front, right);

	right = XMVector3Normalize(right);
	up = XMVector3Normalize(up);

	XMStoreFloat3(&camera_forcus, front);
	XMStoreFloat3(&camera_up, up);
	XMStoreFloat3(&camera_right, right);
}

void View::SetCameraPos(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 up)
{
	this->position	= pos;
	this->target	= target;
	this->camera_up	= up;
}

void View::SetProjection(float fov, float aspect, float min, float max)
{
	is_perspective	= true;
	this->fov		= fov;
	this->aspect	= aspect;
	this->nearZ		= min;
	this->farZ		= max;
}

void View::SetOrtho(float width, float height, float min, float max)
{
	is_perspective	= false;
	this->width		= width;
	this->height	= height;
	this->nearZ		= min;
	this->farZ		= max;
}

void View::Activate()
{
	CalcCameraOrthogonalVector();

#ifdef _DEBUG
	if (Locator::GetInput()->Keyboard().GetState(Keycode::LeftShift) == ButtonState::Held)
	{
		if (Locator::GetInput()->Keyboard().GetState(Keycode::F1) == ButtonState::Press)
		{
			is_debug_camera = !is_debug_camera;
		}
	}
#endif
	if (is_debug_camera)
	{
		CameraControl();

		//MouseInput::GetInstance().IsDebug(true);
	}
	else
	{
		//MouseInput::GetInstance().IsDebug(false);
	}

	XMVECTOR pos_vec, target_vec, up_vec;
	pos_vec = XMVectorSet(position.x, position.y, position.z, 0);
	target_vec = XMVectorSet(target.x, target.y, target.z, 0);
	up_vec = XMVectorSet(camera_up.x, camera_up.y, camera_up.z, 0);
	XMMATRIX vm = XMMatrixLookAtLH(pos_vec, target_vec, up_vec);
	XMStoreFloat4x4(&view_f4x4, vm);

	XMMATRIX pm = XMMatrixIdentity();
	if (is_perspective)
	{
		pm = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
		XMStoreFloat4x4(&projection_f4x4, pm);
	}
	else
	{
		pm = XMMatrixOrthographicLH(width, height, nearZ, farZ);
		XMStoreFloat4x4(&projection_f4x4, pm);
	}
}

//-- Camera control for debug --//
void View::CameraControl()
{
	prev_mouse_pos = cur_mouse_pos;

	cur_mouse_pos.x = static_cast<float>(Locator::GetInput()->Mouse().PosX());
	cur_mouse_pos.y = static_cast<float>(Locator::GetInput()->Mouse().PosY());

	// The amount of mouse movement between the "cur_mouse_pos" and "prev_mouse_pos";
	const float camera_speed_gamepad = 0.04f;
	static XMFLOAT2 movement_val = {};
	movement_val.x = (cur_mouse_pos.x - prev_mouse_pos.x) * camera_speed_gamepad;
	movement_val.y = (cur_mouse_pos.y - prev_mouse_pos.y) * camera_speed_gamepad;

	if (Locator::GetInput()->Keyboard().GetState(Keycode::LeftAlt) == ButtonState::Held)
	{
		if (Locator::GetInput()->Mouse().GetState(MouseButton::Left) == ButtonState::Held)
		{//-- Wraparound --//
			const float ajustmant_val = 3.5f;
			const XMVECTOR vector  = XMVectorSet(position.x - target.x, position.y - target.y, position.z - target.z, 0); // Vector from camera target to camera position
			const XMVECTOR target_vec = XMLoadFloat3(&target);	// Vector of camera target
			const XMVECTOR y_Up    = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
			const XMVECTOR x_Up    = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

			//-- Rotation Matrix --//
			XMMATRIX rotation_matrix_y = XMMatrixRotationAxis(y_Up, XMConvertToRadians(movement_val.x * ajustmant_val));
			XMMATRIX rotation_matrix_x = XMMatrixRotationAxis(x_Up, XMConvertToRadians(movement_val.y * ajustmant_val));

			// Gazing point vector after rotation
			XMVECTOR pos_vec = XMVector3Transform(vector, rotation_matrix_y * rotation_matrix_x);
			pos_vec += target_vec;
			XMStoreFloat3(&position, pos_vec);

			XMVECTOR vCameraUp = XMLoadFloat3(&camera_up);
			XMVECTOR vCameraUp2 = XMVector3TransformNormal(vCameraUp, rotation_matrix_x);
			XMStoreFloat3(&camera_up, vCameraUp2);
		}
		else if (Locator::GetInput()->Mouse().GetState(MouseButton::Right) == ButtonState::Held)
		{//-- Zoom in and Zoom out --//

			const float ajustmant_val = 0.5f;

			const XMVECTOR vector		= XMVectorSet(target.x - position.x, target.y - position.y, target.z - position.z, 0);  // Vector from camera position to camera target
			const XMVECTOR vPos			= XMVectorSet(position.x, position.y, position.z, 0);
			const XMVECTOR vTarget		= XMVectorSet(target.x, target.y, target.z, 0);
			const XMVECTOR normalizeVec = XMVector3Normalize(vector); // Normalisation
			const XMVECTOR zoomVal		= XMVectorSet(normalizeVec.m128_f32[0] * movement_val.x, normalizeVec.m128_f32[1] * movement_val.x,
													normalizeVec.m128_f32[2] * movement_val.x, normalizeVec.m128_f32[3] * movement_val.x);

			XMStoreFloat3(&position, XMVectorAdd(zoomVal, vPos));
			XMStoreFloat3(&target, XMVectorAdd(zoomVal, vTarget));
		}
		else
		{
			movement_val = {};
		}
	}
	else if (Locator::GetInput()->Mouse().GetState(MouseButton::Right) == ButtonState::Held && Locator::GetInput()->Mouse().GetState(MouseButton::Left) == ButtonState::Held)
	{//-- Truck --//
		const XMVECTOR vNormalUp	   = XMVector3Normalize(XMVectorSet(camera_up.x,camera_up.y,camera_up.z,0.0f));
		const XMVECTOR vNormalTarget   = XMVector3Normalize(XMVectorSet(target.x - position.x, target.y - position.y, target.z - position.z, 0.0f));

		const XMVECTOR vRight = XMVector3Cross(vNormalUp, vNormalTarget);

		DirectX::SimpleMath::Vector3 f3_Up;
		DirectX::SimpleMath::Vector3 f3_Right;

		XMStoreFloat3(&f3_Up, vNormalUp);
		XMStoreFloat3(&f3_Right, vRight);

		const float ajustmant_val = 0.5f;
		f3_Up.x    *= -movement_val.y * ajustmant_val;
		f3_Up.y    *= -movement_val.y * ajustmant_val;
		f3_Up.z    *= -movement_val.y * ajustmant_val;

		f3_Right.x *= movement_val.x * ajustmant_val;
		f3_Right.y *= movement_val.x * ajustmant_val;
		f3_Right.z *= movement_val.x * ajustmant_val;

		position.x += f3_Up.x;
		position.y += f3_Up.y;
		position.z += f3_Up.z;

		target.x += f3_Up.x;
		target.y += f3_Up.y;
		target.z += f3_Up.z;

		position.x += f3_Right.x;
		position.y += f3_Right.y;
		position.z += f3_Right.z;

		target.x += f3_Right.x;
		target.y += f3_Right.y;
		target.z += f3_Right.z;
	}
	else if (Locator::GetInput()->Mouse().GetState(MouseButton::Right) == ButtonState::Held)
	{//-- Pan & Tilt --//

		const XMVECTOR vector  = XMVectorSet(target.x - position.x, target.y - position.y, target.z - position.z, 0); // Vector from camera position to camera target
		const XMVECTOR vCamera = XMLoadFloat3(&position);	// Vector of camera position
		const XMVECTOR y_Up    = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMVECTOR x_Up    = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		//-- Rotation Matrix --//
		XMMATRIX rotation_matrix_y = XMMatrixRotationAxis(y_Up, XMConvertToRadians(movement_val.x));
		XMMATRIX rotation_matrix_x = XMMatrixRotationAxis(x_Up, XMConvertToRadians(movement_val.y));

		// Gazing point vector after rotation`
		XMVECTOR vTarget = XMVector3Transform(vector, rotation_matrix_y * rotation_matrix_x);
		vTarget += vCamera;
		XMStoreFloat3(&target, vTarget);

		XMVECTOR vCameraUp  = XMLoadFloat3(&camera_up);
		vCameraUp			= XMVector3TransformNormal(vCameraUp, rotation_matrix_x);
		XMStoreFloat3(&camera_up, vCameraUp);
	}
	else
	{
		movement_val = {};
	}
}

void View::RotationAjustWrapPitchYawRoll(float pitch, float yaw, float roll)
{
	XMMATRIX rotation_matrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll));

	XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(position.x - target.x, position.y - target.y, position.z - target.z, 1.0f);

	XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	// Activate rataion
	vec_from_target_pos_to_camera_pos = XMVector3Transform(vec_from_target_pos_to_camera_pos, rotation_matrix);

	XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	XMVECTOR vecCameraPos = XMVectorMultiply(normal, length);

	XMStoreFloat3(&position, vecCameraPos);
}

void View::RotationAjustWrapX(float angleX)
{
	//XMMATRIX rotation_matrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(angleX), 0, 0);

	//XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(position.x - target.x, position.y - target.y, position.z - target.z, 0.0f);

	//XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	//// Activate rataion
	//vec_from_target_pos_to_camera_pos = XMVector3TransformCoord(vec_from_target_pos_to_camera_pos, rotation_matrix);

	//XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	//XMVECTOR vecCameraPos = XMVectorMultiply(normal, length);

	//XMVECTOR vecTarget    = XMVectorSet(target.x, target.y, target.z, 1.0f);

	//XMStoreFloat3(&position, vecTarget + vecCameraPos);

	XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(position.x - target.x, position.y - target.y, position.z - target.z, 1.0f);

	XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	XMVECTOR vecX = XMVectorSet(1, 0, 0, 0);
	XMVECTOR vec_quaternion = XMQuaternionRotationNormal(vecX, XMConvertToRadians(angleX));
	vec_from_target_pos_to_camera_pos = XMVector3Rotate(vec_from_target_pos_to_camera_pos, vec_quaternion);

	XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	XMVECTOR vec_camera_pos = XMVectorMultiply(normal, length);

	XMVECTOR vecTarget    = XMVectorSet(target.x, target.y, target.z, 1.0f);

	XMStoreFloat3(&position, vecTarget + vec_from_target_pos_to_camera_pos);
}

void View::RotationAjustWrapY(float angleY)
{
	//XMMATRIX rotation_matrix = XMMatrixRotationY(XMConvertToRadians(angleY));

	//XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(position.x - target.x, position.y - target.y, position.z - target.z, 1.0f);

	//XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	//// Activate rataion
	//vec_from_target_pos_to_camera_pos = XMVector3Transform(vec_from_target_pos_to_camera_pos, rotation_matrix);

	//XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	//XMVECTOR vec_camera_pos = XMVectorMultiply(normal, length);

	//XMStoreFloat3(&position, vec_camera_pos);

	XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(position.x - target.x, position.y - target.y, position.z - target.z, 1.0f);

	XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	XMVECTOR vecY = XMVectorSet(0, 1, 0, 0);
	XMVECTOR vec_quaternion = XMQuaternionRotationNormal(vecY, XMConvertToRadians(angleY));
	vec_from_target_pos_to_camera_pos = XMVector3Rotate(vec_from_target_pos_to_camera_pos, vec_quaternion);

	XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	XMVECTOR vec_camera_pos = XMVectorMultiply(normal, length);

	XMVECTOR vecTarget = XMVectorSet(target.x, target.y, target.z, 1.0f);

	XMStoreFloat3(&position, vecTarget + vec_from_target_pos_to_camera_pos);
}

void View::RotationAjustWrapZ(float angleZ)
{
	XMMATRIX rotation_matrix = XMMatrixRotationZ(XMConvertToRadians(angleZ));

	XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(position.x - target.x, position.y - target.y, position.z - target.z, 1.0f);

	XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	// Activate rataion
	vec_from_target_pos_to_camera_pos = XMVector3Transform(vec_from_target_pos_to_camera_pos, rotation_matrix);

	XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	XMVECTOR vecCameraPos = XMVectorMultiply(normal, length);

	XMStoreFloat3(&position, vecCameraPos);
}


void View::WriteImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Checkbox("Debug", &is_debug_camera);

		ImGui::Text("Camera Pos X %f", position.x);
		ImGui::Text("Camera Pos Y %f", position.y);
		ImGui::Text("Camera Pos Z %f", position.z);
		ImGui::Text("Target X %f", target.x);
		ImGui::Text("Target Y %f", target.y);
		ImGui::Text("Target Z %f", target.z);
		ImGui::Text("Forcus X %f", camera_forcus.x);
		ImGui::Text("Forcus Y %f", camera_forcus.y);
		ImGui::Text("Forcus Z %f", camera_forcus.z);
		ImGui::Text("Camera Up X %f", camera_up.x);
		ImGui::Text("Camera Up Y %f", camera_up.y);
		ImGui::Text("Camera Up Z %f", camera_up.z);
		ImGui::Text("Right X %f", camera_right.x);
		ImGui::Text("Right Y %f", camera_right.y);
		ImGui::Text("Right Z %f", camera_right.z);
	}
}
