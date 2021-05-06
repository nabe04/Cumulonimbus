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

	const DirectX::SimpleMath::Matrix& GetView() const { return view_f4x4; }
	const DirectX::SimpleMath::Matrix& GetProjection() const { return projection_f4x4; }

	DirectX::SimpleMath::Vector3 GetPos() { return position; }
	DirectX::SimpleMath::Vector3 GetTarget() const { return target; }

	// Camera rotation ajustment (Camera position rotation from the point of gaze)
	void RotationAjustWrapPitchYawRoll(float pitch, float yaw, float roll);
	void RotationAjustWrapX(float x);
	void RotationAjustWrapY(float y);
	void RotationAjustWrapZ(float z);

	// ÉJÉÅÉâÇÃå¸Ç´éÊìæ
	DirectX::SimpleMath::Vector3 GetCameraRight() const { return camera_right; }
	DirectX::SimpleMath::Vector3 GetCameraUp() const { return camera_up; }
	DirectX::SimpleMath::Vector3 GetCameraFront() const { return camera_forcus; }

	void SetCameraRight(DirectX::SimpleMath::Vector3 right) { camera_right = right; }
	void SetCameraUp(DirectX::SimpleMath::Vector3 up) { camera_up = up; }
	void SetCameraFront(DirectX::SimpleMath::Vector3 front) { camera_forcus = front; }

	float GetNearZ() const { return nearZ; }
	float GetFarZ() const { return farZ; }

	void IsDebugCamera(bool flg) { is_debug_camera = flg; }
	bool IsDebugCamera()		 { return is_debug_camera; }
};


