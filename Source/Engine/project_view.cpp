#include "project_view.h"

#include <vector>

#include <Windows.h>

#include <portable-file-dialogs.h>

#include "locator.h"

namespace
{
	const std::string viewer_dir{ "./Data/Assets" };
}

namespace cumulonimbus::editor
{
	ProjectView::ProjectView()
	{
		viewer_size.x = locator::Locator::GetWindow()->Width() * 0.1f;
	}

	void ProjectView::Render(ecs::Registry* registry)
	{
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

		ImGui::Begin(ICON_FA_FOLDER"Project");
		if(ImGui::BeginTable("##table",2, flags))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("##Viewer", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("##Current Directory", ImGuiTableColumnFlags_None);
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			{
				ImGui::TableSetColumnIndex(0);
				ImportMenu();
				ShowFileTree(viewer_dir);
			}
			{
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("Directory");
			}
			ImGui::EndTable();
		}

		//viewer_size.y = ImGui::GetWindowSize().y;
		//float w = ImGui::GetWindowSize().x - viewer_size.x;
		//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		//ImGui::BeginChild("Viewer", ImVec2(viewer_size.x, viewer_size.y), true);
		//ImportMenu();
		//ImGui::EndChild();
		//ImGui::SameLine();
		//ImGui::InvisibleButton("vsplitter", ImVec2(5.0f, viewer_size.y), ImGuiButtonFlags_None);
		//if (ImGui::IsAnyItemFocused())
		//{
		//	ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		//}
		//if (ImGui::IsItemActive())
		//{
		//	viewer_size.x += ImGui::GetIO().MouseDelta.x;
		//}

		//ImGui::SameLine();
		//ImGui::BeginChild("Current directory", ImVec2(w, viewer_size.y), true);
		//ImGui::EndChild();
		//ImGui::PopStyleVar();
		ImGui::End();
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

	std::filesystem::path ProjectView::ShowFileTree(const std::filesystem::path& path)
	{
		std::vector<std::filesystem::path> directories_path{};
		std::string path_name = path.filename().string();
		if(ImGui::TreeNode(path_name.c_str()))
		{
			for (const auto& file : std::filesystem::recursive_directory_iterator(path))
			{
				if (file.path().parent_path() == path)
				{
					if (std::filesystem::is_directory(file))
					{
						ShowFileTree(file.path());
					}
					else
					{
						std::string name = file.path().filename().string();
						ImGui::Text(name.c_str());
						if (file.path() == path)
						{
							//std::string name = file.path().filename().string();
							//ImGui::Text(name.c_str());
						}
					}
				}
				//if (std::filesystem::is_directory(file))
				//{
				//	ShowFileTree(file.path());
				//}
				//else
				//{
				//	if (file.path() == path)
				//	{
				//		//std::string name = file.path().filename().string();
				//		//ImGui::Text(name.c_str());
				//	}
				//}
			}
			ImGui::TreePop();
		}
		return {};
		//for (const auto& file : std::filesystem::recursive_directory_iterator(viewer_dir))
		//{
		//	if (std::filesystem::is_directory(file))
		//	{
		//
		//	}
		//		//directories_path.emplace_back(file.path());
		//}
	}


} // cumulonimbus::editor
