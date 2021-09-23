#pragma once
#include <memory>

#include "content_browser.h"
#include "hierarchy.h"
#include "inspector.h"
#include "menu_bar.h"
#include "project_view.h"
#include "scene_view.h"
#include "system_inspector.h"
#include "tool_bar.h"

namespace cumulonimbus
{
	namespace scene
	{
		class Scene;
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
		void RenderEditor(scene::Scene* scene, ecs::Registry* registry) const;

		[[nodiscard]]
		SceneView& GetSceneView() const { return *scene_view.get(); }
		[[nodiscard]]
		ProjectView& GetProjectView() const { return *project_view.get(); }
	private:
		std::unique_ptr<ContentBrowser>  content_browser{};
		std::unique_ptr<Hierarchy>		 hierarchy{};
		std::unique_ptr<Inspector>		 inspector{};
		std::unique_ptr<MenuBar>		 menu_bar{};
		std::unique_ptr<ProjectView>	 project_view{};
		std::unique_ptr<SceneView>		 scene_view{};
		std::unique_ptr<SystemInspector> system_inspector{};
		std::unique_ptr<ToolBar>		 tool_bar{};
	};
} // cumulonimbus::editor