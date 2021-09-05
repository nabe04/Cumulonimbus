#include "editor_manager.h"

#include "scene.h"

namespace cumulonimbus::editor
{
	EditorManager::EditorManager()
	{
		content_browser = std::make_unique<ContentBrowser>();
		hierarchy		= std::make_unique<Hierarchy>();
		inspector		= std::make_unique<Inspector>();
	}

	void EditorManager::RenderEditor(Scene* scene, ecs::Registry* registry) const
	{
		content_browser->Render(scene);
		hierarchy->Render(registry);
		inspector->Render(registry, hierarchy->GetSelectedEntity());
	}
} // cumulonimbus::editor

