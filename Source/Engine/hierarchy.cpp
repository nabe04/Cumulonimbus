#include "hierarchy.h"

#include "ecs.h"

namespace cumulonimbus::editor
{
	void Hierarchy::Render(ecs::Registry* registry)
	{
		ImVec2 button_size{ 10.f,10.f };
		ImGui::Begin("Hierarchy");
		if (ImGui::Button(ICON_FA_PLUS, {30,30}))
			ImGui::OpenPopup("my_file_popup");
		if (ImGui::BeginPopup("my_file_popup"))
		{
			if(ImGui::MenuItem("Create Empty"))
			{
				registry->CreateEntity();
			}
			ImGui::EndPopup();
		}

		int n = 0;
		for (auto& [key, value] : registry->GetEntities())
		{
			//value.second
			if (ImGui::Selectable(value.second.c_str(), selected_entity == static_cast<mapping::rename_type::Entity>(n)))
				selected_entity = static_cast<mapping::rename_type::Entity>(n);

			++n;
		}

		ImGui::End();
	}
} // cumulonimbus::editor
