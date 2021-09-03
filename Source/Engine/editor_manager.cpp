#include "editor_manager.h"

#include "scene.h"
#include "gaussian_blur.h"

namespace cumulonimbus::editor
{
	EditorManager::EditorManager()
	{
		content_browser = std::make_unique<ContentBrowser>();
		hierarchy		= std::make_unique<Hierarchy>();
	}

	void EditorManager::RenderEditor(Scene* scene, ecs::Registry* registry) const
	{
		content_browser->Render(scene);
		hierarchy->Render(registry);
	}
} // cumulonimbus::editor

