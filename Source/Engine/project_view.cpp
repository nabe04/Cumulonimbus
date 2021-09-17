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
		// デフォルトのIDをModelにセット
		current_selected_id = utility::GetHash<asset::Model>();

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

		ImGui::Begin(ICON_FA_FOLDER"Project");
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
				// アセットロード(モデル)
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
				// アセット(テクスチャ)
				locator::Locator::GetAssetManager()->AddAsset(selection.at(0));
			}
			ImGui::EndPopup();
		}
	}

	std::filesystem::path ProjectView::ShowAllAssets(const asset::AssetManager& asset_manager)
	{
		if (current_selected_navigation == NavigationType::FileTree)
			current_selected_id = {};

		if(ImGui::TreeNodeEx(ICON_FA_SEARCH"All Assets", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (const auto& [hash, asset_sheet] : asset_manager.GetAssetSheetManager().GetSheets())
			{
				if(!connector.contains(hash))
					continue;

				const std::string asset_name = ICON_FA_SEARCH + connector.at(hash);
				if (ImGui::Selectable(asset_name.c_str(), current_selected_id == hash))
				{// 選択された時
					// 選択されたクラスのハッシュ値のセット
					current_selected_id = hash;
					// File and Folder Listに表示する項目のセット
					current_selected_navigation = NavigationType::AllAssets;
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

		if(current_selected_path == path)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		{
			ImGui::Text(icon);
			ImGui::SameLine();
			is_open = ImGui::TreeNodeEx(path_name.c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				// 現在選択されているパスのセット
				current_selected_path = path;
				// File and Folder Listに表示する項目のセット
				current_selected_navigation = NavigationType::FileTree;
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


		//for (const auto& file : std::filesystem::recursive_directory_iterator(path))
		//{
		//}
		//if(ImGui::TreeNodeEx(path_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		//{
		//	int n = 0;
		//	for (const auto& file : std::filesystem::recursive_directory_iterator(path))
		//	{
		//		if (ImGui::IsItemClicked())
		//		{
		//			current_selected_path = file.path();
		//			// File and Folder Listに表示する項目のセット
		//			current_selected_navigation = NavigationType::FileTree;
		//		}
		//		if(file.path() == current_selected_path)
		//		{

		//		}

		//		if (file.path().parent_path() == path)
		//		{
		//			if (std::filesystem::is_directory(file))
		//			{
		//				ShowFolderTree(file.path());
		//			}
		//			else
		//			{

		//				//static int selected = -1;
		//				//std::string name = file.path().filename().string();
		//				//if (ImGui::Selectable(name.c_str(), selected == n))
		//				//	selected = n;
		//				//++n;
		//			}
		//		}
		//	}
		//	ImGui::TreePop();
		//}
		return {};
	}

	std::filesystem::path ProjectView::ShowFileAndFolderList(const asset::AssetManager& asset_manager)
	{
		if(current_selected_navigation == NavigationType::AllAssets)
		{
			if (!asset_manager.GetAssetSheetManager().HasSheet(current_selected_id))
				return {};
			for(auto& [uuid,path] : asset_manager.GetAssetSheetManager().GetSheets().at(current_selected_id).sheet)
			{
				ImGui::Text(path.c_str());
			}
			//asset_manager.GetAssetSheetManager().GetSheets().
		}
		else if(current_selected_navigation == NavigationType::FileTree)
		{
			for (const auto& file : std::filesystem::recursive_directory_iterator(current_selected_path))
			{
				const std::string name = file.path().string();
				ImGui::Text(name.c_str());
			}
		}

		return {};
	}



} // cumulonimbus::editor
