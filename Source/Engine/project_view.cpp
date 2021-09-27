#include "project_view.h"

#include <vector>
#include <filesystem>

#include <Windows.h>
#include <portable-file-dialogs.h>

#include "imgui_stdlib.h"

#include "asset_manager.h"
#include "file_path_helper.h"
#include "generic.h"
#include "locator.h"
#include "material.h"
#include "material_loader.h"
#include "model.h"
#include "texture.h"
#include "texture_loader.h"


namespace
{
	const std::string viewer_dir{ "./Data/Assets" };
	const std::string context_id{ "context_menu" }; // ImGui::OpenPopup,BeginPopupのコンテキストメニューID
}

namespace cumulonimbus::editor
{
	ProjectView::ProjectView()
	{
		// デフォルトのIDをModelにセット
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

	void ProjectView::Render(const ecs::Registry* registry)
	{
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

		if (ImGui::Begin(ICON_FA_FOLDER" Project"))
		{
			if (ImGui::BeginTable("##table", 2, flags))
			{
				ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
				ImGui::TableSetupColumn("##Navigation Pane", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("##File and Folder List", ImGuiTableColumnFlags_None);
				ImGui::TableHeadersRow();
				ImGui::TableNextRow();
				{// Navigation Pane
					ImGui::TableSetColumnIndex(0);
					ImportMenu(*locator::Locator::GetAssetManager());
					ShowAllAssets(*locator::Locator::GetAssetManager());
					ShowFolderTree(viewer_dir);
				}
				{// File and Folder List
					ImGui::TableSetColumnIndex(1);
					ShowFileAndFolderList(*locator::Locator::GetAssetManager());
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}

	bool ProjectView::DraggingAsset(std::filesystem::path& path) const
	{
		if (!is_dragged)
			return false;

		path = selected_file;
		return true;
	}

	template <class Hash>
	void ProjectView::Register(const std::string& name)
	{
		const mapping::rename_type::Hash hash = utility::GetHash<Hash>();
		if (connector.contains(hash))
			return;

		connector.emplace(hash, name);
	}

	void ProjectView::ImportMenu(asset::AssetManager& asset_manager) const
	{
		if (ImGui::Button(ICON_FA_FILE_IMPORT "Import" ICON_FA_SORT_DOWN))
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
				asset_manager.AddAsset(selection.at(0));
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
				asset_manager.AddAsset(selection.at(0));
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine(0,15);
		if (ImGui::Button(ICON_FA_PLUS "Create" ICON_FA_SORT_DOWN))
			ImGui::OpenPopup("create_popup");

		if (ImGui::BeginPopup("create_popup"))
		{
			if (ImGui::MenuItem("Material"))
			{
				const auto id = asset_manager.GetLoader<asset::MaterialLoader>()->CreateMaterial(asset_manager, viewer_dir);
			}
			ImGui::Separator();
			ImGui::EndPopup();
		}
	}

	void ProjectView::ContextMenu()
	{
		std::string name;

		if(ImGui::BeginPopup(context_id.c_str()))
		{
			if (ImGui::MenuItem("Delete"))
			{
				// 現在選択されているアセットの削除
				delete_file = selected_file;
				locator::Locator::GetAssetManager()->DeleteLoader(delete_file);
			}
			if(ImGui::MenuItem("Rename"))
			{
				rename = selected_file.filename().string();
				rename_id = locator::Locator::GetAssetManager()->GetAssetSheetManager().Search(selected_file);
				is_rename = true;
			}
			ImGui::EndPopup();
		}

		//if (is_rename)
			//ImGui::InputText("Test",&name);
	}

	void ProjectView::RenameItem()
	{
		//const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		const ImVec2 offset{ item_size,-item_size };
		const ImVec2 window_pos = ImGui::GetCursorScreenPos();
		ImGui::SetNextWindowPos(ImVec2{window_pos.x + offset.x * 2.5f , window_pos.y + offset.y}, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2{ item_size * 2,item_size * 2 });
		if(ImGui::Begin(ICON_FA_EDIT"Rename"))
		{
			ImGui::InputText("##Rename Text", &rename);
			if (ImGui::Button("Cancel"))
				is_rename = false;
			//ImGui::SetNextWindowPos()
		}
		ImGui::End();
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
				{// 選択された時
					// 選択されたクラスのハッシュ値のセット
					selected_id = hash;
					// File and Folder Listに表示する項目のセット
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
				// 現在選択されているパスのセット
				selected_path = path;
				// File and Folder Listに表示する項目のセット
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

	std::filesystem::path ProjectView::ShowFileAndFolderList(asset::AssetManager& asset_manager)
	{
		is_dragged = false;
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
				// ボタンの表示
				helper::imgui::ImageButtonWithText(uuid, std::string{ path.filename().string() }.c_str(),
												button_state, { item_size,item_size });

				if (button_state.pressed ||
					button_state.held ||
					button_state.hovered)
					selected_file = path;

				if (is_rename && (rename_id == uuid))
				{
					RenameItem();
				}

				if(button_state.hovered)
				{
					if(ImGui::IsMouseReleased(ImGuiMouseButton_Right))
					{
						ImGui::OpenPopup(context_id.c_str());
					}
				}
				// OpenPopup時コンテキストメニューの表示
				ContextMenu();

				// ボタン配置位置の調整
				const float last_button_x2 = ImGui::GetItemRectMax().x;
				const float next_button_x2 = last_button_x2 + style.ItemSpacing.x + item_size;
				if (n + 1 < asset_manager.GetAssetSheetManager().GetSheets().at(selected_id).sheet.size()
					&& next_button_x2 < window_visible_x2)
					ImGui::SameLine();
				ImGui::PopID();
				// ドラック & ドロップ操作
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					is_dragged = true;
					ImGui::SetDragDropPayload("cell", &n, sizeof(int));
					helper::imgui::Image(uuid);
					ImGui::Text(path_str.c_str());
					ImGui::EndDragDropSource();
				}

				++n;
			}
			// 削除される要素の削除
			if (!delete_file.empty())
			{
				asset_manager.DeleteAsset(delete_file);
				asset_manager.Save();
				delete_file.clear();
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
