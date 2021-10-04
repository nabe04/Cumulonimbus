#include "hierarchy.h"

#include "ecs.h"

namespace
{
	const std::string context_id{ "Context_Menu" };	// ImGui::PopupのコンテキストメニューID
}

namespace cumulonimbus::editor
{
	void Hierarchy::Render(ecs::Registry* registry)
	{
		ImVec2 button_size{ 10.f,10.f };
		if (ImGui::Begin(ICON_FA_ALIGN_RIGHT" Hierarchy"))
		{
			if (ImGui::Button(ICON_FA_PLUS, { 30,30 }))
				ImGui::OpenPopup("my_file_popup");
			if (ImGui::BeginPopup("my_file_popup"))
			{
				if (ImGui::MenuItem("Create Empty"))
				{
					registry->CreateEntity();
				}
				ImGui::EndPopup();
			}

			for (auto& [key, value] : registry->GetEntities())
			{
				//value.second
				//if (ImGui::Selectable(value.second.c_str(), selected_entity == key,ImGuiSelectableFlags_AllowItemOverlap))
				//{
				//	selected_entity = key;

				//}

				ImGui::Selectable(value.second.c_str(), selected_entity == key);
				if(ImGui::IsItemHovered())
				{// ヒエラルキービュー上でのアイテム選択
					if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{// 左クリック時
						selected_entity = key;
					}
					if(ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{// 右クリック時
						ImGui::OpenPopup(context_id.c_str());
					}
				}
			}

			//
			ContextMenu();
		}
		ImGui::End();
	}

	void Hierarchy::ContextMenu()
	{
		if(ImGui::BeginPopup(context_id.c_str()))
		{
			if (ImGui::MenuItem("Create Prefab"))
			{
				CreatePrefab();
			}
			ImGui::MenuItem("Delete");


			ImGui::EndPopup();
		}
	}

	void Hierarchy::CreatePrefab()
	{

	}

} // cumulonimbus::editor
