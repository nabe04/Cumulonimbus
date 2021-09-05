#include "inspector.h"

#include "ecs.h"

namespace cumulonimbus::editor
{
	void Inspector::Render(ecs::Registry* registry, mapping::rename_type::Entity ent)
	{
		ImGui::Begin("Inspector");

		for(auto& [comp_name,comp_data] : registry->GetComponentArrays())
		{
			comp_data->RenderImGui(ent);
		}

		ImGui::End();
	}

} // cumulonimbus::editor