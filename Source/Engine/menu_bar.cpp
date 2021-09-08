#include "menu_bar.h"

#include <filesystem>

#include "scene.h"
#include <portable-file-dialogs.h>
#include "ecs.h"

class Scene;

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
				FileMenu(registry);
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

	void MenuBar::FileMenu(ecs::Registry* registry)
	{
		if(ImGui::MenuItem("New Scene","Ctrl+Shift"))
		{

		}
		if(ImGui::MenuItem("Open Scene","Ctrl+O"))
		{
			const auto selection = pfd::open_file(
				"Open Scene",
				"",
				{ "Scene Files","*.scene" },
				pfd::opt::none).result();

			if (selection.empty())
				return;
			const std::filesystem::path p = selection.at(0);

			registry->GetScene()->LoadScene(p.parent_path().string(), p.filename().string());

		}
		ImGui::Separator();
		if(ImGui::MenuItem("Save","Ctrl+S"))
		{

		}
		if(ImGui::MenuItem("Save As...","Ctrl+Shift+S"))
		{
			const auto destination = pfd::save_file("Save", "", {}, pfd::opt::none).result();
			if (destination.empty())
				return;

			const std::filesystem::path p = destination;
			std::filesystem::create_directory(p);
			registry->GetScene()->SaveScene(p.string(), p.filename().string());
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
