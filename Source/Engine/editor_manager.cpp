#include "editor_manager.h"

#include "scene.h"
#include "component_list.h"

EditorManager::EditorManager()
{
	content_brawser = std::make_unique<ContentBrawser>();
}

void EditorManager::RenderEditor(Scene* scene)
{

	content_brawser->Render(scene);
}