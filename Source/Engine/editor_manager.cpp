#include "editor_manager.h"

#include "scene.h"

namespace cumulonimbus::editor
{
	EditorManager::EditorManager()
	{
		content_browser  = std::make_unique<ContentBrowser>();
		hierarchy		 = std::make_unique<Hierarchy>();
		inspector		 = std::make_unique<Inspector>();
		menu_bar		 = std::make_unique<MenuBar>();
		project_view	 = std::make_unique<ProjectView>();
		scene_view		 = std::make_unique<SceneView>();
		system_inspector = std::make_unique<SystemInspector>();
		tool_bar		 = std::make_unique<ToolBar>();
	}

	void EditorManager::Update(const float dt)
	{
		scene_view->Update(dt);
	}

	void EditorManager::RenderEditor(scene::Scene* scene, ecs::Registry* registry) const
	{
		content_browser->Render(scene);
		hierarchy->Render(registry);
		inspector->Render(registry, hierarchy->GetSelectedEntity());
		menu_bar->Render(registry);
		scene_view->Render(registry, project_view.get()); // ドラッグ & ドロップの関係上 scene_viewとproject_viewの順番を変えてはならない
		project_view->Render(registry);
		system_inspector->Render(registry);
		tool_bar->Render(registry);
	}
} // cumulonimbus::editor

