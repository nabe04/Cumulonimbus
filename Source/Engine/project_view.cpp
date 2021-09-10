#include "project_view.h"

#include <Windows.h>

#include <portable-file-dialogs.h>

#include "locator.h"

namespace cumulonimbus::editor
{
	void ProjectView::Render(ecs::Registry* registry)
	{
		ImGui::Begin(ICON_FA_FOLDER"Project");
		if(ImGui::Button("Import"))
		{
			const auto selection = pfd::open_file{
				"Import",
				"",
				{"All Files","*"}
			}.result();

			if (selection.empty())
				return;

			locator::Locator::GetAssetManager()->AddAsset(selection.at(0));
		}
		ImGui::End();
	}
} // cumulonimbus::editor
