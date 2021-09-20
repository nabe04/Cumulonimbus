#include "project_view.h"

#include <vector>

#include <Windows.h>

#include <portable-file-dialogs.h>

#include "asset_sheet_manager.h"
#include "locator.h"
#include "generic.h"
#include "material.h"
#include "model.h"
#include "texture.h"


namespace
{
	const std::string viewer_dir{ "./Data/Assets" };
}

namespace cumulonimbus::editor
{
	ProjectView::ProjectView()
	{
		// �f�t�H���g��ID��Model�ɃZ�b�g
		selected_id = utility::GetHash<asset::Model>();

		//icon = ICON_FA_FOLDER_OPEN;
		is_folder_open.emplace(viewer_dir, false);
		for (const auto& file : std::filesystem::recursive_directory_iterator(viewer_dir))
		{
			if (std::filesystem::is_directory(file))
			{
				is_folder_open.emplace(file.path(), false);
			}
		}

		Register<asset::Material>("All Materials");
		Register<asset::Model>("All Models");
		Register<asset::Texture>("All Textures");
	}

	void ProjectView::Render(ecs::Registry* registry)
	{
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

		ImGui::Begin(ICON_FA_FOLDER" Project");
		if(ImGui::BeginTable("##table",2, flags))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("##Navigation Pane", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("##File and Folder list", ImGuiTableColumnFlags_None);
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			{
				ImGui::TableSetColumnIndex(0);
				ImportMenu();
				ShowAllAssets(*locator::Locator::GetAssetManager());
				ShowFolderTree(viewer_dir);
			}
			{
				ImGui::TableSetColumnIndex(1);
				ShowFileAndFolderList(*locator::Locator::GetAssetManager());
			}
			ImGui::EndTable();
		}
		ImGui::End();
	}

	template <class Hash>
	void ProjectView::Register(const std::string& name)
	{
		const mapping::rename_type::Hash hash = utility::GetHash<Hash>();
		if (connector.contains(hash))
			return;

		connector.emplace(hash, name);
	}

	void ProjectView::ImportMenu()
	{
		if (ImGui::Button(ICON_FA_PLUS "Import" ICON_FA_SORT_DOWN))
			ImGui::OpenPopup("my_file_popup");
		if (ImGui::BeginPopup("my_file_popup"))
		{
			if(ImGui::MenuItem("Model"))
			{
				const auto selection = pfd::open_file{
					"Import",
					"",
					{"Model","*.fbx *.FBX *.model"} }.result();

				if (selection.empty())
				{
					ImGui::End();
					return;
				}
				// �A�Z�b�g���[�h(���f��)
				locator::Locator::GetAssetManager()->AddAsset(selection.at(0));
			}
			ImGui::Separator();
			if(ImGui::MenuItem("Material"))
			{

			}
			ImGui::Separator();
			if(ImGui::MenuItem("Texture"))
			{
				const auto selection = pfd::open_file{
					"Import",
					"",
					{"All Files","*"}}.result();

				if (selection.empty())
				{
					ImGui::End();
					return;
				}
				// �A�Z�b�g(�e�N�X�`��)
				locator::Locator::GetAssetManager()->AddAsset(selection.at(0));
			}
			ImGui::EndPopup();
		}
	}

	std::filesystem::path ProjectView::ShowAllAssets(const asset::AssetManager& asset_manager)
	{
		if (selected_navigation == NavigationType::FileTree)
			selected_id = {};

		if(ImGui::TreeNodeEx(ICON_FA_SEARCH"All Assets", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (const auto& [hash, asset_sheet] : asset_manager.GetAssetSheetManager().GetSheets())
			{
				if(!connector.contains(hash))
					continue;

				const std::string asset_name = ICON_FA_SEARCH + connector.at(hash);
				if (ImGui::Selectable(asset_name.c_str(), selected_id == hash))
				{// �I�����ꂽ��
					// �I�����ꂽ�N���X�̃n�b�V���l�̃Z�b�g
					selected_id = hash;
					// File and Folder List�ɕ\�����鍀�ڂ̃Z�b�g
					selected_navigation = NavigationType::AllAssets;
				}
			}
			ImGui::TreePop();
		}
		return {};
	}

	std::filesystem::path ProjectView::ShowFolderTree(const std::filesystem::path& path)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

		auto icon = ICON_FA_FOLDER;
		if (is_folder_open.at(path))
			icon = ICON_FA_FOLDER_OPEN;
		const std::string path_name = path.filename().string();
		bool is_open = false;

		if(selected_path == path)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		{
			ImGui::Text(icon);
			ImGui::SameLine();
			is_open = ImGui::TreeNodeEx(path_name.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				// ���ݑI������Ă���p�X�̃Z�b�g
				selected_path = path;
				// File and Folder List�ɕ\�����鍀�ڂ̃Z�b�g
				selected_navigation = NavigationType::FileTree;
			}

			if (is_open)
			{
				//icon = ICON_FA_FOLDER_OPEN;
				for (const auto& file : std::filesystem::recursive_directory_iterator(path))
				{
					if (file.path().parent_path() == path)
					{
						if (std::filesystem::is_directory(file))
						{
							ShowFolderTree(file);
						}
					}
				}
				is_folder_open.at(path) = true;
				ImGui::TreePop();
			}
			else
			{
				is_folder_open.at(path) = false;
			}

		}
		return {};
	}

	std::filesystem::path ProjectView::ShowFileAndFolderList(const asset::AssetManager& asset_manager)
	{
		ImGui::PushItemWidth(200);
		ImGui::SliderFloat("##Texture Size", &item_size, 10, 300,"size : %.1f");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text(selected_file.string().c_str());

		if (selected_navigation == NavigationType::AllAssets)
		{
			if (!asset_manager.GetAssetSheetManager().HasSheet(selected_id))
				return {};

			int n = 0;
			ImGuiStyle& style = ImGui::GetStyle();
			const float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			for (auto& [uuid, path_str] : asset_manager.GetAssetSheetManager().GetSheets().at(selected_id).sheet)
			{
				ImGui::PushID(uuid.c_str());
				std::filesystem::path path{ path_str };
				// �{�^���̕\��
				helper::imgui::ImageButtonWithText(uuid, std::string{ path.filename().string() }.c_str(), button_state, { item_size,item_size });
				if (button_state.pressed)
					selected_file = path;

				// �{�^���z�u�ʒu�̒���
				const float last_button_x2 = ImGui::GetItemRectMax().x;
				const float next_button_x2 = last_button_x2 + style.ItemSpacing.x + item_size;
				if (n + 1 < asset_manager.GetAssetSheetManager().GetSheets().at(selected_id).sheet.size()
					&& next_button_x2 < window_visible_x2)
					ImGui::SameLine();
				ImGui::PopID();
				// �h���b�N & �h���b�v����
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					ImGui::SetDragDropPayload("cell", &n, sizeof(int));
					helper::imgui::Image(uuid);
					ImGui::Text(path_str.c_str());
					ImGui::EndDragDropSource();
				}

				++n;
			}
		}
		else if(selected_navigation == NavigationType::FileTree)
		{
			for (const auto& file : std::filesystem::recursive_directory_iterator(selected_path))
			{
				const std::string name = file.path().string();
				ImGui::Text(name.c_str());
			}
		}

		return {};
	}
} // cumulonimbus::editor
