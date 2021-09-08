#include "menu_bar.h"

#include <portable-file-dialogs.h>

namespace cumulonimbus::editor
{
	void MenuBar::Render(ecs::Registry* registry)
	{
		//-- ウィンドウ設定 --//
		bool* is_open;

		ImGuiWindowFlags window_flags = 0;
        if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
        if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_MenuBar;

		//-- メニューバー --//
        ImGui::Begin("Menu Bar", nullptr, window_flags);
		if(ImGui::BeginMenuBar())
		{
			if(ImGui::BeginMenu("File"))
			{
				FileMenu();
				ImGui::EndMenu();
			}

			if(ImGui::BeginMenu("Window"))
			{
				if(ImGui::MenuItem("Menu Setting"))
					is_menu_open = true;

				ImGui::EndMenu();
			}

			if(ImGui::BeginMenu("Help"))
			{

				ImGui::EndMenu();;
			}

			ImGui::EndMenuBar();
		}
		ImGui::End();

		if (is_menu_open)
			MenuSetting();
	}

	void MenuBar::FileMenu()
	{
		if(ImGui::MenuItem("New Scene","Ctrl+Shift"))
		{

		}
		if(ImGui::MenuItem("Open Scene","Ctrl+O"))
		{
			auto selection = pfd::open_file(
				"Open Scene",
				"",
				{ "Scene Files","*.scene" },
				pfd::opt::none).result();



		}
		ImGui::Separator();
		if(ImGui::MenuItem("Save","Ctrl+S"))
		{

		}
		if(ImGui::MenuItem("Save As...","Ctrl+Shift+S"))
		{
			auto destination = pfd::save_file("Save");
		}
	}

	void MenuBar::MenuSetting()
	{
		ImGui::Begin("Menu Setting", &is_menu_open);

		if (ImGui::BeginTable("table", 1))
		{
			ImGui::TableNextColumn(); ImGui::Checkbox("No titlebar", &no_titlebar);
			ImGui::TableNextColumn(); ImGui::Checkbox("No move", &no_move);
			ImGui::EndTable();
		}

		ImGui::End();
	}


} // cumulonimbus::editor
