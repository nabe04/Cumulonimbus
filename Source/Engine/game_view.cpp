#include "game_view.h"

#include "ecs.h"
#include "camera_component.h"
#include "cum_imgui_helper.h"

namespace cumulonimbus::editor
{
	void GameView::Render(ecs::Registry* registry)
	{
		if (ImGui::Begin(ICON_FA_GAMEPAD"Game"))
		{
			// CameraComponent“à‚ÅMain CameraŽž‚Ì•`‰æ
			for (auto& camera_comp : registry->GetArray<component::CameraComponent>().GetComponents())
			{
				if (!camera_comp.GetIsMainCamera())
					continue;

				const auto size = ImGui::GetWindowSize();
				helper::imgui::Image(*camera_comp.GetCamera()->GetFrameBufferSRV_Address(), { size.x,size.y });
			}
		}
		ImGui::End();
	}
} // cumulonimbus::editor
