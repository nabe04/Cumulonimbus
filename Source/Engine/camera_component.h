#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"
#include "constant_buffer.h"
#include "frame_buffer.h"
#include "shader_interop_renderer.h"
#include "rename_type_mapping.h"
#include "locator.h"

class FrameBuffer;

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::component
{
	class CameraComponent final : public ComponentBase
	{
	public:
		/**
		 * @param is_main_camera	: バックバッファ用カメラに設定するか(デフォルトはtrue)
		 * @param width				: ビュー行列時の幅(デフォルトはスクリーンの幅)
		 * @param height			: ビュー行列時の高さ(デフォルトはスクリーンの高さ)
		 */
		explicit CameraComponent(
			ecs::Registry* registry, mapping::rename_type::Entity ent,
			bool is_main_camera = true,
			float width  = locator::Locator::GetWindow()->Width(),
			float height = locator::Locator::GetWindow()->Height());
		explicit CameraComponent()  = default; // for cereal
 		~CameraComponent() override = default;

		void Update(float dt) override;
		void RenderImGui() override;

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		void ClearFrameBuffer(DirectX::SimpleMath::Vector4 clear_color = { 0.0f,0.0f,0.0f,1.0f }) const;
		void BindFrameBuffer() const;
		void UnbindFrameBuffer() const;

		void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
		void UnbindCBuffer() const;

		/**
		 * @brief						: オブジェクトカメラを使用する際の
		 *								  対象モデルのエンティティを指定
		 * @param ent					: 対象モデルのエンティティ
		 * @attention					: is_use_camera_for_debug(デバッグ用カメラ)フラグが"false"になる
		 */
		void AttachObject(cumulonimbus::mapping::rename_type::Entity ent);

		/**
		 * @brief				: オブジェクト用カメラのパラメータの初期化
		 *						  カメラの初期位置をオブジェクトの後方にセット
		 *						  (オブジェクトのフロントベクトル * -1) * camera_length
		 * @param camera_length : オブジェクトとカメラ間の長さ(デフォルト == 50)
		 */
		void InitializeObjectCameraParam(float camera_length = 50.0f);

		/**
		 * @brief		: バックバッファ用のカメラに切り替える
		 * @attention	: 現クラスのCameraComponent::is_main_cameraをtrueに
		 *				  現クラス以外の全てのCameraComponent::is_main_cameraをfalseにする
		 */
		void SwitchMainCamera();

		/**
		 * @brief		 : クォータニオンを使用してright_vectorを基準に
		 *			       front_vectorを回転
		 * @param radian : 回転分の角度(ラジアン角)
		 */
		void RotationFrontVectorFromRightVector(float radian);
		/**
		 * @brief		 : クォータニオンを使用してup_vectorを基準に
		 *			       front_vectorを回転
		 * @param radian : 回転分の角度(ラジアン角)
		 */
		void RotationFrontVectorFromUpVector(float radian);

		/**
		 * @brief				: カメラパラメータの設定(位置、注視点、カメラアップベクトル)
		 * @param eye_position	: カメラ位置
		 * @param target		: カメラ注視点
		 * @param camera_up		: カメラアップベクトル
		 */
		void SetViewInfo(
			DirectX::SimpleMath::Vector3 eye_position,
			DirectX::SimpleMath::Vector3 target,
			DirectX::SimpleMath::Vector3 camera_up);
		/**
		 * @brief        : 透視投影に必要なパラメータの設定
		 * @param fov    : 視野角
		 * @param aspect : アスペクト比
		 * @param min    : 最近値
		 * @param max    : 最遠地
		 */
		void SetProjection(float fov, float aspect, float min, float max);
		/**
		 * @brief        : 平行投影に必要なパラメータの設定
		 * @param width  : カメラ幅
		 * @param height : カメラ高さ
		 * @param min    : 最近値
		 * @param max    : 最遠地
		 */
		void SetOrtho(float width, float height, float min, float max);

		/**
		 * @brief : カメラ注視点の設定
		 */
		void SetFocusPosition(const DirectX::SimpleMath::Vector3& target) { focus_position = target; }
		/**
		 * @brief : カメラ位置の設定
		 */
		void SetEyePosition(const DirectX::SimpleMath::Vector3& pos) { eye_position = pos; }
		/**
		 * @brief		: デバッグ時のカメラ速度の設定
		 * @pram speed	: カメラの速さ
		 */
		void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { this->camera_speed = speed; }
		/**
		 * @brief : 視野角(fov)の設定
		 */
		void SetFov(const float fov) const { cb_camera->data.camera_fov = fov; }

		void SetIsDebugCamera(const bool flg) { is_use_camera_for_debug = flg; }
		void SetIsActive(const bool flg) { is_active = flg; }

		[[nodiscard]] float GetFov()    const { return fov; }
		[[nodiscard]] float GetAspect() const { return aspect; }
		[[nodiscard]] float GetNearZ()  const { return near_z; }
		[[nodiscard]] float GetFarZ()   const { return far_z; }

		[[nodiscard]] DirectX::SimpleMath::Matrix GetViewMat()			const { return view_mat; }
		[[nodiscard]] DirectX::SimpleMath::Matrix GetProjectionMat()	const { return projection_mat; }

		[[nodiscard]] DirectX::SimpleMath::Vector3 GetEyePosition()   const { return eye_position; }
		[[nodiscard]] DirectX::SimpleMath::Vector3 GetFocusPosition() const { return focus_position; }

		// カメラの向き取得
		[[nodiscard]] DirectX::SimpleMath::Vector3 GetCameraRight() const { return right_vec; }
		[[nodiscard]] DirectX::SimpleMath::Vector3 GetCameraUp()    const { return up_vec; }
		[[nodiscard]] DirectX::SimpleMath::Vector3 GetCameraFront() const { return front_vec; }

		[[nodiscard]] DirectX::SimpleMath::Vector2 GetCameraSpeed() const { return camera_speed; }

		[[nodiscard]] bool GetIsActive() const { return is_active; }
		[[nodiscard]] bool GetIsMainCamera() const { return is_main_camera; }

		[[nodiscard]] ID3D11ShaderResourceView** GetFrameBufferSRV_Address() const { return off_screen->GetRenderTargetSRV(); }

	private:
		std::shared_ptr<buffer::ConstantBuffer<CameraCB>>	cb_camera{ nullptr };
		std::shared_ptr<FrameBuffer>						off_screen{ nullptr };

		//-- カメラの基本パラメータ --//
		// カメラの最大角度(軸基準) (y,zは未定義)
		DirectX::SimpleMath::Vector3 max_camera_angle{ 85.0f,-1.f,-1.f }; //	カメラの最大角(xyzそれぞれ+-両方の角度を表す。パラメータが0以下の場合はカメラの角度制限なし)

		DirectX::SimpleMath::Vector3 eye_position{ 0.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3 focus_position{ 0.0f,0.0f,1.0f };

		DirectX::SimpleMath::Vector3 up_vec{ 0.0f,1.0f,0.0f };			// view変換行列に渡すときに使用するupベクトル(固定)
		DirectX::SimpleMath::Vector3 current_up_vec{ 0.0f,1.0f,0.0f };	// カメラの現在のupベクトル
		DirectX::SimpleMath::Vector3 right_vec{ 1.0f,0.0f,0.0f };
		DirectX::SimpleMath::Vector3 front_vec{ focus_position.x - eye_position.x,
												focus_position.y - eye_position.y,
												focus_position.z - eye_position.z };

		DirectX::SimpleMath::Vector3 current_camera_up{ .0f,1.f,.0f };		// カメラのそれぞれのベクトルが直行になるように補正したベクトル(計算などに使用)
		DirectX::SimpleMath::Vector3 camera_right{ 1.0f,0.0f,0.0f };

		DirectX::SimpleMath::Matrix view_mat{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix projection_mat{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix view_projection_mat{ DirectX::SimpleMath::Matrix::Identity };

		DirectX::SimpleMath::Vector3 camera_angle{}; // カメラの角度(Degree),左手系の座標軸からの角度

		float near_z;	// カメラの最近値
		float far_z;	// カメラの最遠地
		float fov;		// 視野角
		float aspect;	// アスペクト比
		float width;	// ビュー行列上の幅
		float height;	// ビュー行列上の高さ

		//-- for debug --//
		DirectX::SimpleMath::Vector2 camera_speed{ 3.f,3.f };
		DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse eye_position
		DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse eye_position one frame ago

		//-- カメラとオブジェクトのアタッチ用変数 --//
		mapping::rename_type::Entity attach_entity;	// アタッチするオブジェクトのエンティティ
		float camera_length; // オブジェクトとカメラの長さ

		bool  is_use_camera_for_debug = true; // デバッグ用カメラを使用するか
		bool  is_perspective = true;  // 透視投影を使用するか
		bool  is_active		 = true;  // カメラを描画対象に加えるか(RenderPath::Render関数内でtrueの場合のみ描画する)
		bool  is_main_camera = false; // バックバッファ用のカメラ(全てのcamera_componentの中でtrueになるのは常に一つだけ)

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
		 * @brief : カメラの角度調整
		 *			カメラの最大角をもとにカメラ前方ベクトル(front_vec)を調整
		 */
		void AdjustCameraAngle();

		/**
		 * @brief : コンスタントバッファパラメータのセット
		 *			(Update関数の最後で行う)
		 */
		void SetCBufferParam() const;

		void SetCameraRight(const DirectX::SimpleMath::Vector3& right);
		void SetCameraUp(const DirectX::SimpleMath::Vector3& up);
		void SetCameraFront(const DirectX::SimpleMath::Vector3& front);

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

} // cumulonimbus::component


