#include "view.h"

#include <cassert>
#include <cmath>

#include <wrl.h>

#include <imgui.h>
#include "framework.h"
#include "locator.h"
#include "camera_work.h"


View::View()
{
	camera_work = std::make_unique<CameraWork>(*this);

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
	if (!std::isfinite(focus_position.x) || !std::isfinite(focus_position.y) || !std::isfinite(focus_position.z))
	{
		assert(!"Camera focus_position val is nan or infinity");
	}

	front_vec = DirectX::SimpleMath::Vector3{ focus_position.x - eye_position.x,
											  focus_position.y - eye_position.y,
											  focus_position.z - eye_position.z };

	XMStoreFloat3(&front_vec, XMVector3Normalize(XMLoadFloat3(&front_vec)));
	XMStoreFloat3(&up_vec	, XMVector3Normalize(XMLoadFloat3(&up_vec)));
	XMStoreFloat3(&right_vec, XMVector3Normalize(XMLoadFloat3(&right_vec)));
}

void View::SetCameraPos(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 up)
{
	this->eye_position	= pos;
	this->focus_position	= target;
	this->up_vec	= up;
}

void View::SetProjection(float fov, float aspect, float min, float max)
{
	is_perspective	= true;
	this->fov		= fov;
	this->aspect	= aspect;
	this->near_z	= min;
	this->far_z		= max;
}

void View::SetOrtho(float width, float height, float min, float max)
{
	is_perspective	= false;
	this->width		= width;
	this->height	= height;
	this->near_z	= min;
	this->far_z		= max;
}

void View::Activate()
{
	camera_work->SetCameraInfo(*this);
	camera_work->Update(true);
	SetFocusPosition(camera_work->GetFocusPosition());
	SetEyePosition(camera_work->GetPosition());
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
		// HACK: カメラワーククラスができたらカメラワーククラスに任せて消す
		CameraControl();

		//MouseInput::GetInstance().IsDebug(true);
	}
	else
	{
		//MouseInput::GetInstance().IsDebug(false);
	}

	const XMMATRIX view_mat = XMMatrixLookAtLH( XMVectorSet(eye_position.x, eye_position.y, eye_position.z, 0),
												XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 0),
												XMVectorSet(up_vec.x, up_vec.y, up_vec.z, 0));
	XMStoreFloat4x4(&view_f4x4, view_mat);

	XMMATRIX pm = XMMatrixIdentity();
	if (is_perspective)
	{
		pm = XMMatrixPerspectiveFovLH(fov, aspect, near_z, far_z);
		XMStoreFloat4x4(&projection_f4x4, pm);
	}
	else
	{
		pm = XMMatrixOrthographicLH(width, height, near_z, far_z);
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
			const XMVECTOR vector  = XMVectorSet(eye_position.x - focus_position.x, eye_position.y - focus_position.y, eye_position.z - focus_position.z, 0); // Vector from camera focus_position to camera eye_position
			const XMVECTOR target_vec = XMLoadFloat3(&focus_position);	// Vector of camera focus_position
			const XMVECTOR y_Up    = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
			const XMVECTOR x_Up    = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

			//-- Rotation Matrix --//
			XMMATRIX rotation_matrix_y = XMMatrixRotationAxis(y_Up, XMConvertToRadians(movement_val.x * ajustmant_val));
			XMMATRIX rotation_matrix_x = XMMatrixRotationAxis(x_Up, XMConvertToRadians(movement_val.y * ajustmant_val));

			// Gazing point vector after rotation
			XMVECTOR pos_vec = XMVector3Transform(vector, rotation_matrix_y * rotation_matrix_x);
			pos_vec += target_vec;
			XMStoreFloat3(&eye_position, pos_vec);

			XMVECTOR vCameraUp = XMLoadFloat3(&up_vec);
			XMVECTOR vCameraUp2 = XMVector3TransformNormal(vCameraUp, rotation_matrix_x);
			XMStoreFloat3(&up_vec, vCameraUp2);
		}
		else if (Locator::GetInput()->Mouse().GetState(MouseButton::Right) == ButtonState::Held)
		{//-- Zoom in and Zoom out --//

			const float ajustmant_val = 0.5f;

			const XMVECTOR vector		= XMVectorSet(focus_position.x - eye_position.x, focus_position.y - eye_position.y, focus_position.z - eye_position.z, 0);  // Vector from camera eye_position to camera focus_position
			const XMVECTOR vPos			= XMVectorSet(eye_position.x, eye_position.y, eye_position.z, 0);
			const XMVECTOR vTarget		= XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 0);
			const XMVECTOR normalizeVec = XMVector3Normalize(vector); // Normalisation
			const XMVECTOR zoomVal		= XMVectorSet(normalizeVec.m128_f32[0] * movement_val.x, normalizeVec.m128_f32[1] * movement_val.x,
													normalizeVec.m128_f32[2] * movement_val.x, normalizeVec.m128_f32[3] * movement_val.x);

			XMStoreFloat3(&eye_position, XMVectorAdd(zoomVal, vPos));
			XMStoreFloat3(&focus_position, XMVectorAdd(zoomVal, vTarget));
		}
		else
		{
			movement_val = {};
		}
	}
	else if (Locator::GetInput()->Mouse().GetState(MouseButton::Right) == ButtonState::Held && Locator::GetInput()->Mouse().GetState(MouseButton::Left) == ButtonState::Held)
	{//-- Truck --//
		const XMVECTOR vNormalUp	   = XMVector3Normalize(XMVectorSet(up_vec.x,up_vec.y,up_vec.z,0.0f));
		const XMVECTOR vNormalTarget   = XMVector3Normalize(XMVectorSet(focus_position.x - eye_position.x, focus_position.y - eye_position.y, focus_position.z - eye_position.z, 0.0f));

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

		eye_position.x += f3_Up.x;
		eye_position.y += f3_Up.y;
		eye_position.z += f3_Up.z;

		focus_position.x += f3_Up.x;
		focus_position.y += f3_Up.y;
		focus_position.z += f3_Up.z;

		eye_position.x += f3_Right.x;
		eye_position.y += f3_Right.y;
		eye_position.z += f3_Right.z;

		focus_position.x += f3_Right.x;
		focus_position.y += f3_Right.y;
		focus_position.z += f3_Right.z;
	}
	else if (Locator::GetInput()->Mouse().GetState(MouseButton::Right) == ButtonState::Held)
	{//-- Pan & Tilt --//

		const XMVECTOR vector  = XMVectorSet(focus_position.x - eye_position.x, focus_position.y - eye_position.y, focus_position.z - eye_position.z, 0); // Vector from camera eye_position to camera focus_position
		const XMVECTOR vCamera = XMLoadFloat3(&eye_position);	// Vector of camera eye_position
		const XMVECTOR y_Up    = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMVECTOR x_Up    = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		//-- Rotation Matrix --//
		XMMATRIX rotation_matrix_y = XMMatrixRotationAxis(y_Up, XMConvertToRadians(movement_val.x));
		XMMATRIX rotation_matrix_x = XMMatrixRotationAxis(x_Up, XMConvertToRadians(movement_val.y));

		// Gazing point vector after rotation`
		XMVECTOR vTarget = XMVector3Transform(vector, rotation_matrix_y * rotation_matrix_x);
		vTarget += vCamera;
		XMStoreFloat3(&focus_position, vTarget);

		XMVECTOR vCameraUp  = XMLoadFloat3(&up_vec);
		vCameraUp			= XMVector3TransformNormal(vCameraUp, rotation_matrix_x);
		XMStoreFloat3(&up_vec, vCameraUp);
	}
	else
	{
		movement_val = {};
	}
}

void View::RotationAdjustWrapPitchYawRoll(float pitch, float yaw, float roll)
{
	XMMATRIX rotation_matrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll));

	XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(eye_position.x - focus_position.x, eye_position.y - focus_position.y, eye_position.z - focus_position.z, 1.0f);

	XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	// Activate rataion
	vec_from_target_pos_to_camera_pos = XMVector3Transform(vec_from_target_pos_to_camera_pos, rotation_matrix);

	XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	XMVECTOR vecCameraPos = XMVectorMultiply(normal, length);

	XMStoreFloat3(&eye_position, vecCameraPos);
}

void View::RotationAdjustWrapX(float angleX)
{
	//XMMATRIX rotation_matrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(angleX), 0, 0);

	//XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(eye_position.x - focus_position.x, eye_position.y - focus_position.y, eye_position.z - focus_position.z, 0.0f);

	//XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	//// Activate rataion
	//vec_from_target_pos_to_camera_pos = XMVector3TransformCoord(vec_from_target_pos_to_camera_pos, rotation_matrix);

	//XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	//XMVECTOR vecCameraPos = XMVectorMultiply(normal, length);

	//XMVECTOR vecTarget    = XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 1.0f);

	//XMStoreFloat3(&eye_position, vecTarget + vecCameraPos);

	XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(eye_position.x - focus_position.x, eye_position.y - focus_position.y, eye_position.z - focus_position.z, 1.0f);

	XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	XMVECTOR vecX = XMVectorSet(1, 0, 0, 0);
	XMVECTOR vec_quaternion = XMQuaternionRotationNormal(vecX, XMConvertToRadians(angleX));
	vec_from_target_pos_to_camera_pos = XMVector3Rotate(vec_from_target_pos_to_camera_pos, vec_quaternion);

	XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	XMVECTOR vec_camera_pos = XMVectorMultiply(normal, length);

	XMVECTOR vecTarget    = XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 1.0f);

	XMStoreFloat3(&eye_position, vecTarget + vec_from_target_pos_to_camera_pos);
}

void View::RotationAdjustWrapY(float angleY)
{
	//XMMATRIX rotation_matrix = XMMatrixRotationY(XMConvertToRadians(angleY));

	//XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(eye_position.x - focus_position.x, eye_position.y - focus_position.y, eye_position.z - focus_position.z, 1.0f);

	//XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	//// Activate rataion
	//vec_from_target_pos_to_camera_pos = XMVector3Transform(vec_from_target_pos_to_camera_pos, rotation_matrix);

	//XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	//XMVECTOR vec_camera_pos = XMVectorMultiply(normal, length);

	//XMStoreFloat3(&eye_position, vec_camera_pos);

	XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(eye_position.x - focus_position.x, eye_position.y - focus_position.y, eye_position.z - focus_position.z, 1.0f);

	XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	XMVECTOR vecY = XMVectorSet(0, 1, 0, 0);
	XMVECTOR vec_quaternion = XMQuaternionRotationNormal(vecY, XMConvertToRadians(angleY));
	vec_from_target_pos_to_camera_pos = XMVector3Rotate(vec_from_target_pos_to_camera_pos, vec_quaternion);

	XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	XMVECTOR vec_camera_pos = XMVectorMultiply(normal, length);

	XMVECTOR vecTarget = XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 1.0f);

	XMStoreFloat3(&eye_position, vecTarget + vec_from_target_pos_to_camera_pos);
}

void View::RotationAdjustWrapZ(float angleZ)
{
	XMMATRIX rotation_matrix = XMMatrixRotationZ(XMConvertToRadians(angleZ));

	XMVECTOR vec_from_target_pos_to_camera_pos = XMVectorSet(eye_position.x - focus_position.x, eye_position.y - focus_position.y, eye_position.z - focus_position.z, 1.0f);

	XMVECTOR length = XMVector3Length(vec_from_target_pos_to_camera_pos);

	// Activate rataion
	vec_from_target_pos_to_camera_pos = XMVector3Transform(vec_from_target_pos_to_camera_pos, rotation_matrix);

	XMVECTOR normal = XMVector3Normalize(vec_from_target_pos_to_camera_pos);

	XMVECTOR vecCameraPos = XMVectorMultiply(normal, length);

	XMStoreFloat3(&eye_position, vecCameraPos);
}


void View::WriteImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Checkbox("Debug", &is_debug_camera);

		ImGui::Text("Camera Pos X %f", eye_position.x);
		ImGui::Text("Camera Pos Y %f", eye_position.y);
		ImGui::Text("Camera Pos Z %f", eye_position.z);
		ImGui::Text("Target X %f", focus_position.x);
		ImGui::Text("Target Y %f", focus_position.y);
		ImGui::Text("Target Z %f", focus_position.z);
		ImGui::Text("Forcus X %f", front_vec.x);
		ImGui::Text("Forcus Y %f", front_vec.y);
		ImGui::Text("Forcus Z %f", front_vec.z);
		ImGui::Text("Camera Up X %f", up_vec.x);
		ImGui::Text("Camera Up Y %f", up_vec.y);
		ImGui::Text("Camera Up Z %f", up_vec.z);
		ImGui::Text("Right X %f", right_vec.x);
		ImGui::Text("Right Y %f", right_vec.y);
		ImGui::Text("Right Z %f", right_vec.z);

		camera_work->RenderImGui();
	}
}
