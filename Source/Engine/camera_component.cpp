#include "camera_component.h"

#include <cassert>
#include <cmath>
#include <wrl.h>

#include <imgui.h>

#include "arithmetic.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "framework.h"
#include "frame_buffer.h"
#include "locator.h"
#include "transform_component.h"
using namespace DirectX;

namespace cumulonimbus::component
{
	CameraComponent::CameraComponent(
		cumulonimbus::ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		bool is_main_camera,
		float width, float height)
		:ComponentBase{ registry,ent }
	{
		cb_camera  = std::make_shared<buffer::ConstantBuffer<CameraCB>>(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());
		off_screen = std::make_shared<FrameBuffer>(
			locator::Locator::GetDx11Device()->device.Get(),
			width, height);

		// 初期設定
		cb_camera->data.camera_position = { .0f,.0f,.0f };
		cb_camera->data.camera_at = { 0.0f,0.0f,1.0f };
		cb_camera->data.camera_far_z = 1.0f;
		cb_camera->data.camera_near_z = 0.0f;
		cb_camera->data.camera_right = { 1.0f,0.0f,0.0f };
		cb_camera->data.camera_up = { 0.0f,1.0f,0.0f };
		cb_camera->data.camera_front = { cb_camera->data.camera_at.x - cb_camera->data.camera_position.x,
											cb_camera->data.camera_at.y - cb_camera->data.camera_position.y,
											cb_camera->data.camera_at.z - cb_camera->data.camera_position.z };
		cb_camera->data.camera_fov = 0.0f;
		cb_camera->data.camera_aspect = 0.0f;
		cb_camera->data.camera_width = cumulonimbus::locator::Locator::GetWindow()->Width();
		cb_camera->data.camera_height = cumulonimbus::locator::Locator::GetWindow()->Height();
		cb_camera->data.camera_view_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		cb_camera->data.camera_view_projection_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

		if (is_main_camera)
			SwitchMainCamera();
	}

	void CameraComponent::Update(float dt)
	{
		if (is_use_camera_for_debug)
		{
			CalcCameraDirectionalVector();
		CalcCameraAngle(); //オイラー角で(現在)計算しているので今は使わない
			UpdateDefaultCamera(dt);
		}
		else
		{
			UpdateObjectCamera(dt);
		}

		SetFocusPosition(focus_position);
		SetEyePosition(eye_position);
		CalcCameraOrthogonalVector();

		// ビュー変換行列の作成
		const XMMATRIX view_mat = XMMatrixLookAtLH(XMVectorSet(eye_position.x, eye_position.y, eye_position.z, 0),
			XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 0),
			XMVectorSet(up_vec.x, up_vec.y, up_vec.z, 0));
		XMStoreFloat4x4(&this->view_mat, view_mat);

		// プロジェクション変換行列の作成
		XMMATRIX projection_mat = XMMatrixIdentity();
		if (is_perspective)
		{
			projection_mat = XMMatrixPerspectiveFovLH(fov, aspect, near_z, far_z);
			XMStoreFloat4x4(&this->projection_mat, projection_mat);
		}
		else
		{
			projection_mat = XMMatrixOrthographicLH(width, height, near_z, far_z);
			XMStoreFloat4x4(&this->projection_mat, projection_mat);
		}

		const XMMATRIX view_projection_mat = DirectX::XMMatrixMultiply(view_mat, projection_mat);
		XMStoreFloat4x4(&this->view_projection_mat, view_projection_mat);

		SetCBufferParam();
	}

	void CameraComponent::RenderImGui()
	{
		if (ImGui::TreeNode("CameraComponent"))
		{
			auto& camera = cb_camera->data;
			helper::imgui::Image(*off_screen->GetRenderTargetSRV(), { 160,100 });

			ImGui::Checkbox("Debug Camera", &is_use_camera_for_debug);
			ImGui::Text("CameraComponent Pos X %f", camera.camera_position.x);
			ImGui::Text("CameraComponent Pos Y %f", camera.camera_position.y);
			ImGui::Text("CameraComponent Pos Z %f", camera.camera_position.z);
			ImGui::Text("Focus X %f", camera.camera_at.x);
			ImGui::Text("Focus Y %f", camera.camera_at.y);
			ImGui::Text("Focus Z %f", camera.camera_at.z);
			ImGui::Text("CameraComponent Front X %f", camera.camera_front.x);
			ImGui::Text("CameraComponent Front Y %f", camera.camera_front.y);
			ImGui::Text("CameraComponent Front Z %f", camera.camera_front.z);
			ImGui::Text("CameraComponent Up X %f", camera.camera_up.x);
			ImGui::Text("CameraComponent Up Y %f", camera.camera_up.y);
			ImGui::Text("CameraComponent Up Z %f", camera.camera_up.z);
			ImGui::Text("CameraComponent Right X %f", camera.camera_right.x);
			ImGui::Text("CameraComponent Right Y %f", camera.camera_right.y);
			ImGui::Text("CameraComponent Right Z %f", camera.camera_right.z);

			ImGui::Text("focus_position x : %f\nfocus_position y : %f\nfocus_position z : %f", focus_position.x, focus_position.y, focus_position.z);
			ImGui::Text("angle x  : %f\nangle y  : %f\nangle z  : %f", camera_angle.x, camera_angle.y, camera_angle.z);
			ImGui::Text("Pos x  : %f\n Pos y  : %f\n Pos z  : %f", eye_position.x, eye_position.y, eye_position.z);

			ImGui::DragFloat2("CameraSpeed", (float*)&camera_velocity, 0.5f, 1, 10);
			ImGui::DragFloat("Camera Length", &camera_length, 0.1f, 0.1f, 1000.0f);


			ImGui::TreePop();
		}
	}

	void CameraComponent::Save(const std::string& file_path)
	{

	}

	void CameraComponent::Load(const std::string& file_path_and_name)
	{

	}

	void CameraComponent::AttachObject(cumulonimbus::mapping::rename_type::Entity ent)
	{
		attach_entity = ent;
		is_use_camera_for_debug = false;
	}

	void CameraComponent::InitializeObjectCameraParam(float camera_length)
	{
		this->camera_length = camera_length;
		auto& transform_comp = GetRegistry()->GetComponent<cumulonimbus::component::TransformComponent>(attach_entity);
		// カメラの位置をエンティティの持つオブジェクトの後方にセット
		eye_position = transform_comp.GetPosition() + (transform_comp.GetModelFront() * -1 * camera_length);
		// 注視点をエンティティの位置にセット
		focus_position = transform_comp.GetPosition();
	}

	void CameraComponent::SwitchMainCamera()
	{
		// 一度全てのCameraComponentのis_main_cameraをfalseに
		for(auto& camera_comp : GetRegistry()->GetArray<CameraComponent>().GetComponents())
		{
			camera_comp.is_main_camera = false;
		}
		// 自クラスのis_main_cameraをtrueに
		is_main_camera = true;
	}

	void CameraComponent::ClearFrameBuffer(DirectX::SimpleMath::Vector4 clear_color) const
	{
		off_screen->Clear(locator::Locator::GetDx11Device()->immediate_context.Get(), clear_color.x, clear_color.y, clear_color.z, clear_color.z);
	}

	void CameraComponent::BindFrameBuffer() const
	{
		off_screen->Activate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}

	void CameraComponent::UnbindFrameBuffer() const
	{
		off_screen->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}

	void CameraComponent::BindCBuffer(bool set_in_vs, bool set_in_ps) const
	{
		cb_camera->Activate(cumulonimbus::locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Camera, set_in_vs, set_in_ps);
	}

	void CameraComponent::UnbindCBuffer() const
	{
		cb_camera->Deactivate(cumulonimbus::locator::Locator::GetDx11Device()->immediate_context.Get());
	}



	void CameraComponent::CalcCameraOrthogonalVector()
	{
		if (!std::isfinite(focus_position.x) || !std::isfinite(focus_position.y) || !std::isfinite(focus_position.z))
		{
			assert(!"CameraComponent focus_position val is nan or infinity");
		}

		front_vec = SimpleMath::Vector3{ focus_position.x - eye_position.x,
										 focus_position.y - eye_position.y,
										 focus_position.z - eye_position.z };
		right_vec = arithmetic::CalcRightVec(current_up_vec, front_vec);
		current_up_vec = arithmetic::CalcUpVec(front_vec, right_vec);

		right_vec.Normalize();
		current_up_vec.Normalize();
		front_vec.Normalize();
	}

	void CameraComponent::UpdateObjectCamera(float dt)
	{
		//CalcCameraDirectionalVector();
		// camera_lengthとfront_vecからカメラの位置を算出
		auto& position = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();

		eye_position = position + (front_vec * -1) * camera_length;
	}

	void CameraComponent::UpdateDefaultCamera(float dt)
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
				Crane(camera_velocity.y, current_up_vec);
			if (key.GetState(Keycode::S) == ButtonState::Held)
				Crane(-camera_velocity.y, current_up_vec);
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
				Crane(camera_velocity.y, current_up_vec);
			if (key.GetState(Keycode::S) == ButtonState::Held)
				Crane(-camera_velocity.y, current_up_vec);
		}
	}

	void CameraComponent::RotationFrontVectorFromRightVector(const float radian)
	{
		SimpleMath::Quaternion q = SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(right_vec, radian);
		front_vec = SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();
	}

	void CameraComponent::RotationFrontVectorFromUpVector(const float radian)
	{
		SimpleMath::Quaternion q = SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(up_vec, radian);
		front_vec = SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();
	}

	void CameraComponent::SetViewInfo(
		SimpleMath::Vector3 pos,
		SimpleMath::Vector3 target,
		SimpleMath::Vector3 camera_up)
	{
		eye_position	= pos;
		focus_position	= target;
		current_up_vec  = camera_up;
	}

	void CameraComponent::SetProjection(const float fov,const float aspect,const float min,const float max)
	{
		is_perspective	= true;
		this->fov		= fov;
		this->aspect	= aspect;
		this->near_z	= min;
		this->far_z		= max;
	}

	void CameraComponent::SetOrtho(float width, float height, float min, float max)
	{
		is_perspective = false;
		this->width = width;
		this->height = height;
		near_z = min;
		far_z = max;
	}

	void CameraComponent::SetCameraRight(const DirectX::SimpleMath::Vector3& right)
	{
		if (arithmetic::IsEqual(right.x, 0.0f) &&
			arithmetic::IsEqual(right.y, 0.0f) &&
			arithmetic::IsEqual(right.z, 0.0f))
			assert(!"Vector is zero");

		right_vec = right;
	}

	void CameraComponent::SetCameraUp(const DirectX::SimpleMath::Vector3& up)
	{
		if (arithmetic::IsEqual(up.x, 0.0f) &&
			arithmetic::IsEqual(up.y, 0.0f) &&
			arithmetic::IsEqual(up.z, 0.0f))
			assert(!"Vector is zero");

		current_up_vec = up;
	}

	void CameraComponent::SetCameraFront(const DirectX::SimpleMath::Vector3& front)
	{
		if (arithmetic::IsEqual(front.x, 0.0f) &&
			arithmetic::IsEqual(front.y, 0.0f) &&
			arithmetic::IsEqual(front.z, 0.0f))
			assert(!"Vector is zero");

		current_up_vec = front;
	}


	void CameraComponent::CalcCameraDirectionalVector()
	{
		front_vec		= DirectX::SimpleMath::Vector3{ focus_position - eye_position };
		camera_right	= arithmetic::CalcRightVec(current_up_vec, front_vec);
		current_up_vec	= arithmetic::CalcUpVec(front_vec, camera_right);

		front_vec.Normalize();
		camera_right.Normalize();
		current_camera_up.Normalize();
	}

	void CameraComponent::CalcCameraAngle()
	{
		using namespace DirectX::SimpleMath;

		const Vector3 right{ 1,0,0 };
		const Vector3 up{ 0,1,0 };
		const Vector3 front{ 0,0,1 };
		camera_angle.x = DirectX::XMConvertToDegrees(acosf(up.Dot(current_up_vec)));
		if (up.Cross(current_up_vec).x < 0)
			camera_angle.x *= -1;
		camera_angle.y = DirectX::XMConvertToDegrees(acosf(front.Dot(front_vec)));
		if (front.Cross(front_vec).y < 0)
			camera_angle.y *= -1;

		camera_angle.z = arithmetic::CalcAngle_Z(front_vec);
	}

	void CameraComponent::SetCBufferParam() const
	{
		auto& camera_data = cb_camera->data;
		camera_data.camera_view_matrix = view_mat;
		camera_data.camera_projection_matrix = projection_mat;
		camera_data.camera_view_projection_matrix = view_projection_mat;

		camera_data.camera_position = eye_position;
		camera_data.camera_distance_from_origin = eye_position.Length();

		camera_data.camera_at = focus_position;
		camera_data.camera_near_z = near_z;

		camera_data.camera_up = current_up_vec;
		camera_data.camera_far_z = far_z;

		camera_data.camera_front = front_vec;
		camera_data.camera_aspect = aspect;

		camera_data.camera_width = width;
		camera_data.camera_height = height;
	}

	//-------------------------  デバッグカメラワーク  -------------------------//

	void CameraComponent::Pan(float velocity)
	{
		// クォータニオン Ver
		DirectX::SimpleMath::Quaternion q{};
		q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(current_up_vec, DirectX::XMConvertToRadians(velocity * camera_velocity.x * 0.1f));
		front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();

		focus_position = eye_position + front_vec;

		CalcCameraDirectionalVector();
	}

	void CameraComponent::Tilt(float velocity)
	{
		DirectX::SimpleMath::Quaternion q{};
		q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(right_vec, DirectX::XMConvertToRadians(velocity * camera_velocity.y * 0.1f));
		front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();
		focus_position = eye_position + front_vec;
		CalcCameraDirectionalVector();

		{//　カメラの角度保持(上下に+-90度まで)
			if (front_vec.y > 0)
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

	void CameraComponent::DollyInOut(float velocity)
	{
		const float v = -velocity * camera_velocity.y;

		focus_position += right_vec * velocity;
		eye_position += front_vec * v;
	}

	void CameraComponent::Track(float velocity, const DirectX::SimpleMath::Vector3& axis)
	{
		// カメラの注視点と位置を同じだけ移動
		focus_position += axis * velocity;
		eye_position += axis * velocity;
	}

	void CameraComponent::Crane(float velocity, const DirectX::SimpleMath::Vector3& axis)
	{
		// カメラの注視点と位置を同じだけ移動
		focus_position += axis * velocity;
		eye_position += axis * velocity;
	}

} // cumulonimbus::component
