#include "light.h"

#include "imgui.h"

#include "arithmetic.h"
#include "view.h"

using namespace DirectX;

void Light::Update(const View* view)
{
	// Calculate right,up,front vector
	DirectX::SimpleMath::Vector3 direction_v3 = { direction.x,direction.y,direction.z };
	view_front = direction_v3;
	view_front.Normalize();
	view_right = arithmetic::CalcRightVec(view_up, view_front);
	view_right.Normalize();
	view_up    = arithmetic::CalcUpVec(view_front, view_right);
	view_up.Normalize();

	DirectX::XMFLOAT3 target_pos = view->GetTarget();

	XMMATRIX view_mat = XMMatrixLookAtLH(XMLoadFloat3(&position), {0,0,0}, XMLoadFloat3(&view_up));
	XMStoreFloat4x4(&light_view_matrix, view_mat);
	XMMATRIX proj_mat = XMMatrixOrthographicLH(200,200, view->GetNearZ(),1000);
	//XMMATRIX proj_mat = XMMatrixPerspectiveFovLH(view->Fov(), view->Aspect(), view->NearZ(), view->FarZ());
	XMStoreFloat4x4(&light_projection_matrix, proj_mat);

	XMStoreFloat4x4(&light_view_projection_matrix, XMMatrixMultiply(view_mat, proj_mat));
}

void Light::WriteImGui()
{
	if (ImGui::CollapsingHeader("Light Info"))
	{
		ImGui::DragFloat("Position X", &position.x, 0.01f, -100.0f, 100.0f);
		ImGui::DragFloat("Position Y", &position.y, 0.01f, -100.0f, 100.0f);
		ImGui::DragFloat("Position Z", &position.z, 0.01f, -100.0f, 100.0f);
		ImGui::DragFloat("Direction X", &direction.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Direction Y", &direction.y, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Direction Z", &direction.z, 0.01f, -1.0f, 1.0f);
		ImGui::ColorEdit3("Ambient", (float*)&ambient);
		ImGui::ColorEdit4("Light Color", (float*)&color);
	}
}