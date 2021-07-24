#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "view.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

class CameraWork final
{
public:
	CameraWork(cumulonimbus::ecs::Registry* registry);
	~CameraWork() = default;

	void Update(float dt);
	void RenderImGui();

	/**
	 * @brief				: カメラの位置、注視点、アップベクトルのセット
	 * @param eye_position	: 位置
	 * @param target		: 注視点
	 * @param up_vec		: アップベクトル
	 */
	void SetCameraInfo(
		const DirectX::SimpleMath::Vector3& eye_position /* 位置 */,
		const DirectX::SimpleMath::Vector3& target       /* 注視点 */,
		const DirectX::SimpleMath::Vector3& up_vec       /* アップベクトル*/);
	void SetCameraInfo(const Camera& v);

	void SetPosition(const DirectX::SimpleMath::Vector3& eye_position);
	void SetTargetVec(const DirectX::SimpleMath::Vector3& target);
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up);
	void SetCameraUpRightFrontVector(const DirectX::SimpleMath::Vector3& up,
									 const DirectX::SimpleMath::Vector3& right,
									 const DirectX::SimpleMath::Vector3& front);

	void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { this->camera_velocity = speed; }

	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetPosition()		 const { return eye_position; }
	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetFocusPosition() const { return focus_position; }
	[[nodiscard]] const DirectX::SimpleMath::Vector3& GetCameraFront()	 const { return front_vec; }

	//-- オブジェアタッチ用関数 --//
	/**
	 * @brief						: オブジェクトカメラを使用する際の
	 *								  対象モデルのエンティティを指定
	 * @param ent					: 対象モデルのエンティティ
	 * @param switch_object_camera	: カメラ処理をオブジェクト用にするか(true : オブジェクト用カメラを使用)
	 */
	void AttachObject(cumulonimbus::mapping::rename_type::Entity ent, bool switch_object_camera = true);

	/**
	 * @brief				: オブジェクト用カメラのパラメータの初期化
	 *						  カメラの初期位置をオブジェクトの後方にセット
	 *						  (オブジェクトのフロントベクトル * -1) * camera_length
	 * @param camera_length : オブジェクトとカメラ間の長さ(デフォルト == 50)
	 */
	void InitializeObjectCameraParam(float camera_length = 50.0f);

private:
	std::unique_ptr<buffer::ConstantBuffer<CameraCB>> cb_camera;
	cumulonimbus::ecs::Registry* registry{ nullptr };

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

	//-- for debug --//
	DirectX::SimpleMath::Vector2 camera_velocity{ 3.f,3.f };

	//-- カメラとオブジェクトのアタッチ用変数 --//
	cumulonimbus::mapping::rename_type::Entity attach_entity;	// アタッチするオブジェクトのエンティティ
	bool is_use_camera_for_object = false; // オブジェクトアタッチ用フラグ(true : オブジェクトにアタッチされている)
	float camera_length; // オブジェクトとカメラの長さ


	/**
	 * @brief : オブジェクトアタッチ時の更新関数
	 *			is_use_camera_for_object == true時に実行される
	 */
	void UpdateObjectCamera(float dt);

	/**
	 * @brief : 非オブジェクトアタッチ時の更新関数
	 *			is_use_camera_for_object == false時に実行される
	 */
	void UpdateDefaultCamera(float dt);

	/**
	 * @brief   : 現在のカメラの正規直行ベクトルを算出
	 */
	void CalcCameraDirectionalVector();

	/**
	 * @brief : 左手座標軸を基準にしてのカメラの角度(Degree)計算
	 */
	void CalcCameraAngle();

	/**
	 * @brief			: カメラワーク時の動作
	 * @param velocity	: カメラスピード(デバックの設定で変更可能)
	 */
	void Pan(float velocity);								// カメラの左右の傾き(位置は固定)
	void Tilt(float velocity);								// カメラの上下の傾き(位置は固定)
	void DollyInOut(float velocity);						// カメラの前後の動き(向きは固定)
	void Track(float velocity,
		const DirectX::SimpleMath::Vector3& axis /*基準軸*/);								// カメラの左右移動(向きは固定)
	void Crane(float velocity,
			   const DirectX::SimpleMath::Vector3& axis /*基準軸*/);	// カメラの上下移動(向きは固定)
};
