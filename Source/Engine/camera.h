#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "constant_buffer.h"
#include "frame_buffer.h"
#include "shader_interop_renderer.h"

namespace cumulonimbus::camera
{
	class Camera final
	{
	public:
		explicit Camera(float width, float height);
		explicit Camera() = default;
		Camera(const Camera& other);
		Camera& operator=(const Camera& other);
		~Camera() = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(max_camera_angle),

				CEREAL_NVP(eye_position),
				CEREAL_NVP(focus_position),
				CEREAL_NVP(focus_offset),

				CEREAL_NVP(up_vec),
				CEREAL_NVP(current_up_vec),
				CEREAL_NVP(right_vec),
				CEREAL_NVP(front_vec),

				CEREAL_NVP(current_camera_up),
				CEREAL_NVP(camera_right),

				CEREAL_NVP(view_mat),
				CEREAL_NVP(projection_mat),
				CEREAL_NVP(view_projection_mat),

				CEREAL_NVP(camera_angle),

				CEREAL_NVP(near_z),
				CEREAL_NVP(far_z),
				CEREAL_NVP(fov),
				CEREAL_NVP(aspect),
				CEREAL_NVP(width),
				CEREAL_NVP(height),

				CEREAL_NVP(camera_speed),
				CEREAL_NVP(cur_mouse_pos),
				CEREAL_NVP(prev_mouse_pos),

				CEREAL_NVP(is_perspective)
			);
		}

		std::unique_ptr<Camera> Clone() { return std::make_unique<Camera>(*this); }

		void Initialize();
		void Update(float dt);
		void RenderImGui();

		void ClearFrameBuffer(
			const DirectX::SimpleMath::Vector4& clear_color = { 0.0f,0.0f,0.0f,1.0f }) const;
		void BindFrameBuffer() const;
		void UnbindFrameBuffer() const;

		void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
		void UnbindCBuffer() const;

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
		 * @brief			: TPSカメラを想定してのカメラのピッチ(X軸)回転
		 * @param radian	: 回転分の角度(ラジアン角)
		 */
		void RotationTPSPitch(float radian);

		/**
		 * @brief			: TPSカメラを想定してのカメラのYaw(Y軸)回転
		 * @param radian	: 回転分の角度(ラジアン角)
		 */
		void RotationTPSYaw(float radian);

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
		void Pan(float velocity);			// カメラの左右の傾き(位置は固定)
		void Tilt(float velocity);			// カメラの上下の傾き(位置は固定)
		void DollyInOut(float velocity);	// カメラの前後の動き(向きは固定)
		void Track(float velocity,
			const DirectX::SimpleMath::Vector3& axis /*基準軸*/);								// カメラの左右移動(向きは固定)
		void Crane(float velocity,
			const DirectX::SimpleMath::Vector3& axis /*基準軸*/);	// カメラの上下移動(向きは固定)

		/**
		 * @brief				: カメラパラメータの設定(位置、注視点、カメラアップベクトル)
		 * @param pos			: カメラ位置
		 * @param target		: カメラ注視点
		 * @param camera_up		: カメラアップベクトル
		 */
		void SetViewInfo(
			const DirectX::SimpleMath::Vector3& pos,
			const DirectX::SimpleMath::Vector3& target,
			const DirectX::SimpleMath::Vector3& camera_up);
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
		* @brief : カメラ位置の設定
		*/
		void SetEyePosition(const DirectX::SimpleMath::Vector3& pos) { eye_position = pos; }
		/**
		 * @brief : カメラ注視点の設定
		 */
		void SetFocusPosition(const DirectX::SimpleMath::Vector3& target) { focus_position = target; }
		/**
		 * @brief : カメラ位置オフセット値の設定
		 */
		void SetFocusOffset(const DirectX::SimpleMath::Vector3& offset) { focus_offset = offset; }

		/**
		 * @brief : 視野角(fov)の設定
		 */
		void SetFov(const float fov) const { cb_camera->data.camera_fov = fov; }

		void SetCameraSpeed(const DirectX::SimpleMath::Vector2& speed) { camera_speed = speed; }

		[[nodiscard]]
		float GetFov()    const { return fov; }
		[[nodiscard]]
		float GetAspect() const { return aspect; }
		[[nodiscard]]
		float GetNearZ()  const { return near_z; }
		[[nodiscard]]
		float GetFarZ()   const { return far_z; }

		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetViewMat()			const { return view_mat; }
		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetProjectionMat()	const { return projection_mat; }

		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetEyePosition()   const { return eye_position; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetFocusPosition() const { return focus_position; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetFocusOffset()   const { return focus_offset; }

		// カメラの向き取得
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetCameraRight() const { return right_vec; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetCameraUp()    const { return up_vec; }
		[[nodiscard]]
		const DirectX::SimpleMath::Vector3& GetCameraFront() const { return front_vec; }

		[[nodiscard]]
		const DirectX::SimpleMath::Vector2& GetCameraSpeed() const { return camera_speed; }

		[[nodiscard]]
		ID3D11ShaderResourceView** GetFrameBufferSRV_Address() const { return off_screen->GetRenderTargetSRV(); }
	private:
		std::shared_ptr<buffer::ConstantBuffer<CameraCB>>	cb_camera{ nullptr };
		std::shared_ptr<FrameBuffer>						off_screen{ nullptr };

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
		bool  is_perspective = true;  // 透視投影を使用するか
	};
} // cumulonimbus::camera