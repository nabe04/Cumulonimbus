#include "view.h"

#include <cassert>
#include <cmath>
#include <wrl.h>

#include <imgui.h>

#include "arithmetic.h"
#include "framework.h"
#include "locator.h"
#include "camera_work.h"
using namespace DirectX;

View::View(cumulonimbus::ecs::Registry* registry)
{
	camera_work = std::make_unique<CameraWork>(registry);
	cb_camera   = std::make_unique<buffer::ConstantBuffer<CameraCB>>(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());

	// ‰ŠúÝ’è
	cb_camera->data.camera_position = { .0f,.0f,.0f };
	cb_camera->data.camera_at		= { 0.0f,0.0f,1.0f };
	cb_camera->data.camera_far_z	= 1.0f;
	cb_camera->data.camera_near_z	= 0.0f;
	cb_camera->data.camera_right	= { 1.0f,0.0f,0.0f };
	cb_camera->data.camera_up		= { 0.0f,1.0f,0.0f };
	cb_camera->data.camera_front    = { cb_camera->data.camera_at.x - cb_camera->data.camera_position.x,
									    cb_camera->data.camera_at.y - cb_camera->data.camera_position.y,
										cb_camera->data.camera_at.z - cb_camera->data.camera_position.z};
	cb_camera->data.camera_fov		= 0.0f;
	cb_camera->data.camera_aspect	= 0.0f;
	cb_camera->data.camera_width	= cumulonimbus::locator::Locator::GetWindow()->Width();
	cb_camera->data.camera_height	= cumulonimbus::locator::Locator::GetWindow()->Height();
	cb_camera->data.camera_view_matrix				= { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	cb_camera->data.camera_view_projection_matrix	= { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
}

View::~View()
{

}

void View::BindCBuffer(bool set_in_vs, bool set_in_ps) const
{
	cb_camera->Activate(cumulonimbus::locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Camera, set_in_vs, set_in_ps);
}

void View::UnbindCBuffer() const
{
	cb_camera->Deactivate(cumulonimbus::locator::Locator::GetDx11Device()->immediate_context.Get());
}



void View::CalcCameraOrthogonalVector()
{
	auto& camera = cb_camera->data;

	if (!std::isfinite(camera.camera_at.x) || !std::isfinite(camera.camera_at.y) || !std::isfinite(camera.camera_at.z))
	{
		assert(!"Camera focus_position val is nan or infinity");
	}

	camera.camera_front = DirectX::SimpleMath::Vector3{ camera.camera_at.x - camera.camera_position.x,
														camera.camera_at.y - camera.camera_position.y,
														camera.camera_at.z - camera.camera_position.z };
	camera.camera_right = arithmetic::CalcRightVec(camera.camera_up, camera.camera_front);
	camera.camera_up    = arithmetic::CalcUpVec(camera.camera_front, camera.camera_right);
	//up_vec	  = DirectX::SimpleMath::Vector3{ 0,1,0 };

	XMStoreFloat3(&camera.camera_front	, XMVector3Normalize(XMLoadFloat3(&camera.camera_front)));
	XMStoreFloat3(&camera.camera_up		, XMVector3Normalize(XMLoadFloat3(&camera.camera_up)));
	XMStoreFloat3(&camera.camera_right	, XMVector3Normalize(XMLoadFloat3(&camera.camera_right)));
}

void View::SetCameraPos(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 up)
{
	auto& camera = cb_camera->data;
	camera.camera_position	= pos;
	camera.camera_at		= target;
	camera.camera_up		= up;
}

void View::SetProjection(float fov, float aspect, float min, float max)
{
	auto& camera			= cb_camera->data;
	is_perspective			= true;
	camera.camera_fov		= fov;
	camera.camera_aspect	= aspect;
	camera.camera_near_z	= min;
	camera.camera_far_z		= max;
}

void View::SetOrtho(float width, float height, float min, float max)
{
	auto& camera			= cb_camera->data;
	is_perspective			= false;
	camera.camera_width		= width;
	camera .camera_height	= height;
	camera.camera_near_z	= min;
	camera.camera_far_z		= max;
}

void View::Update(float dt)
{
	camera_work->SetCameraInfo(*this);
	camera_work->Update(dt);
	SetFocusPosition(camera_work->GetFocusPosition());
	SetEyePosition(camera_work->GetPosition());
	CalcCameraOrthogonalVector();

#ifdef _DEBUG
	if (cumulonimbus::locator::Locator::GetInput()->Keyboard().GetState(Keycode::LeftShift) == ButtonState::Held)
	{
		if (cumulonimbus::locator::Locator::GetInput()->Keyboard().GetState(Keycode::F1) == ButtonState::Press)
		{
			is_debug_camera = !is_debug_camera;
		}
	}
#endif
	auto& camera = cb_camera->data;

	const XMMATRIX view_mat = XMMatrixLookAtLH( XMVectorSet(camera.camera_position.x, camera.camera_position.y, camera.camera_position.z, 0),
												XMVectorSet(camera.camera_at.x, camera.camera_at.y, camera.camera_at.z, 0),
												XMVectorSet(view_up_vec.x, view_up_vec.y, view_up_vec.z, 0));
	XMStoreFloat4x4(&camera.camera_view_matrix, view_mat);

	XMMATRIX projection_mat = XMMatrixIdentity();
	if (is_perspective)
	{
		projection_mat = XMMatrixPerspectiveFovLH(camera.camera_fov, camera.camera_aspect, camera.camera_near_z, camera.camera_far_z);
		XMStoreFloat4x4(&camera.camera_projection_matrix, projection_mat);
	}
	else
	{
		projection_mat = XMMatrixOrthographicLH(camera.camera_width, camera.camera_height, camera.camera_near_z, camera.camera_far_z);
		XMStoreFloat4x4(&camera.camera_projection_matrix, projection_mat);
	}

	const XMMATRIX view_projection_mat = DirectX::XMMatrixMultiply(view_mat, projection_mat);
	XMStoreFloat4x4(&camera.camera_view_projection_matrix, view_projection_mat);
}

void View::WriteImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		auto& camera = cb_camera->data;

		ImGui::Checkbox("Debug", &is_debug_camera);

		ImGui::Text("Camera Pos X %f"	, camera.camera_position.x);
		ImGui::Text("Camera Pos Y %f"	, camera.camera_position.y);
		ImGui::Text("Camera Pos Z %f"	, camera.camera_position.z);
		ImGui::Text("Focus X %f"		, camera.camera_at.x);
		ImGui::Text("Focus Y %f"		, camera.camera_at.y);
		ImGui::Text("Focus Z %f"		, camera.camera_at.z);
		ImGui::Text("Camera Front X %f"	, camera.camera_front.x);
		ImGui::Text("Camera Front Y %f"	, camera.camera_front.y);
		ImGui::Text("Camera Front Z %f"	, camera.camera_front.z);
		ImGui::Text("Camera Up X %f"	, camera.camera_up.x);
		ImGui::Text("Camera Up Y %f"	, camera.camera_up.y);
		ImGui::Text("Camera Up Z %f"	, camera.camera_up.z);
		ImGui::Text("Camera Right X %f"	, camera.camera_right.x);
		ImGui::Text("Camera Right Y %f"	, camera.camera_right.y);
		ImGui::Text("Camera Right Z %f"	, camera.camera_right.z);

		camera_work->RenderImGui();
	}
}
