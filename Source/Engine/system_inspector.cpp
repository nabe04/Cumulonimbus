#include "system_inspector.h"

#include "ecs.h"
#include "scene.h"

namespace cumulonimbus::editor
{
	void SystemInspector::Render(ecs::Registry* registry)
	{
		if (ImGui::Begin(ICON_FA_GLOBE"System"))
		{
			registry->GetScene()->GetCollisionManager()->RenderImGui(registry);
		}
		ImGui::End();
	}

} // cumulonimbus::editor
