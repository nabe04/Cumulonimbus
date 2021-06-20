#include "light.h"

#include "imgui.h"

#include "arithmetic.h"
#include "constant_buffer.h"
#include "view.h"

using namespace DirectX;

Light::Light(ID3D11Device* device, const LightData& light_data)
{
	cb_light = std::make_unique<buffer::ConstantBuffer<LightData>>(device);
	cb_light->data = light_data;
}

void Light::Update(const View* view)
{
	// Calculate right,up,front vector
	const DirectX::SimpleMath::Vector3 l_direction = { cb_light->data.direction.x,cb_light->data.direction.y,cb_light->data.direction.z };
	view_front = l_direction;
	view_front.Normalize();
	view_right = arithmetic::CalcRightVec(view_up, view_front);
	view_right.Normalize();
	view_up    = arithmetic::CalcUpVec(view_front, view_right);
	view_up.Normalize();

	DirectX::XMFLOAT3 target_pos = view->GetFocusPosition();
	DirectX::XMFLOAT3 position = { cb_light->data.position.x, cb_light->data.position.y, cb_light->data.position.z };
	const XMMATRIX view_mat = XMMatrixLookAtLH(XMLoadFloat3(&position), { 0,0,0 }, XMLoadFloat3(&view_up));
	XMStoreFloat4x4(&cb_light->data.light_view_matrix, view_mat);

	const XMMATRIX perspective_projection_mat = XMMatrixPerspectiveFovLH(view->GetFov(), view->GetAspect(), view->GetNearZ(), view->GetFarZ());
	XMStoreFloat4x4(&cb_light->data.perspective_projection_matrix, perspective_projection_mat);
	XMStoreFloat4x4(&cb_light->data.perspective_view_projection_matrix, XMMatrixMultiply(view_mat, perspective_projection_mat));
	const XMMATRIX orthographic_projection_mat = XMMatrixOrthographicLH(cb_light->data.orthographic_view_width, cb_light->data.orthographic_view_height, cb_light->data.orthographic_near_z, cb_light->data.orthographic_far_z);
	XMStoreFloat4x4(&cb_light->data.orthographic_projection_matrix, orthographic_projection_mat);
	XMStoreFloat4x4(&cb_light->data.orthographic_view_projection_matrix, XMMatrixMultiply(view_mat, orthographic_projection_mat));
}

void Light::ActivateCBuffer(ID3D11DeviceContext* const immediate_context, const bool set_in_vs, const bool set_in_ps) const
{
	cb_light->Activate(immediate_context, cb_slot, set_in_vs, set_in_ps);
}

void Light::DeactivateCBuffer(ID3D11DeviceContext* const immediate_context) const
{
	cb_light->Deactivate(immediate_context);
}


void Light::WriteImGui() const
{
	if (ImGui::CollapsingHeader("Light Info"))
	{
		ImGui::DragFloat("Position X", &cb_light->data.position.x, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Position Y", &cb_light->data.position.y, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Position Z", &cb_light->data.position.z, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Direction X", &cb_light->data.direction.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Direction Y", &cb_light->data.direction.y, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Direction Z", &cb_light->data.direction.z, 0.01f, -1.0f, 1.0f);
		ImGui::ColorEdit3("Ambient", (float*)&cb_light->data.ambient);
		ImGui::ColorEdit4("Light Color", (float*)&cb_light->data.color);
	}
}