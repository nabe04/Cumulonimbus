#include "menu_bar.h"

#include <filesystem>
#include <Windows.h>

#include <portable-file-dialogs.h>
#include <ranges>

#include "scene.h"
#include "ecs.h"
#include "locator.h"
#include "scene_loader.h"
#include "generic.h"
#include "filename_helper.h"
#include "scene_manager.h"

namespace
{
	const std::string popup_new_scene{ "Create New Scene" }; // �uNew Scene�v���I�����ꂽ����ImGui::Popup ID
}

namespace cumulonimbus::editor
{
	void MenuBar::Render(
		mapping::rename_type::UUID& selected_scene_id,
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes)
	{
		// �|�b�v�A�b�v�p�t���O�̃��Z�b�g
		is_create_new_scene = false;

		//-- �E�B���h�E�ݒ� --//
		ImGuiWindowFlags window_flags = 0;
		if (no_titlebar)   window_flags |= ImGuiWindowFlags_NoTitleBar;
		if (no_move)       window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_MenuBar;

		//-- ���j���[�o�[ --//
		if (ImGui::Begin("Menu Bar", nullptr, window_flags))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					FileMenu(selected_scene_id, active_scenes);
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Window"))
				{
					if (ImGui::MenuItem("Menu Setting"))
						is_menu_open = true;

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Help"))
				{

					ImGui::EndMenu();;
				}

				ImGui::EndMenuBar();
			}

			scene::SceneManager& scene_manager	= *active_scenes.at(selected_scene_id)->GetSceneManager();
			ecs::Registry& registry				= *active_scenes.at(selected_scene_id)->GetRegistry();
			PopupCreateScene(selected_scene_id, registry, scene_manager);

		}
		ImGui::End();

		if (is_menu_open)
			MenuSetting();
	}

	// Todo : �����V�[���Ή���폜�\��
	void MenuBar::Render(ecs::Registry* registry)
	{
		//// �|�b�v�A�b�v�p�t���O�̃��Z�b�g
		//is_create_new_scene = false;

		////-- �E�B���h�E�ݒ� --//
		//ImGuiWindowFlags window_flags = 0;
  //      if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
  //      if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
		//window_flags |= ImGuiWindowFlags_MenuBar;

		////-- ���j���[�o�[ --//
		//if (ImGui::Begin("Menu Bar", nullptr, window_flags))
		//{
		//	if (ImGui::BeginMenuBar())
		//	{
		//		if (ImGui::BeginMenu("File"))
		//		{
		//			FileMenu(registry);
		//			ImGui::EndMenu();
		//		}

		//		if (ImGui::BeginMenu("Window"))
		//		{
		//			if (ImGui::MenuItem("Menu Setting"))
		//				is_menu_open = true;

		//			ImGui::EndMenu();
		//		}

		//		if (ImGui::BeginMenu("Help"))
		//		{

		//			ImGui::EndMenu();;
		//		}

		//		ImGui::EndMenuBar();
		//	}

		//	PopupCreateScene(registry);

		//}
		//ImGui::End();

		//if (is_menu_open)
		//	MenuSetting();
	}

	void MenuBar::FileMenu(
		const mapping::rename_type::UUID& selected_scene_id,
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes)
	{
		if (ImGui::MenuItem("New Scene", "Ctrl+Shift"))
		{
			is_create_new_scene = true;
		}

		if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
		{
			const auto selection = pfd::open_file(
				"Open Scene",
				"",
				{ "Scene Files","*.scene" },
				pfd::opt::none).result();

			if (selection.empty())
				return;
			const std::filesystem::path path = selection.at(0);
			const std::string relative_path  = utility::ConvertAbsolutePathToRelativePath(path.string());
			scene::SceneManager& scene_manager = *active_scenes.begin()->second->GetSceneManager();
			locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->OpenScene(scene_manager, relative_path);
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Save", "Ctrl+S"))
		{
			for(const auto& [scene_id, scene] : active_scenes)
			{
				// �A�Z�b�g�V�[�g�ɕۑ��Ώۂ�ID���܂܂�Ă��Ȃ��ꍇ
				if (!locator::Locator::GetAssetManager()->GetAssetSheetManager().GetSheet<asset::SceneAsset>().sheet.contains(scene_id))
				{// �V�[���𖼑O��t���ĕۑ�
					const auto destination = pfd::save_file("Save", "", {}, pfd::opt::none).result();
					if (destination.empty())
						continue;

					const std::filesystem::path path = destination;
					const std::string relative_path  = utility::ConvertAbsolutePathToRelativePath(path.string());
					locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->SaveAs(*scene.get(), relative_path);
				}
				else
				{// ���݂̃V�[���p�X�ŕۑ�
					locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->Save(active_scenes, scene_id);
				}
			}
		}
		if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
		{
			const auto destination = pfd::save_file("Save", "", {}, pfd::opt::none).result();
			if (destination.empty())
				return;

			const std::filesystem::path path = destination;
			const std::string relative_path  = utility::ConvertAbsolutePathToRelativePath(path.string());
			scene::Scene& save_scene = *active_scenes.at(selected_scene_id).get();
			// SceneManager�Ή��łɕύX
			locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->SaveAs(save_scene, relative_path);
		}
	}

	void MenuBar::FileMenu(ecs::Registry* registry)
	{
		//if(ImGui::MenuItem("New Scene","Ctrl+Shift"))
		//{
		//	is_create_new_scene = true;
		//}

		//if(ImGui::MenuItem("Open Scene","Ctrl+O"))
		//{
		//	const auto selection = pfd::open_file(
		//		"Open Scene",
		//		"",
		//		{ "Scene Files","*.scene" },
		//		pfd::opt::none).result();

		//	if (selection.empty())
		//		return;
		//	const std::filesystem::path path = selection.at(0);
		//	const std::string relative_path  = utility::ConvertAbsolutePathToRelativePath(path.string());
		//	locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->OpenScene(*registry->GetScene(), relative_path);
		//}
		//ImGui::Separator();
		//if(ImGui::MenuItem("Save","Ctrl+S"))
		//{
		//	// �V�[�������uNoTitled�v(�V�[�����ۑ�����Ă��Ȃ�)�̏ꍇ
		//	if(const std::string& current_scene_path = locator::Locator::GetSystem()->GetCurrentScenePath();
		//	   current_scene_path == filename_helper::GetNoTitled())
		//	{// �V�[���𖼑O��t���ĕۑ�
		//		const auto destination = pfd::save_file("Save", "", {}, pfd::opt::none).result();
		//		if (destination.empty())
		//			return;

		//		const std::filesystem::path path = destination;
		//		const std::string relative_path  = utility::ConvertAbsolutePathToRelativePath(path.string());
		//		locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->SaveAs(*registry->GetScene(), relative_path);
		//	}
		//	else
		//	{// ���݂̃V�[���p�X�ŕۑ�
		//		locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->Save(*registry->GetScene(), current_scene_path);
		//	}
		//}
		//if(ImGui::MenuItem("Save As...","Ctrl+Shift+S"))
		//{
		//	const auto destination = pfd::save_file("Save", "", {}, pfd::opt::none).result();
		//	if (destination.empty())
		//		return;

		//	const std::filesystem::path path = destination;
		//	const std::string relative_path  = utility::ConvertAbsolutePathToRelativePath(path.string());
		//	locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->SaveAs(*registry->GetScene(), relative_path);
		//}
	}

	void MenuBar::MenuSetting()
	{
		if (ImGui::Begin("Menu Setting", &is_menu_open))
		{
			if (ImGui::BeginTable("table", 1))
			{
				ImGui::TableNextColumn(); ImGui::Checkbox("No titlebar", &no_titlebar);
				ImGui::TableNextColumn(); ImGui::Checkbox("No move", &no_move);
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}

	void MenuBar::PopupCreateScene(
		mapping::rename_type::UUID& selected_scene_id,
		ecs::Registry& registry,
		scene::SceneManager& scene_manager)
	{
		if (is_create_new_scene)
		{
			ImGui::OpenPopup(popup_new_scene.c_str());
		}

		// ��ʂ̈ʒu�Z�o
		const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal(popup_new_scene.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			const system::System& system = *locator::Locator::GetSystem();
			const std::string window_text{ "Do you want to save the changes you made in the scene.\n" +
											system.GetCurrentScenePath() + "\n\n" +
											"Your changes will be lost if you don't save them." };
			ImGui::Text(window_text.c_str());
			ImGui::Separator();

			if (ImGui::Button("Save", ImVec2(140, 0)))
			{
				// ���݂̃V�[�������uNoTitled�v(�V�[���������݂��Ȃ�)�̏ꍇ
				if(const std::string& current_scene_path = system.GetCurrentScenePath();
					current_scene_path == filename_helper::GetNoTitled())
				{// �V�[���𖼑O��t���ĕۑ�
					const auto destination = pfd::save_file("Save", "", {}, pfd::opt::none).result();
					if (destination.empty())
						ImGui::CloseCurrentPopup();

					const std::filesystem::path path = destination;
					const std::string relative_path  = utility::ConvertAbsolutePathToRelativePath(path.string());
					locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->SaveAs(*registry.GetScene(), relative_path);
				}
				else
				{// �V�[���������݂��Ă���
					locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->Save(*registry.GetScene(), current_scene_path);
				}
				locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->CreateScene(scene_manager);
				selected_scene_id = scene_manager.GetActiveScenes()->begin()->first;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Don't Save", ImVec2(140, 0)))
			{
				locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>()->CreateScene(scene_manager);
				selected_scene_id = scene_manager.GetActiveScenes()->begin()->first;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if(ImGui::Button("Cancel", ImVec2(140, 0)))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
} // cumulonimbus::editor
