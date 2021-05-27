#include "view.h"

#include <cassert>
#include <cmath>

#include <wrl.h>

#include <imgui.h>

#include "arithmetic.h"
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
	right_vec = arithmetic::CalcRightVec(up_vec, front_vec);
	up_vec    = arithmetic::CalcUpVec(front_vec, right_vec);
	//up_vec	  = DirectX::SimpleMath::Vector3{ 0,1,0 };

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

	const XMMATRIX view_mat = XMMatrixLookAtLH( XMVectorSet(eye_position.x, eye_position.y, eye_position.z, 0),
												XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 0),
												XMVectorSet(view_up_vec.x, view_up_vec.y, view_up_vec.z, 0));
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

void View::WriteImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Checkbox("Debug", &is_debug_camera);

		ImGui::Text("Camera Pos X %f", eye_position.x);
		ImGui::Text("Camera Pos Y %f", eye_position.y);
		ImGui::Text("Camera Pos Z %f", eye_position.z);
		ImGui::Text("Focus X %f", focus_position.x);
		ImGui::Text("Focus Y %f", focus_position.y);
		ImGui::Text("Focus Z %f", focus_position.z);
		ImGui::Text("Camera Front X %f", front_vec.x);
		ImGui::Text("Camera Front Y %f", front_vec.y);
		ImGui::Text("Camera Front Z %f", front_vec.z);
		ImGui::Text("Camera Up X %f", up_vec.x);
		ImGui::Text("Camera Up Y %f", up_vec.y);
		ImGui::Text("Camera Up Z %f", up_vec.z);
		ImGui::Text("Camera Right X %f", right_vec.x);
		ImGui::Text("Camera Right Y %f", right_vec.y);
		ImGui::Text("Camera Right Z %f", right_vec.z);

		camera_work->RenderImGui();
	}
}
