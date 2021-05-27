#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "view.h"

class CameraWork final
{
public:
	CameraWork(const View& v);
	~CameraWork() = default;

	void Update(bool is_debug);
	void RenderImGui();

#pragma region
	/*
	 * brief    : カメラの位置、注視点、アップベクトルのセット
	 */
	void SetCameraInfo(
		const DirectX::SimpleMath::Vector3& eye_position /* 位置 */,
		const DirectX::SimpleMath::Vector3& target       /* 注視点 */,
		const DirectX::SimpleMath::Vector3& up_vec       /* アップベクトル*/);
	void SetCameraInfo(const View& v);

	void SetPosition(const DirectX::SimpleMath::Vector3& eye_position);
	void SetTargetVec(const DirectX::SimpleMath::Vector3& target);
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up);
	void SetCameraUpRightFrontVector(const DirectX::SimpleMath::Vector3& up,
									 const DirectX::SimpleMath::Vector3& right,
									 const DirectX::SimpleMath::Vector3& front);

	void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { this->camera_velocity = speed; }

	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetPosition() const { return eye_position; }
	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetFocusPosition() const { return focus_position; }
	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetCameraFront() const { return front_vec; }
#pragma endregion Setter & Getter

private:
	// カメラの最大角度(軸基準) (y,zは未定義)
	const DirectX::SimpleMath::Vector3 max_camera_angle{ 85.0f,0,0 };

	DirectX::SimpleMath::Vector3 eye_position{ 0.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 focus_position{ 0.0f,0.0f,1.0f };

	DirectX::SimpleMath::Vector3 up_vec{ 0.0f,1.0f,0.0f };		// カメラのUpベクトルを実際にセットするときに使用するベクトル
	DirectX::SimpleMath::Vector3 right_vec{ 1.0f,0.0f,0.0f };
	DirectX::SimpleMath::Vector3 front_vec{ focus_position.x - eye_position.x,
											focus_position.y - eye_position.y,
											focus_position.z - eye_position.z };

	DirectX::SimpleMath::Vector3 current_camera_up{ .0f,1.f,.0f };		// カメラのそれぞれのベクトルが直行になるように補正したベクトル(計算などに使用)
	DirectX::SimpleMath::Vector3 camera_right{ 1.0f,0.0f,0.0f };

	DirectX::SimpleMath::Matrix view_f4x4{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix projection_f4x4{ DirectX::SimpleMath::Matrix::Identity };

	DirectX::SimpleMath::Vector3 camera_angle{}; // カメラの角度(Degree),左手系の座標軸からの角度

	// for debug
	DirectX::SimpleMath::Vector2 camera_velocity{ 3.f,3.f };

	/*
	 * brief    : カメラワーク時の動作
	 * velocity : カメラスピード(デバックの設定で変更可能)
	 */
	void Pan(float velocity);								// カメラの左右の傾き(位置は固定)
	void Tilt(float velocity);								// カメラの上下の傾き(位置は固定)
	void DollyInOut(float velocity);						// カメラの前後の動き(向きは固定)
	void Track(float velocity,
		const DirectX::SimpleMath::Vector3& axis /*基準軸*/);								// カメラの左右移動(向きは固定)
	void Crane(float velocity,
			   const DirectX::SimpleMath::Vector3& axis /*基準軸*/);	// カメラの上下移動(向きは固定)
	void CraneIn_Y_Up_Axis(float velocity);
	
	void CalcCameraDirectionalVector();
	void CalcCameraAngle();
};
