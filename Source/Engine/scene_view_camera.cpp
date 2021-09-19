#include "scene_view_camera.h"

#include <cassert>
#include <cmath>
#include <wrl.h>

#include "arithmetic.h"
#include "cereal_helper.h"
#include "framework.h"
#include "frame_buffer.h"

namespace cumulonimbus::camera
{
	template <class Archive>
	void SceneViewCamera::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(max_camera_angle),

			CEREAL_NVP(eye_position),
			CEREAL_NVP(focus_position),
			CEREAL_NVP(focus_offset),

			CEREAL_NVP(up_vec),
			CEREAL_NVP(current_up_vec),
			CEREAL_NVP(right_vec),
			CEREAL_NVP(front_vec),

			CEREAL_NVP(current_camera_up),
			CEREAL_NVP(camera_right),

			CEREAL_NVP(view_mat),
			CEREAL_NVP(projection_mat),
			CEREAL_NVP(view_projection_mat),

			CEREAL_NVP(camera_angle),

			CEREAL_NVP(near_z),
			CEREAL_NVP(far_z),
			CEREAL_NVP(fov),
			CEREAL_NVP(aspect),
			CEREAL_NVP(width),
			CEREAL_NVP(height),

			CEREAL_NVP(camera_speed),
			CEREAL_NVP(cur_mouse_pos),
			CEREAL_NVP(prev_mouse_pos),

			CEREAL_NVP(camera_length),
			CEREAL_NVP(is_use_camera_for_debug),
			CEREAL_NVP(is_perspective),
			CEREAL_NVP(is_active),
			CEREAL_NVP(is_main_camera)
		);
	}

	SceneViewCamera::SceneViewCamera(float width, float height)
	{
		cb_camera  = std::make_unique<buffer::ConstantBuffer<CameraCB>>(cumulonimbus::locator::Locator::GetDx11Device()->device.Get());
		off_screen = std::make_unique<FrameBuffer>(
						locator::Locator::GetDx11Device()->device.Get(),
						width, height);

		// 初期設定
		this->width = width;
		this->height = height;

		cb_camera->data.camera_position = { .0f,.0f,.0f };
		cb_camera->data.camera_at		= { 0.0f,0.0f,1.0f };
		cb_camera->data.camera_far_z	= 1.0f;
		cb_camera->data.camera_near_z	= 0.0f;
		cb_camera->data.camera_right	= { 1.0f,0.0f,0.0f };
		cb_camera->data.camera_up		= { 0.0f,1.0f,0.0f };
		cb_camera->data.camera_front	= { cb_camera->data.camera_at.x - cb_camera->data.camera_position.x,
											cb_camera->data.camera_at.y - cb_camera->data.camera_position.y,
											cb_camera->data.camera_at.z - cb_camera->data.camera_position.z };
		cb_camera->data.camera_fov		= 0.0f;
		cb_camera->data.camera_aspect	= 0.0f;
		cb_camera->data.camera_width	= locator::Locator::GetWindow()->Width();
		cb_camera->data.camera_height	= locator::Locator::GetWindow()->Height();
		cb_camera->data.camera_view_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		cb_camera->data.camera_view_projection_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	}

	void SceneViewCamera::Update(float dt)
	{
		CalcCameraDirectionalVector();
		CalcCameraAngle(); //オイラー角で(現在)計算しているので今は使わない
		UpdateDefaultCamera(dt);

		CalcCameraOrthogonalVector();

		// ビュー変換行列の作成
		const DirectX::XMMATRIX view_mat = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(eye_position.x, eye_position.y, eye_position.z, 0),
		                                                             DirectX::XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 0),
		                                                             DirectX::XMVectorSet(up_vec.x, up_vec.y, up_vec.z, 0));
		XMStoreFloat4x4(&this->view_mat, view_mat);

		// プロジェクション変換行列の作成
		DirectX::XMMATRIX projection_mat = DirectX::XMMatrixIdentity();
		if (is_perspective)
		{
			projection_mat = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, near_z, far_z);
			XMStoreFloat4x4(&this->projection_mat, projection_mat);
		}
		else
		{
			projection_mat = DirectX::XMMatrixOrthographicLH(width, height, near_z, far_z);
			XMStoreFloat4x4(&this->projection_mat, projection_mat);
		}

		const DirectX::XMMATRIX view_projection_mat = DirectX::XMMatrixMultiply(view_mat, projection_mat);
		XMStoreFloat4x4(&this->view_projection_mat, view_projection_mat);

		SetCBufferParam();
	}

	void SceneViewCamera::RenderImGui()
	{
	}

	void SceneViewCamera::CalcCameraOrthogonalVector()
	{
		if (!std::isfinite(focus_position.x) || !std::isfinite(focus_position.y) || !std::isfinite(focus_position.z))
		{
			assert(!"CameraComponent focus_position val is nan or infinity");
		}

		front_vec = DirectX::SimpleMath::Vector3{ focus_position.x - eye_position.x,
												  focus_position.y - eye_position.y,
												  focus_position.z - eye_position.z };
		right_vec = arithmetic::CalcRightVec(current_up_vec, front_vec);
		current_up_vec = arithmetic::CalcUpVec(front_vec, right_vec);

		right_vec.Normalize();
		current_up_vec.Normalize();
		front_vec.Normalize();
	}

	void SceneViewCamera::UpdateDefaultCamera(float dt)
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
				Track(camera_speed.x, right_vec);
			if (key.GetState(Keycode::A) == ButtonState::Held)
				Track(-camera_speed.x, right_vec);
			if (key.GetState(Keycode::W) == ButtonState::Held)
				Crane(camera_speed.y, current_up_vec);
			if (key.GetState(Keycode::S) == ButtonState::Held)
				Crane(-camera_speed.y, current_up_vec);
		}
		else if (mouse.GetState(MouseButton::Right) == ButtonState::Held)
		{
			Pan(static_cast<float>(mouse.DeltaX()));
			Tilt(static_cast<float>(mouse.DeltaY()));

			if (key.GetState(Keycode::D) == ButtonState::Held)
				Track(camera_speed.x, right_vec);
			if (key.GetState(Keycode::A) == ButtonState::Held)
				Track(-camera_speed.x, right_vec);
			if (key.GetState(Keycode::W) == ButtonState::Held)
				Crane(camera_speed.y, current_up_vec);
			if (key.GetState(Keycode::S) == ButtonState::Held)
				Crane(-camera_speed.y, current_up_vec);
		}
	}

	void SceneViewCamera::CalcCameraDirectionalVector()
	{
		front_vec		= DirectX::SimpleMath::Vector3{ focus_position - eye_position };
		camera_right	= arithmetic::CalcRightVec(current_up_vec, front_vec);
		current_up_vec	= arithmetic::CalcUpVec(front_vec, camera_right);

		front_vec.Normalize();
		camera_right.Normalize();
		current_camera_up.Normalize();
	}

	void SceneViewCamera::CalcCameraAngle()
	{
		const DirectX::SimpleMath::Vector3 right{ 1,0,0 };
		const DirectX::SimpleMath::Vector3 up{ 0,1,0 };
		const DirectX::SimpleMath::Vector3 front{ 0,0,1 };
		camera_angle.x = DirectX::XMConvertToDegrees(acosf(up.Dot(current_up_vec)));
		if (up.Cross(current_up_vec).x < 0)
			camera_angle.x *= -1;
		camera_angle.y = DirectX::XMConvertToDegrees(acosf(front.Dot(front_vec)));
		if (front.Cross(front_vec).y < 0)
			camera_angle.y *= -1;

		camera_angle.z = arithmetic::CalcAngle_Z(front_vec);
	}

	void SceneViewCamera::AdjustCameraAngle()
	{
		{// X軸の回転角算出(ベクトルが{0,0,1}時に0度)
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
					DirectX::SimpleMath::Quaternion q = q.CreateFromAxisAngle(right_vec, diff);
					front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
					front_vec.Normalize();
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
					DirectX::SimpleMath::Quaternion q = q.CreateFromAxisAngle(right_vec, -diff);
					front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
					front_vec.Normalize();
				}
			}
		}
	}

	void SceneViewCamera::SetCBufferParam() const
	{
		auto& camera_data = cb_camera->data;
		camera_data.camera_view_matrix				= view_mat;
		camera_data.camera_projection_matrix		= projection_mat;
		camera_data.camera_view_projection_matrix	= view_projection_mat;

		camera_data.camera_position				= eye_position;
		camera_data.camera_distance_from_origin = eye_position.Length();

		camera_data.camera_at		= focus_position;
		camera_data.camera_near_z	= near_z;

		camera_data.camera_up		= current_up_vec;
		camera_data.camera_far_z	= far_z;

		camera_data.camera_right	= camera_right;
		camera_data.camera_fov		= fov;

		camera_data.camera_front	= front_vec;
		camera_data.camera_aspect	= aspect;

		camera_data.camera_width	= width;
		camera_data.camera_height	= height;
	}

	void SceneViewCamera::SetCameraRight(const DirectX::SimpleMath::Vector3& right)
	{
		if (arithmetic::IsEqual(right.x, 0.0f) &&
			arithmetic::IsEqual(right.y, 0.0f) &&
			arithmetic::IsEqual(right.z, 0.0f))
			assert(!"Vector is zero");

		right_vec = right;
	}

	void SceneViewCamera::SetCameraUp(const DirectX::SimpleMath::Vector3& up)
	{
		if (arithmetic::IsEqual(up.x, 0.0f) &&
			arithmetic::IsEqual(up.y, 0.0f) &&
			arithmetic::IsEqual(up.z, 0.0f))
			assert(!"Vector is zero");

		current_up_vec = up;
	}

	void SceneViewCamera::SetCameraFront(const DirectX::SimpleMath::Vector3& front)
	{
		if (arithmetic::IsEqual(front.x, 0.0f) &&
			arithmetic::IsEqual(front.y, 0.0f) &&
			arithmetic::IsEqual(front.z, 0.0f))
			assert(!"Vector is zero");

		current_up_vec = front;
	}

	void SceneViewCamera::Pan(float velocity)
	{
		// クォータニオン Ver
		DirectX::SimpleMath::Quaternion q{};
		q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(current_up_vec, DirectX::XMConvertToRadians(velocity * camera_speed.x * 0.1f));
		front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();

		focus_position = eye_position + front_vec;

		CalcCameraDirectionalVector();
	}

	void SceneViewCamera::Tilt(float velocity)
	{
		DirectX::SimpleMath::Quaternion q{};
		q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(right_vec, DirectX::XMConvertToRadians(velocity * camera_speed.y * 0.1f));
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

	void SceneViewCamera::DollyInOut(float velocity)
	{
		const float v = -velocity * camera_speed.y;

		focus_position += right_vec * velocity;
		eye_position += front_vec * v;
	}

	void SceneViewCamera::Track(float velocity, const DirectX::SimpleMath::Vector3& axis)
	{
		// カメラの注視点と位置を同じだけ移動
		focus_position += axis * velocity;
		eye_position += axis * velocity;
	}

	void SceneViewCamera::Crane(float velocity, const DirectX::SimpleMath::Vector3& axis)
	{
		// カメラの注視点と位置を同じだけ移動
		focus_position += axis * velocity;
		eye_position += axis * velocity;
	}
} // cumulonimbus::camera
