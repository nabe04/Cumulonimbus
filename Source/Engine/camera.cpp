#include "camera.h"

#include "arithmetic.h"
#include "cum_imgui_helper.h"
#include "locator.h"

namespace cumulonimbus::camera
{
	Camera::Camera(const float width, const float height)
	{
		// 初期設定
		this->width  = width;
		this->height = height;

		Initialize();

		cb_camera->GetData().camera_position = { .0f,.0f,.0f };
		cb_camera->GetData().camera_at		 = { 0.0f,0.0f,1.0f };
		cb_camera->GetData().camera_far_z	 = 1.0f;
		cb_camera->GetData().camera_near_z	 = 0.0f;
		cb_camera->GetData().camera_right	 = { 1.0f,0.0f,0.0f };
		cb_camera->GetData().camera_up		 = { 0.0f,1.0f,0.0f };
		cb_camera->GetData().camera_front	 = { cb_camera->GetData().camera_at.x - cb_camera->GetData().camera_position.x,
											     cb_camera->GetData().camera_at.y - cb_camera->GetData().camera_position.y,
											     cb_camera->GetData().camera_at.z - cb_camera->GetData().camera_position.z };
		cb_camera->GetData().camera_fov		= 0.0f;
		cb_camera->GetData().camera_aspect	= 0.0f;
		cb_camera->GetData().camera_width	= static_cast<float>(locator::Locator::GetWindow()->Width());
		cb_camera->GetData().camera_height	= static_cast<float>(locator::Locator::GetWindow()->Height());
		cb_camera->GetData().camera_view_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		cb_camera->GetData().camera_view_projection_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

		SetViewInfo(cb_camera->GetData().camera_position, cb_camera->GetData().camera_at, cb_camera->GetData().camera_up);
		SetProjection(DirectX::XM_PI / 8.0f, static_cast<float>(locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);
	}

	Camera::Camera(const Camera& other)
		:max_camera_angle{other.max_camera_angle},
		 eye_position{other.eye_position},
		 focus_position{other.focus_position},
		 focus_offset{other.focus_offset},
		 up_vec{other.up_vec},
		 current_up_vec{other.current_up_vec},
		 right_vec{other.right_vec},
		 front_vec{other.front_vec},
		 current_camera_up{other.current_camera_up},
		 camera_right{other.camera_right},
		 view_mat{other.view_mat},
		 projection_mat{other.projection_mat},
		 view_projection_mat{other.view_projection_mat},
		 camera_angle{other.camera_angle},
		 near_z{other.near_z},
		 far_z{other.far_z},
		 fov{other.fov},
		 aspect{other.aspect},
		 width{other.width},
		 height{other.height},
		 camera_speed{other.camera_speed},
		 cur_mouse_pos{other.cur_mouse_pos},
		 prev_mouse_pos{other.prev_mouse_pos},
		 is_perspective{other.is_perspective}
	{
		Initialize();

		cb_camera->GetData().camera_position = eye_position;
		cb_camera->GetData().camera_at		= focus_position;
		cb_camera->GetData().camera_far_z	= far_z;
		cb_camera->GetData().camera_near_z	= near_z;
		cb_camera->GetData().camera_right	= right_vec;
		cb_camera->GetData().camera_up		= up_vec;
		cb_camera->GetData().camera_front	= { cb_camera->GetData().camera_at.x - cb_camera->GetData().camera_position.x,
											    cb_camera->GetData().camera_at.y - cb_camera->GetData().camera_position.y,
											    cb_camera->GetData().camera_at.z - cb_camera->GetData().camera_position.z };
		cb_camera->GetData().camera_fov		= fov;
		cb_camera->GetData().camera_aspect	= aspect;
		cb_camera->GetData().camera_width	= width;
		cb_camera->GetData().camera_height	= height;
		cb_camera->GetData().camera_view_matrix = view_mat;
		cb_camera->GetData().camera_view_projection_matrix = view_projection_mat;
		cb_camera->GetData().camera_inv_view_matrix = view_mat.Invert();
		cb_camera->GetData().camera_inv_projection_matrix = projection_mat.Invert();
		cb_camera->GetData().camera_inv_view_projection_matrix = view_projection_mat.Invert();


		SetViewInfo(cb_camera->GetData().camera_position, cb_camera->GetData().camera_at, cb_camera->GetData().camera_up);
		SetProjection(DirectX::XM_PI / 8.0f, static_cast<float>(locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);
	}

	Camera& Camera::operator=(const Camera& other)
	{
		if (this == &other)
		{
			return *this;
		}

		max_camera_angle		= other.max_camera_angle;
		eye_position			= other.eye_position;
		focus_position			= other.focus_position;
		focus_offset			= other.focus_offset;
		up_vec					= other.up_vec;
		current_up_vec			= other.current_up_vec;
		right_vec				= other.right_vec;
		front_vec				= other.front_vec;
		current_camera_up		= other.current_camera_up;
		camera_right			= other.camera_right;
		view_mat				= other.view_mat;
		projection_mat			= other.projection_mat;
		view_projection_mat		= other.view_projection_mat;
		camera_angle			= other.camera_angle;
		near_z					= other.near_z;
		far_z					= other.far_z;
		fov						= other.fov;
		aspect					= other.aspect;
		width					= other.width;
		height					= other.height;
		camera_speed			= other.camera_speed;
		cur_mouse_pos			= other.cur_mouse_pos;
		prev_mouse_pos			= other.prev_mouse_pos;
		is_perspective			= other.is_perspective;

		Initialize();

		cb_camera->GetData().camera_position = eye_position;
		cb_camera->GetData().camera_at		 = focus_position;
		cb_camera->GetData().camera_far_z	 = far_z;
		cb_camera->GetData().camera_near_z	 = near_z;
		cb_camera->GetData().camera_right	 = right_vec;
		cb_camera->GetData().camera_up		 = up_vec;
		cb_camera->GetData().camera_front	 = { cb_camera->GetData().camera_at.x - cb_camera->GetData().camera_position.x,
											     cb_camera->GetData().camera_at.y - cb_camera->GetData().camera_position.y,
											     cb_camera->GetData().camera_at.z - cb_camera->GetData().camera_position.z };
		cb_camera->GetData().camera_fov		= fov;
		cb_camera->GetData().camera_aspect	= aspect;
		cb_camera->GetData().camera_width	= width;
		cb_camera->GetData().camera_height	= height;
		cb_camera->GetData().camera_view_matrix					= view_mat;
		cb_camera->GetData().camera_projection_matrix			= projection_mat;
		cb_camera->GetData().camera_view_projection_matrix		= view_projection_mat;
		cb_camera->GetData().camera_inv_view_matrix				= view_mat.Invert();
		cb_camera->GetData().camera_inv_projection_matrix		= projection_mat.Invert();
		cb_camera->GetData().camera_inv_view_projection_matrix	= view_projection_mat.Invert();

		SetViewInfo(cb_camera->GetData().camera_position, cb_camera->GetData().camera_at, cb_camera->GetData().camera_up);
		SetProjection(DirectX::XM_PI / 8.0f, static_cast<float>(locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);

		return *this;
	}

	void Camera::Initialize()
	{
		if (cb_camera)
			cb_camera.reset();
		cb_camera	= std::make_shared<buffer::ConstantBuffer<CameraCB>>(locator::Locator::GetDx11Device()->device.Get());

		if (off_screen)
			off_screen.reset();
		off_screen	= std::make_shared<FrameBuffer>(
						locator::Locator::GetDx11Device()->device.Get(),
						static_cast<int>(width), static_cast<int>(height));
	}

	void Camera::Update(float dt)
	{
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

	void Camera::RenderImGui()
	{
		auto& camera = cb_camera->GetData();
		helper::imgui::Image(*off_screen->GetRenderTargetSRV(), { 160,100 });
		//ImGui::Text("-----------------------");
		//ImGui::Text("Camera Pos X %f", camera.camera_position.x);
		//ImGui::Text("Camera Pos Y %f", camera.camera_position.y);
		//ImGui::Text("Camera Pos Z %f", camera.camera_position.z);
		//ImGui::Text("-----------------------");
		//ImGui::Text("Focus X %f", camera.camera_at.x);
		//ImGui::Text("Focus Y %f", camera.camera_at.y);
		//ImGui::Text("Focus Z %f", camera.camera_at.z);
		//ImGui::Text("-----------------------");
		//ImGui::Text("Camera Front X %f", camera.camera_front.x);
		//ImGui::Text("Camera Front Y %f", camera.camera_front.y);
		//ImGui::Text("Camera Front Z %f", camera.camera_front.z);
		//ImGui::Text("-----------------------");
		//ImGui::Text("Camera Up X %f", camera.camera_up.x);
		//ImGui::Text("Camera Up Y %f", camera.camera_up.y);
		//ImGui::Text("Camera Up Z %f", camera.camera_up.z);
		//ImGui::Text("-----------------------");
		//ImGui::Text("Camera Right X %f", camera.camera_right.x);
		//ImGui::Text("Camera Right Y %f", camera.camera_right.y);
		//ImGui::Text("Camera Right Z %f", camera.camera_right.z);

		//ImGui::Text("focus_position x : %f\nfocus_position y : %f\nfocus_position z : %f", focus_position.x, focus_position.y, focus_position.z);
		//ImGui::Text("angle x  : %f\nangle y  : %f\nangle z  : %f", camera_angle.x, camera_angle.y, camera_angle.z);
		//ImGui::Text("Pos x  : %f\n Pos y  : %f\n Pos z  : %f", eye_position.x, eye_position.y, eye_position.z);

		ImGui::DragFloat3("Offset", &focus_offset.x, 0.5f, -FLT_MAX, FLT_MAX);
	}

	void Camera::ClearFrameBuffer(const DirectX::SimpleMath::Vector4& clear_color) const
	{
		off_screen->Clear(
			locator::Locator::GetDx11Device()->immediate_context.Get(),
			clear_color.x, clear_color.y, clear_color.z, clear_color.z);
	}

	void Camera::BindFrameBuffer() const
	{
		off_screen->Activate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}

	void Camera::UnbindFrameBuffer() const
	{
		off_screen->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}

	void Camera::BindCBuffer(const bool set_in_vs, const bool set_in_ps) const
	{
		cb_camera->Activate(
			locator::Locator::GetDx11Device()->immediate_context.Get(),
			CBSlot_Camera, set_in_vs, set_in_ps);
	}

	void Camera::UnbindCBuffer() const
	{
		cb_camera->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}

	void Camera::BindSrvTexture(const int slot) const
	{
		if(slot < 0)
		{// デフォルトのスロットでセットする
			locator::Locator::GetDx11Device()->BindShaderResource(
				mapping::graphics::ShaderStage::PS,
				off_screen->GetRenderTargetSRV(),
				TexSlot_BaseColorMap);
		}
		else
		{
			locator::Locator::GetDx11Device()->BindShaderResource(
				mapping::graphics::ShaderStage::PS,
				off_screen->GetRenderTargetSRV(),
				slot);
		}
	}

	void Camera::UnbindSrvTexture(const int slot) const
	{
		if (slot < 0)
		{// デフォルトのスロットでセットする
			locator::Locator::GetDx11Device()->UnbindShaderResource(
													mapping::graphics::ShaderStage::PS,
													TexSlot_BaseColorMap);
		}
		else
		{
			locator::Locator::GetDx11Device()->UnbindShaderResource(
													mapping::graphics::ShaderStage::PS,
													slot);
		}
	}

	void Camera::BindDsvTexture(const int slot) const
	{
		if (slot < 0)
		{// デフォルトのスロットでセットする
			locator::Locator::GetDx11Device()->BindShaderResource(
													mapping::graphics::ShaderStage::PS,
													off_screen->GetDepthStencilSRV(),
													TexSlot_Depth);
		}
		else
		{
			locator::Locator::GetDx11Device()->BindShaderResource(
													mapping::graphics::ShaderStage::PS,
													off_screen->GetDepthStencilSRV(),
													slot);
		}
	}

	void Camera::UnbindDsvTexture(const int slot) const
	{
		if (slot < 0)
		{// デフォルトのスロットでセットする
			locator::Locator::GetDx11Device()->UnbindShaderResource(
													mapping::graphics::ShaderStage::PS,
													TexSlot_Depth);
		}
		else
		{
			locator::Locator::GetDx11Device()->UnbindShaderResource(
													mapping::graphics::ShaderStage::PS,
													slot);
		}
	}

	void Camera::RotationFrontVectorFromRightVector(const float radian)
	{
		DirectX::SimpleMath::Quaternion q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(right_vec, radian);
		front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();
	}

	void Camera::RotationFrontVectorFromUpVector(const float radian)
	{
		DirectX::SimpleMath::Quaternion q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(up_vec, radian);
		front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();
	}

	void Camera::RotationTPSPitch(const float radian)
	{
		DirectX::SimpleMath::Vector3 v = DirectX::SimpleMath::Vector3{ 0,1,0 }.Cross(front_vec);
		v.Normalize();
		DirectX::SimpleMath::Quaternion q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(v, radian);
		front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();
	}

	void Camera::RotationTPSYaw(const float radian)
	{
		DirectX::SimpleMath::Quaternion q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle({ 0,1,0 }, radian);
		front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();
	}

	void Camera::SetViewInfo(
		const DirectX::SimpleMath::Vector3& pos,
		const DirectX::SimpleMath::Vector3& target,
		const DirectX::SimpleMath::Vector3& camera_up)
	{
		eye_position	= pos;
		focus_position	= target;
		current_up_vec	= camera_up;
	}

	void Camera::SetProjection(
		const float fov, const float aspect,
		const float min, const float max)
	{
		is_perspective	= true;
		this->fov		= fov;
		this->aspect	= aspect;
		this->near_z	= min;
		this->far_z		= max;
	}

	void Camera::SetOrtho(
		const float width, const float height,
		const float min  , const float max)
	{
		is_perspective	= false;
		this->width		= width;
		this->height	= height;
		near_z			= min;
		far_z			= max;
	}

	void Camera::CalcCameraOrthogonalVector()
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

	//void Camera::UpdateDefaultCamera(float dt)
	//{
	//	const auto& mouse = cumulonimbus::locator::Locator::GetInput()->Mouse();
	//	const auto& key = cumulonimbus::locator::Locator::GetInput()->Keyboard();

	//	if (mouse.GetState(MouseButton::Left) == ButtonState::Held &&
	//		mouse.GetState(MouseButton::Right) == ButtonState::Held)
	//	{
	//		Track(static_cast<float>(mouse.DeltaX()), right_vec);
	//		Crane(static_cast<float>(-mouse.DeltaY()), { 0,1,0 });
	//	}
	//	else if (mouse.GetState(MouseButton::Left) == ButtonState::Held)
	//	{
	//		DollyInOut(static_cast<float>(mouse.DeltaY()));
	//		Pan(static_cast<float>(mouse.DeltaX()));

	//		if (key.GetState(Keycode::D) == ButtonState::Held)
	//			Track(camera_speed.x, right_vec);
	//		if (key.GetState(Keycode::A) == ButtonState::Held)
	//			Track(-camera_speed.x, right_vec);
	//		if (key.GetState(Keycode::W) == ButtonState::Held)
	//			Crane(camera_speed.y, current_up_vec);
	//		if (key.GetState(Keycode::S) == ButtonState::Held)
	//			Crane(-camera_speed.y, current_up_vec);
	//	}
	//	else if (mouse.GetState(MouseButton::Right) == ButtonState::Held)
	//	{
	//		Pan(static_cast<float>(mouse.DeltaX()));
	//		Tilt(static_cast<float>(mouse.DeltaY()));

	//		if (key.GetState(Keycode::D) == ButtonState::Held)
	//			Track(camera_speed.x, right_vec);
	//		if (key.GetState(Keycode::A) == ButtonState::Held)
	//			Track(-camera_speed.x, right_vec);
	//		if (key.GetState(Keycode::W) == ButtonState::Held)
	//			Crane(camera_speed.y, current_up_vec);
	//		if (key.GetState(Keycode::S) == ButtonState::Held)
	//			Crane(-camera_speed.y, current_up_vec);
	//	}
	//}

	void Camera::CalcCameraDirectionalVector()
	{
		front_vec		= DirectX::SimpleMath::Vector3{ focus_position - eye_position };
		camera_right	= arithmetic::CalcRightVec(current_up_vec, front_vec);
		current_up_vec	= arithmetic::CalcUpVec(front_vec, camera_right);

		front_vec.Normalize();
		camera_right.Normalize();
		current_camera_up.Normalize();
	}

	void Camera::CalcCameraAngle()
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

	void Camera::AdjustCameraAngle()
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

	void Camera::SetCBufferParam() const
	{
		auto& camera_data = cb_camera->GetData();
		camera_data.camera_view_matrix				  = view_mat;
		camera_data.camera_projection_matrix		  = projection_mat;
		camera_data.camera_view_projection_matrix	  = view_projection_mat;
		camera_data.camera_inv_view_matrix			  = view_mat.Invert();
		camera_data.camera_inv_projection_matrix	  = projection_mat.Invert();
		camera_data.camera_inv_view_projection_matrix = view_projection_mat.Invert();


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

	void Camera::SetCameraRight(const DirectX::SimpleMath::Vector3& right)
	{
		if (arithmetic::IsEqual(right.x, 0.0f) &&
			arithmetic::IsEqual(right.y, 0.0f) &&
			arithmetic::IsEqual(right.z, 0.0f))
			assert(!"Vector is zero");

		right_vec = right;
	}

	void Camera::SetCameraUp(const DirectX::SimpleMath::Vector3& up)
	{
		if (arithmetic::IsEqual(up.x, 0.0f) &&
			arithmetic::IsEqual(up.y, 0.0f) &&
			arithmetic::IsEqual(up.z, 0.0f))
			assert(!"Vector is zero");

		current_up_vec = up;
	}

	void Camera::SetCameraFront(const DirectX::SimpleMath::Vector3& front)
	{
		if (arithmetic::IsEqual(front.x, 0.0f) &&
			arithmetic::IsEqual(front.y, 0.0f) &&
			arithmetic::IsEqual(front.z, 0.0f))
			assert(!"Vector is zero");

		current_up_vec = front;
	}

	void Camera::Pan(float velocity)
	{// クォータニオン Ver
		DirectX::SimpleMath::Quaternion q{};
		q = DirectX::SimpleMath::Quaternion::Identity;
		q.Normalize();
		q = q.CreateFromAxisAngle(current_up_vec, DirectX::XMConvertToRadians(velocity * camera_speed.x * 0.1f));
		front_vec = DirectX::SimpleMath::Vector3::Transform(front_vec, q);
		front_vec.Normalize();

		focus_position = eye_position + front_vec;

		CalcCameraDirectionalVector();
	}

	void Camera::Tilt(const float velocity)
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

				if (const float rad = acosf(d);
					rad > DirectX::XMConvertToRadians(max_camera_angle.x))
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

	void Camera::DollyInOut(const float velocity)
	{
		const float v = -velocity * camera_speed.y;

		focus_position += front_vec * v;
		eye_position   += front_vec * v;
	}

	void Camera::Track(const float velocity, const DirectX::SimpleMath::Vector3& axis)
	{
		// カメラの注視点と位置を同じだけ移動
		focus_position	+= axis * velocity;
		eye_position	+= axis * velocity;
	}

	void Camera::Crane(const float velocity, const DirectX::SimpleMath::Vector3& axis)
	{
		// カメラの注視点と位置を同じだけ移動
		focus_position	+= axis * velocity;
		eye_position	+= axis * velocity;
	}
} // cumulonimbus::camera