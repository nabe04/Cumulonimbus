#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

class CameraWork final
{
public:
	CameraWork();
	~CameraWork() = default;

	void Update(bool is_debug);
	[[noreturn]] void RenderImGui();

#pragma region
	/*
	 * brief    : カメラの位置、注視点、アップベクトルのセット
	 * position : 位置
	 * target   : 注視点
	 * up_vec   : アップベクトル
	 */
	[[noreturn]] void SetCameraInfo(
		const DirectX::SimpleMath::Vector3& position,
		const DirectX::SimpleMath::Vector3& target,
		const DirectX::SimpleMath::Vector3& up_vec)
	{
		this->position	 = position;
		this->target_vec = target;
		this->camera_up  = up_vec;
	}

	[[noreturn]] void SetPosition(const DirectX::SimpleMath::Vector3& position);
	[[noreturn]] void SetTargetVec(const DirectX::SimpleMath::Vector3& target);
	[[noreturn]] void SetCameraUp(const DirectX::SimpleMath::Vector3& up);

	[[noreturn]] void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { this->camera_speed = speed; }

	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetPositon() const { return position; }
	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetTargetVec() const { return target_vec; }
	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetCameraFront() const { return camera_front; }
#pragma endregion Setter & Getter

private:
	DirectX::SimpleMath::Vector3 position{ 0.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 target_vec{ 0.0f,0.0f,1.0f };
	DirectX::SimpleMath::Vector3 camera_front{ target_vec.x - position.x, target_vec.y - position.y , target_vec.z - position.z };
	DirectX::SimpleMath::Vector3 camera_up{ 0.0f,1.0f,0.0f };			// カメラのUpベクトルを実際にセットするときに使用するベクトル
	DirectX::SimpleMath::Vector3 current_camerea_up{ .0f,1.f,.0f };		// カメラのそれぞれのベクトルが直行になるように補正したベクトル(計算などに使用)
	DirectX::SimpleMath::Vector3 camera_right{ 1.0f,0.0f,0.0f };

	// I might use it in the future
	DirectX::SimpleMath::Vector3 right{ 1.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 up{ 0.0f,1.0f,0.0f };
	DirectX::SimpleMath::Vector3 front{ 0.0f,0.0f,1.0f };

	DirectX::SimpleMath::Matrix view_f4x4;
	DirectX::SimpleMath::Matrix projection_f4x4;

	DirectX::SimpleMath::Vector3 camera_angle{}; // カメラの角度(Degree),左手系の座標軸からの角度

	DirectX::SimpleMath::Vector3 test_angle{};

	// for debug
	DirectX::SimpleMath::Vector2 camera_speed{ 1.f,1.f };
	[[noreturn]] void Pan(const float velocity);
	[[noreturn]] void Tilt(const float velocity);
	[[noreturn]] void DollyInOut(const float velocity);
	[[noreturn]] void Track(const DirectX::SimpleMath::Vector3& direction, const float velocity);

	[[noreturn]] void PanAndTilt(const float velocity, const DirectX::SimpleMath::Vector3& add_angle);

	[[noreturn]] void CalcCameraDirectionalVector();
	[[noreturn]] void CalcCameraAngle();
};
