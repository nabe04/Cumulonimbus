#include "inspector.h"

#include "ecs.h"

namespace cumulonimbus::editor
{
	void Inspector::Render(ecs::Registry* registry, mapping::rename_type::Entity ent)
	{
		ImGui::Begin("Inspector");

		std::string& name = registry->GetEntities().at(ent).second;
		const int arr_size = name.length();
		char buf[256];
		strcpy_s(buf, arr_size + 1, name.c_str());
		if(ImGui::InputText("##", buf,64, ImGuiInputTextFlags_CharsNoBlank))
		{
			name = buf;
		}

		for(auto& [comp_name,comp_data] : registry->GetComponentArrays())
		{
			comp_data->RenderImGui(ent);
		}

		ImGui::End();
	}

} // cumulonimbus::editor