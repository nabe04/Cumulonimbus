#include "hierarchy.h"

#include "ecs.h"

namespace cumulonimbus::editor
{
	void Hierarchy::Render(ecs::Registry* registry)
	{
		ImGui::Begin("Hierarchy");

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
