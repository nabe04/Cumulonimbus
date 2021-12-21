#include "game_view.h"

#include "arithmetic.h"
#include "ecs.h"
#include "camera_component.h"
#include "cum_imgui_helper.h"

namespace cumulonimbus::editor
{
	void GameView::Render(ecs::Registry* registry)
	{
		if (ImGui::Begin(ICON_FA_GAMEPAD"Game"))
		{
			// CameraComponent����Main Camera���̕`��
			for (auto& camera_comp : registry->GetArray<component::CameraComponent>().GetComponents())
			{
				if (!camera_comp.GetIsMainCamera())
					continue;

				// ImGui��̃E�B���h�E�T�C�Y�ɍ����悤�ɒ���
				image_size = arithmetic::CalcWindowSize(Window::aspect_ratio,
					static_cast<int>(ImGui::GetContentRegionAvail().x),
					static_cast<int>(ImGui::GetContentRegionAvail().y));
				// �E�B���h�E����ʂ̒����ɗ���悤�ɒ���
				window_offset.y = (ImGui::GetContentRegionAvail().y - image_size.y) / 2.f;
				ImGui::Dummy({ .0f,window_offset.y });
				helper::imgui::Image(*camera_comp.GetCamera()->GetFrameBufferSrvAddress(), { image_size.x,image_size.y });
			}
		}
		ImGui::End();
	}
} // cumulonimbus::editor
