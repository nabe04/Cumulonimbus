#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

class CameraWork;

class View final
{
private:
	std::unique_ptr<CameraWork> camera_work{};

	DirectX::SimpleMath::Vector3 eye_position{ 0.0f,0.0f,0.0f };	// ˆÊ’u
	DirectX::SimpleMath::Vector3 focus_position { 0.0f,0.0f,0.0f };	// ’Ž‹“_
	DirectX::SimpleMath::Vector3 up_vec{ 0.0f,1.0f,0.0f };
	DirectX::SimpleMath::Vector3 right_vec{ 1.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 front_vec{ focus_position.x - eye_position.x,
											focus_position.y - eye_position.y,
											focus_position.z - eye_position.z };

	// I might use it in the future
	DirectX::SimpleMath::Vector3 right{ 1.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 up{ 0.0f,1.0f,0.0f };
	DirectX::SimpleMath::Vector3 front{ 0.0f,0.0f,1.0f };

	DirectX::SimpleMath::Matrix view_f4x4;
	DirectX::SimpleMath::Matrix projection_f4x4;
	float fov		= 0.0f;
	float aspect	= 0.0f;
	float near_z	= 0.0f;
	float far_z		= 1.0f;
	float width		= 0;
	float height	= 0;
	bool  is_perspective = true;
	bool  is_debug_camera = false;

	//-- Camera controlled activate mouse eye_position --//
	DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse eye_position
	DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse eye_position one frame ago

	void CalcCameraOrthogonalVector();

public:
	View();
	~View();

	void Activate();
	void CameraControl();	// Camera control for debug

	void WriteImGui();

	void SetCameraPos(DirectX::SimpleMath::Vector3 eye_position, DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 up);
	void SetProjection(float fov, float aspect, float min, float max);
	void SetOrtho(float width, float height, float min, float max);
	void SetFocusPosition(const DirectX::SimpleMath::Vector3& target) { this->focus_position = target; }

	void SetEyePosition(const DirectX::SimpleMath::Vector3& pos) { this->eye_position = pos; }

	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetView() const { return view_f4x4; }
	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetProjection() const { return projection_f4x4; }

	[[nodiscard]] DirectX::SimpleMath::Vector3 GetEyePosition()   const { return eye_position; }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetFocusPosition() const { return focus_position; }

	// Camera rotation adjustment (Camera eye_position rotation from the point of gaze)
	void RotationAdjustWrapPitchYawRoll(float pitch, float yaw, float roll);
	void RotationAdjustWrapX(float x);
	void RotationAdjustWrapY(float y);
	void RotationAdjustWrapZ(float z);

	// ƒJƒƒ‰‚ÌŒü‚«Žæ“¾
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraRight() const { return right_vec; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraUp()    const { return up_vec; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraFront() const { return front_vec; }

	void SetCameraRight(const DirectX::SimpleMath::Vector3& right) { right_vec = right; }
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up) { up_vec = up; }
	void SetCameraFront(const DirectX::SimpleMath::Vector3& front) { front_vec = front; }
	void SetFov(const float fov) { this->fov = fov; }

	[[nodiscard]] float GetFov()    const { return fov; }
	[[nodiscard]] float GetAspect() const { return aspect; }
	[[nodiscard]] float GetNearZ()  const { return near_z; }
	[[nodiscard]] float GetFarZ()   const { return far_z; }


	void IsDebugCamera(const bool flg) { is_debug_camera = flg; }
	[[nodiscard]] bool IsDebugCamera() const { return is_debug_camera; }
};


