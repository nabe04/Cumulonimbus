#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "shader_interop_renderer.h"
#include "rename_type_mapping.h"

class CameraWork;

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

class View final
{
public:
	explicit View(cumulonimbus::ecs::Registry* registry);
	~View();

	void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
	void UnbindCBuffer() const;

	void Update(float dt);
	void WriteImGui();

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

	void SetViewInfo(
		DirectX::SimpleMath::Vector3 eye_position,
		DirectX::SimpleMath::Vector3 target,
		DirectX::SimpleMath::Vector3 camera_up);
	void SetProjection(float fov, float aspect, float min, float max);
	void SetOrtho(float width, float height, float min, float max);
	void SetFocusPosition(const DirectX::SimpleMath::Vector3& target)  { focus_position = target; }

	void SetEyePosition(const DirectX::SimpleMath::Vector3& pos)       { eye_position = pos; }
	void SetTargetVec(const DirectX::SimpleMath::Vector3& target);
	void SetCameraUpRightFrontVector(
		const DirectX::SimpleMath::Vector3& up,
		const DirectX::SimpleMath::Vector3& right,
		const DirectX::SimpleMath::Vector3& front);

	void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { this->camera_velocity = speed; }

	[[nodiscard]] DirectX::SimpleMath::Matrix GetViewMat()			const   { return view_mat; }
	[[nodiscard]] DirectX::SimpleMath::Matrix GetProjectionMat()	const	{ return projection_mat; }

	[[nodiscard]] DirectX::SimpleMath::Vector3 GetEyePosition()   const { return eye_position; }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetFocusPosition() const { return focus_position; }

	// カメラの向き取得
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraRight() const { return right_vec; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraUp()    const { return up_vec; }
	[[nodiscard]]DirectX::SimpleMath::Vector3 GetCameraFront() const { return front_vec; }

	void SetCameraRight(const DirectX::SimpleMath::Vector3& right);
	void SetCameraUp(const DirectX::SimpleMath::Vector3& up);
	void SetCameraFront(const DirectX::SimpleMath::Vector3& front);
	void SetFov(const float fov) const { cb_camera->data.camera_fov = fov; }

	[[nodiscard]] float GetFov()    const { return fov; }
	[[nodiscard]] float GetAspect() const { return aspect; }
	[[nodiscard]] float GetNearZ()  const { return near_z; }
	[[nodiscard]] float GetFarZ()   const { return far_z; }

	void IsDebugCamera(const bool flg) { is_debug_camera = flg; }
	[[nodiscard]] bool IsDebugCamera() const { return is_debug_camera; }

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

	DirectX::SimpleMath::Matrix view_mat			{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix projection_mat		{ DirectX::SimpleMath::Matrix::Identity };
	DirectX::SimpleMath::Matrix view_projection_mat	{ DirectX::SimpleMath::Matrix::Identity };

	DirectX::SimpleMath::Vector3 camera_angle{}; // カメラの角度(Degree),左手系の座標軸からの角度

	float near_z;	// カメラの最近値
	float far_z;	// カメラの最遠地
	float fov;		// 視野角
	float aspect;	// アスペクト比
	float width;	// ビュー行列上の幅
	float height;	// ビュー行列上の高さ

	//-- for debug --//
	DirectX::SimpleMath::Vector2 camera_velocity{ 3.f,3.f };

	//-- カメラとオブジェクトのアタッチ用変数 --//
	cumulonimbus::mapping::rename_type::Entity attach_entity;	// アタッチするオブジェクトのエンティティ
	bool is_use_camera_for_object = false; // オブジェクトアタッチ用フラグ(true : オブジェクトにアタッチされている)
	float camera_length; // オブジェクトとカメラの長さ

	DirectX::SimpleMath::Vector3 view_up_vec{ 0.0f,1.0f,0.0f };
	bool  is_perspective  = true;
	bool  is_debug_camera = false;

	//-- Camera controlled activate mouse eye_position --//
	DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse eye_position
	DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse eye_position one frame ago

	/**
	 * @brier : 直行ベクトル(right,up,front)の算出
	 */
	void CalcCameraOrthogonalVector();

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
	 * @brief : コンスタントバッファパラメータのセット
	 *			(Update関数の最後で行う)
	 */
	void SetCBufferParam() const;

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


