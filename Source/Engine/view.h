#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "shader_interop_renderer.h"

class CameraWork;

namespace cumulonimbus::ecs
{
	class Registry;
}

class View final
{
public:
	View(cumulonimbus::ecs::Registry* registry);
	~View();

	void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
	void UnbindCBuffer() const;

	void Update(float dt);

	void WriteImGui();

	void SetCameraPos(DirectX::SimpleMath::Vector3 eye_position, DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 up);
	void SetProjection(float fov, float aspect, float min, float max);
	void SetOrtho(float width, float height, float min, float max);
	void SetFocusPosition(const DirectX::SimpleMath::Vector3& target) const { cb_camera->data.camera_at= target; }

	void SetEyePosition(const DirectX::SimpleMath::Vector3& pos) const  { cb_camera->data.camera_position = pos; }

	[[nodiscard]] DirectX::SimpleMath::Matrix GetView()			const   { return cb_camera->data.camera_view_matrix; }
	[[nodiscard]] DirectX::SimpleMath::Matrix GetProjection()	const	{ return  cb_camera->data.camera_view_projection_matrix; }

	[[nodiscard]] DirectX::SimpleMath::Vector3 GetEyePosition()   const { return cb_camera->data.camera_position; }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetFocusPosition() const { return cb_camera->data.camera_at; }

	// ÉJÉÅÉâÇÃå¸Ç´éÊìæ
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraRight() const { return cb_camera->data.camera_right; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraUp()    const { return cb_camera->data.camera_up; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraFront() const { return cb_camera->data.camera_front; }

	void SetCameraRight(const DirectX::SimpleMath::Vector3& right) const { cb_camera->data.camera_right = right; }
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up)	   const { cb_camera->data.camera_up = up; }
	void SetCameraFront(const DirectX::SimpleMath::Vector3& front) const { cb_camera->data.camera_front = front; }
	void SetFov(const float fov) const { cb_camera->data.camera_fov = fov; }

	[[nodiscard]] float GetFov()    const { return cb_camera->data.camera_fov; }
	[[nodiscard]] float GetAspect() const { return cb_camera->data.camera_aspect; }
	[[nodiscard]] float GetNearZ()  const { return cb_camera->data.camera_near_z; }
	[[nodiscard]] float GetFarZ()   const { return cb_camera->data.camera_far_z; }


	void IsDebugCamera(const bool flg) { is_debug_camera = flg; }
	[[nodiscard]] bool IsDebugCamera() const { return is_debug_camera; }

private:
	std::unique_ptr<CameraWork> camera_work{};
	std::unique_ptr<buffer::ConstantBuffer<CameraCB>> cb_camera;

	//DirectX::SimpleMath::Vector3 eye_position{ 0.0f,0.0f,0.0f };	// à íu
	//DirectX::SimpleMath::Vector3 focus_position{ 0.0f,0.0f,0.0f };	// íçéãì_
	//DirectX::SimpleMath::Vector3 up_vec{ 0.0f,1.0f,0.0f };
	//DirectX::SimpleMath::Vector3 right_vec{ 1.0f,0.0f,0.0f };
	//DirectX::SimpleMath::Vector3 front_vec{ focus_position.x - eye_position.x,
	//										focus_position.y - eye_position.y,
	//										focus_position.z - eye_position.z };


	DirectX::SimpleMath::Vector3 view_up_vec{ 0.0f,1.0f,0.0f };
	//DirectX::SimpleMath::Matrix view_f4x4;
	//DirectX::SimpleMath::Matrix projection_f4x4;
	//float fov = 0.0f;
	//float aspect = 0.0f;
	//float near_z = 0.0f;
	//float far_z = 1.0f;
	//float width = 0;
	//float height = 0;
	bool  is_perspective = true;
	bool  is_debug_camera = false;

	//-- Camera controlled activate mouse eye_position --//
	DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse eye_position
	DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse eye_position one frame ago

	void CalcCameraOrthogonalVector();

};


