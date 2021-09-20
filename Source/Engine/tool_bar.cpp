#include "tool_bar.h"

namespace cumulonimbus::editor
{
	ToolBar::ToolBar()
	{
		pps_state.reset();
	}

	void ToolBar::Render(ecs::Registry* registry)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { .9f,.0,.9f,1 });
		ImGui::Begin("##Tool Bar",nullptr);
		const float im_window_width = ImGui::GetWindowWidth();

		ImGui::SetCursorPosX((im_window_width / 2) - pps_size.x);
		if(ImGui::Button(ICON_FA_PLAY, { pps_size.x,pps_size.y }))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_PAUSE, { pps_size.x,pps_size.y }))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_STEP_FORWARD, { pps_size.x,pps_size.y }))
		{

		}
		//ImGui::GetItemRectSize();
		//ImGui::PopItemWidth();

		ImGui::End();
		ImGui::PopStyleColor();

	}
} // cumulonimbus::editor
