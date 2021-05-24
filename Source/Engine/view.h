#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "input_manager.h"

class View final
{
private:
	DirectX::SimpleMath::Vector3 position{ 0.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 target { 0.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 camera_forcus{ target.x - position.x, target.y - position.y , target.z - position.z };
	DirectX::SimpleMath::Vector3 camera_up{ 0.0f,1.0f,0.0f };
	DirectX::SimpleMath::Vector3 camera_right{ 1.0f,0.0f,0.0f };

	// I might use it in the future
	DirectX::SimpleMath::Vector3 right{ 1.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 up{ 0.0f,1.0f,0.0f };
	DirectX::SimpleMath::Vector3 front{ 0.0f,0.0f,1.0f };

	DirectX::SimpleMath::Matrix view_f4x4;
	DirectX::SimpleMath::Matrix projection_f4x4;
	float fov		= 0.0f;
	float aspect	= 0.0f;
	float nearZ		= 0.0f;
	float farZ		= 1.0f;
	float width		= 0;
	float height	= 0;
	bool  is_perspective;
	bool  is_debug_camera = false;

	//-- Camera controlledactivate mouse position --//
	DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse position
	DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse position one frame ago

private:
	void CalcCameraOrthogonalVector();

public:
	View();
	virtual ~View();

	void Activate();
	void CameraControl();	// Camera control for debug

	void WriteImGui();

	void SetCameraPos(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 target, DirectX::SimpleMath::Vector3 up);
	void SetProjection(float fov, float aspect, float min, float max);
	void SetOrtho(float width, float height, float min, float max);
	void SetCameraTarget(const DirectX::SimpleMath::Vector3& target) { this->target = target; }

	void SetPosition(const DirectX::SimpleMath::Vector3& pos) { this->position = pos; }

	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetView() const { return view_f4x4; }
	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetProjection() const { return projection_f4x4; }

	[[nodiscard]] DirectX::SimpleMath::Vector3 GetPos() const { return position; }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetTarget() const { return target; }

	// Camera rotation ajustment (Camera position rotation from the point of gaze)
	void RotationAdjustWrapPitchYawRoll(float pitch, float yaw, float roll);
	void RotationAdjustWrapX(float x);
	void RotationAdjustWrapY(float y);
	void RotationAdjustWrapZ(float z);

	// ƒJƒƒ‰‚ÌŒü‚«Žæ“¾
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraRight() const { return camera_right; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraUp() const { return camera_up; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraFront() const { return camera_forcus; }

	void SetCameraRight(const DirectX::SimpleMath::Vector3& right) { camera_right = right; }
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up) { camera_up = up; }
	void SetCameraFront(const DirectX::SimpleMath::Vector3& front) { camera_forcus = front; }
	void SetFov(const float fov) { this->fov = fov; }

	[[nodiscard]] float GetFov() const { return fov; }
	[[nodiscard]] float GetAspect() const { return aspect; }
	[[nodiscard]] float GetNearZ() const { return nearZ; }
	[[nodiscard]] float GetFarZ() const { return farZ; }


	void IsDebugCamera(const bool flg) { is_debug_camera = flg; }
	bool IsDebugCamera() const { return is_debug_camera; }
};


