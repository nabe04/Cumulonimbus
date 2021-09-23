#pragma once
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "scene_view_camera.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace editor
	{
		class ProjectView;
	} // editor
} // cumulonimbus

namespace cumulonimbus::editor
{
	class SceneView final
	{
	public:
		//	マウスのホバー状態
		enum class MouseHoverEvent
		{
			Begin_Hovered,  // ホバー開始時
			Hovering,		// ホバー中
			End_Hovered,	// ホバー終了時
			None,			// ウィンドウ外時

			End
		};

		struct MouseHover
		{
			bool is_old_hovered{ false };
			bool is_current_hovered{ false };
			MouseHoverEvent event{ MouseHoverEvent::None };

			void Update();
		};

	public:
		explicit SceneView();
		~SceneView() = default;

		void Update(float dt);
		void Render(ecs::Registry* registry, const ProjectView* project_view);

		/**
		 * @brief : SceneView内のマウス位置をWindow内の位置に変換
		 * @remark : SceneView外にマウスが存在する場合(-1,-1)がかえる
		 */
		[[nodiscard]]
		DirectX::XMINT2 ConvertWindowPos() const;

		/**
		 * @brief : Scene Viewウィンドウ内にカーソルがあるか
		 * @return : true -> カーソルがある
		 * @return : false -> カーソルがない
		 */
		[[nodiscard]]
		bool IsWindowHovered();

		[[nodiscard]]
		camera::SceneViewCamera& GetSceneViewCamera() const { return *scene_view_camera.get(); }
	private:
		std::unique_ptr<camera::SceneViewCamera> scene_view_camera{};
		MouseHover								 mouse_hover{};
		DirectX::SimpleMath::Vector2			 image_size{};
		DirectX::SimpleMath::Vector2			 window_offset{};
		DirectX::SimpleMath::Vector2			 window_mouse_pos{};
		DirectX::SimpleMath::Vector2			 window_pos{};
		DirectX::SimpleMath::Vector2			 window_size{}; // タイトルバーを除くウィンドウサイズ
		float									 title_bar_height{};
		mapping::rename_type::Entity			 dragging_entity{}; // 現在ドラッグされているエンティティ

		/**
		 * @brief : プロジェクトビューからドラッグしてきたモデルの追加
		 */
		void AddModel(ecs::Registry* registry, const std::filesystem::path& file_path);
		/**
		 * @brief : プロジェクトビューからドラッグしてきたモデルをマウスで配置中
		 */
		void DraggingModel(ecs::Registry* registry) const;
	};
} // cumulonimbus::editor
