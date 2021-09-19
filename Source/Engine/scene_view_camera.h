#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "frame_buffer.h"
#include "shader_interop_renderer.h"
#include "locator.h"

namespace cumulonimbus::camera
{
	class SceneViewCamera final
	{
	public:
		explicit SceneViewCamera(float width, float height);
		explicit SceneViewCamera() = default; // for cereal
		~SceneViewCamera() = default;

		template<class Archive>
		void serialize(Archive&& archive);

		void Update(float dt);
		void RenderImGui();
	private:
		std::unique_ptr<buffer::ConstantBuffer<CameraCB>>	cb_camera{ nullptr };
		std::unique_ptr<FrameBuffer>						off_screen{ nullptr };

		//-- カメラの基本パラメータ --//
		// カメラの最大角度(軸基準) (y,zは未定義)
		DirectX::SimpleMath::Vector3 max_camera_angle{ 85.0f,-1.f,-1.f }; //	カメラの最大角(xyzそれぞれ+-両方の角度を表す。パラメータが0以下の場合はカメラの角度制限なし)

		DirectX::SimpleMath::Vector3 eye_position{ 0.0f,0.0f,0.0f };	// カメラ位置
		DirectX::SimpleMath::Vector3 focus_position{ 0.0f,0.0f,1.0f };	// カメラ焦点位置
		DirectX::SimpleMath::Vector3 focus_offset{ 0.0f,0.0f,0.0f };	// カメラ焦点位置のオフセット値

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

		float near_z{};	// カメラの最近値
		float far_z{};	// カメラの最遠地
		float fov{};		// 視野角
		float aspect{};	// アスペクト比
		float width{};	// ビュー行列上の幅
		float height{};	// ビュー行列上の高さ

		//-- for debug --//
		DirectX::SimpleMath::Vector2 camera_speed{ 1.f,1.f };
		DirectX::SimpleMath::Vector2 cur_mouse_pos{ 0.0f,0.0f };		// Current mouse eye_position
		DirectX::SimpleMath::Vector2 prev_mouse_pos{ 0.0f,0.0f };		// Mouse eye_position one frame ago

		//-- カメラとオブジェクトのアタッチ用変数 --//
		float camera_length{}; // オブジェクトとカメラの長さ

		bool  is_use_camera_for_debug = true; // デバッグ用カメラを使用するか
		bool  is_perspective = true;  // 透視投影を使用するか
		bool  is_active		 = true;  // カメラを描画対象に加えるか(RenderPath::Render関数内でtrueの場合のみ描画する)
		bool  is_main_camera = false; // バックバッファ用のカメラ(全てのcamera_componentの中でtrueになるのは常に一つだけ)

		/**
		 * @brier : 直行ベクトル(right,up,front)の算出
		 */
		void CalcCameraOrthogonalVector();

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
} // cumulonimbus::camera