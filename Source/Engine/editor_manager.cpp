#include "editor_manager.h"

#include "scene.h"

namespace cumulonimbus::editor
{
	EditorManager::EditorManager()
	{
		//-- �I�u�W�F�N�g�̃C���X�^���X�� --//
		hierarchy		 = std::make_unique<Hierarchy>();
		inspector		 = std::make_unique<Inspector>();
		menu_bar		 = std::make_unique<MenuBar>();
		project_view	 = std::make_unique<ProjectView>();
		scene_view		 = std::make_unique<SceneView>();
		game_view		 = std::make_unique<GameView>();
		system_inspector = std::make_unique<SystemInspector>();
		tool_bar		 = std::make_unique<ToolBar>();
	}

	void EditorManager::Update(const float dt)
	{
		scene_view->Update(dt);
		tool_bar->Update();
		system_inspector->Update(dt);
	}

	void EditorManager::RenderEditor(std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes)
	{
		if(!active_scenes.contains(selected_scene_id))
		{
			assert(!"Don't have scene id(EditorManager::RenderEditor)");
		}

		scene::Scene& selected_scene = *active_scenes.at(selected_scene_id);
		ecs::Registry& registry = *selected_scene.GetRegistry();

		hierarchy->Render(selected_scene_id, active_scenes, *project_view.get());
		inspector->Render(&registry, hierarchy->GetSelectedEntity());
		game_view->Render(&registry);
		scene_view->Render(&registry, project_view.get(), hierarchy.get()); // �h���b�O & �h���b�v�̊֌W�� scene_view��project_view�̏��Ԃ�ς��Ă͂Ȃ�Ȃ�
		project_view->Render(&registry);
		system_inspector->Render(&registry);
		tool_bar->Render(&registry);
		menu_bar->Render(selected_scene_id, active_scenes);
	}
} // cumulonimbus::editor

