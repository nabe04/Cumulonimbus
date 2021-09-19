#include "light.h"

#include "imgui.h"

#include "locator.h"
#include "arithmetic.h"
#include "constant_buffer.h"
#include "camera_component.h"

using namespace DirectX;

Light::Light(ID3D11Device* device)
{
	cb_light		= std::make_unique<buffer::ConstantBuffer<LightCB>>(device);
	// ƒ‰ƒCƒg‚Ì‰ŠúÝ’è
	cb_light->data.light_position			= { 10.0f,20.0f,-10.0f };
	cb_light->data.light_direction			= { 0.0f,-1.0f,1.0f };
	cb_light->data.light_color				= { 1.0f,1.0f,1.0f };
	cb_light->data.orthographic_view_width	= 2560;
	cb_light->data.orthographic_view_height = 2560;
	cb_light->data.orthographic_near_z		= 0.1f;
	cb_light->data.orthographic_far_z		= 1000;
}

//template <class Archive>
//void Light::serialize(Archive&& archive)
//{
//	archive(
//		CEREAL_NVP(view_right),
//		CEREAL_NVP(view_up),
//		CEREAL_NVP(view_front)
//	);
//}


void Light::Update(const cumulonimbus::component::CameraComponent* camera)
{
	// Calculate right,up,front vector
	const DirectX::SimpleMath::Vector3 l_direction = cb_light->data.light_direction;
	view_front = l_direction;
	view_front.Normalize();
	view_right = arithmetic::CalcRightVec(view_up, view_front);
	view_right.Normalize();
	view_up    = arithmetic::CalcUpVec(view_front, view_right);
	view_up.Normalize();

	DirectX::XMFLOAT3 target_pos = camera->GetCamera()->GetFocusPosition();
	DirectX::XMFLOAT3 position   = cb_light->data.light_position;
	const XMMATRIX view_mat      = XMMatrixLookAtLH(XMLoadFloat3(&position), { 0,0,0 }, XMLoadFloat3(&view_up));

	cb_light->data.light_view_matrix = camera->GetCamera()->GetViewMat();
	const XMMATRIX perspective_projection_mat = XMMatrixPerspectiveFovLH(camera->GetCamera()->GetFov(), camera->GetCamera()->GetAspect(), camera->GetCamera()->GetNearZ(), camera->GetCamera()->GetFarZ());
	XMStoreFloat4x4(&cb_light->data.light_perspective_projection_matrix, perspective_projection_mat);
	XMStoreFloat4x4(&cb_light->data.light_perspective_view_projection_matrix, XMMatrixMultiply(view_mat, perspective_projection_mat));
	const XMMATRIX orthographic_projection_mat = XMMatrixOrthographicLH(cb_light->data.orthographic_view_width, cb_light->data.orthographic_view_height, cb_light->data.orthographic_near_z, cb_light->data.orthographic_far_z);
	XMStoreFloat4x4(&cb_light->data.light_orthographic_projection_matrix, orthographic_projection_mat);
	XMStoreFloat4x4(&cb_light->data.light_orthographic_view_projection_matrix, XMMatrixMultiply(view_mat, orthographic_projection_mat));
}

void Light::BindCBuffer(bool set_in_vs, bool set_in_ps) const
{
	cb_light->Activate(cumulonimbus::locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Light, set_in_vs, set_in_ps);
}

void Light::UnbindCBuffer() const
{
	cb_light->Deactivate(cumulonimbus::locator::Locator::GetDx11Device()->immediate_context.Get());
}

void Light::Load() const
{
	//if (cb_light.get())
	//	cb_light.reset();

	cb_light->Load(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());

	//cb_light = std::make_unique<buffer::ConstantBuffer<LightCB>>(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());
}


void Light::WriteImGui() const
{
	if (ImGui::CollapsingHeader("Light Info"))
	{
		ImGui::DragFloat("Position X", &cb_light->data.light_position.x, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Position Y", &cb_light->data.light_position.y, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Position Z", &cb_light->data.light_position.z, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Direction X", &cb_light->data.light_direction.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Direction Y", &cb_light->data.light_direction.y, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Direction Z", &cb_light->data.light_direction.z, 0.01f, -1.0f, 1.0f);
		ImGui::ColorEdit3("Light Color", (float*)&cb_light->data.light_color);
	}
}