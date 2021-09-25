#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "component_base.h"
#include "camera.h"
#include "rename_type_mapping.h"
#include "locator.h"

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

		void GameUpdate(float dt) override;
		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void serialize(Archive&& archive);

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

		[[nodiscard]]
		camera::Camera* GetCamera() const { return camera.get(); }
		[[nodiscard]]
		bool GetIsActive() const { return is_active; }
		[[nodiscard]]
		bool GetIsMainCamera() const { return is_main_camera; }
	private:
		std::shared_ptr<camera::Camera> camera{};

		//-- カメラとオブジェクトのアタッチ用変数 --//
		mapping::rename_type::Entity attach_entity;	// アタッチするオブジェクトのエンティティ
		float camera_length; // オブジェクトとカメラの長さ

		bool  is_use_camera_for_debug = true; // デバッグ用カメラを使用するか
		bool  is_active		 = true;  // カメラを描画対象に加えるか(RenderPath::Render関数内でtrueの場合のみ描画する)
		bool  is_main_camera = false; // バックバッファ用のカメラ(全てのcamera_componentの中でtrueになるのは常に一つだけ)

		/**
		 * @brief : オブジェクトアタッチ時の更新関数
		 *			is_use_camera_for_object == true時に実行される
		 */
		void UpdateObjectCamera(float dt);
	};

} // cumulonimbus::component


