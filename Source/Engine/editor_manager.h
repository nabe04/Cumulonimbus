#pragma once
#include <memory>

#include "content_browser.h"
#include "hierarchy.h"
#include "inspector.h"
#include "menu_bar.h"
#include "project_view.h"
#include "scene_view.h"
#include "game_view.h"
#include "system_inspector.h"
#include "tool_bar.h"

namespace cumulonimbus
{
	namespace scene
	{
		class Scene;
		class SceneManager;
	} // scene
} // cumulonimbus

namespace cumulonimbus::editor
{
	class EditorManager final
	{
	public:
		explicit EditorManager();
		~EditorManager() = default;

		void Update(float dt);
		/**
		 * @brief : Editor部分の描画
		 * @param active_scenes : 現在開いているシーン(SceneManagerのパラメータ)
		 */
		void RenderEditor(std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes);
		// Todo : 複数シーン用のRenderEditorを作成した際に消す
		void RenderEditor(scene::Scene* scene, ecs::Registry* registry) const;

		[[nodiscard]]
		Hierarchy& GetHierarchyView() const { return *hierarchy.get(); }
		[[nodiscard]]
		SceneView& GetSceneView() const { return *scene_view.get(); }
		[[nodiscard]]
		ProjectView& GetProjectView() const { return *project_view.get(); }
		[[nodiscard]]
		SystemInspector& GetSystemInspector() const { return *system_inspector.get(); }
		[[nodiscard]]
		ToolBar& GetToolBar() const { return *tool_bar.get(); }

		void SetSelectedSceneId(const mapping::rename_type::UUID& id) { selected_scene_id = id; }
	private:
		//-- 描画するGUI郡 --//
		std::unique_ptr<ContentBrowser>  content_browser{};
		std::unique_ptr<Hierarchy>		 hierarchy{};
		std::unique_ptr<Inspector>		 inspector{};
		std::unique_ptr<MenuBar>		 menu_bar{};
		std::unique_ptr<ProjectView>	 project_view{};
		std::unique_ptr<SceneView>		 scene_view{};
		std::unique_ptr<GameView>		 game_view{};
		std::unique_ptr<SystemInspector> system_inspector{};
		std::unique_ptr<ToolBar>		 tool_bar{};
		// 現在選択されているシーンのID
		mapping::rename_type::UUID selected_scene_id{};
		//scene::Scene* selected_scene{};
	};
} // cumulonimbus::editor