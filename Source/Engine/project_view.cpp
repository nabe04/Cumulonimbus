#include "project_view.h"

#include <Windows.h>

#include <portable-file-dialogs.h>

#include "locator.h"

namespace cumulonimbus::editor
{
	ProjectView::ProjectView()
	{
		viewer_size.x = locator::Locator::GetWindow()->Width() * 0.1f;
	}

	void ProjectView::Render(ecs::Registry* registry)
	{
		ImGui::Begin(ICON_FA_FOLDER"Project");

		viewer_size.y = ImGui::GetWindowSize().y;
		float w = ImGui::GetWindowSize().x - viewer_size.x;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::BeginChild("Viewer", ImVec2(viewer_size.x, viewer_size.y), true);
		ImportMenu();
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::InvisibleButton("vsplitter", ImVec2(5.0f, viewer_size.y), ImGuiButtonFlags_None);
		if (ImGui::IsAnyItemFocused())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		}
		if (ImGui::IsItemActive())
		{
			viewer_size.x += ImGui::GetIO().MouseDelta.x;
		}

		ImGui::SameLine();
		ImGui::BeginChild("Current directory", ImVec2(w, viewer_size.y), true);
		ImGui::EndChild();
		ImGui::PopStyleVar();
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

} // cumulonimbus::editor
