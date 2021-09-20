#pragma once
#include <memory>

#include "content_browser.h"
#include "hierarchy.h"
#include "inspector.h"
#include "menu_bar.h"
#include "project_view.h"
#include "tool_bar.h"

namespace cumulonimbus
{
	namespace scene
	{
		class Scene;
	} // scene
} // cumulonimbus

namespace cumulonimbus::editor
{
	class EditorManager final
	{
	private:
		std::unique_ptr<ContentBrowser> content_browser{};
		std::unique_ptr<Hierarchy>		hierarchy{};
		std::unique_ptr<Inspector>		inspector{};
		std::unique_ptr<MenuBar>		menu_bar{};
		std::unique_ptr<ProjectView>	project_view{};
		std::unique_ptr<ToolBar>		tool_bar{};

	public:
		explicit EditorManager();
		~EditorManager() = default;

		void RenderEditor(scene::Scene* scene, ecs::Registry* registry) const;
	};
} // cumulonimbus::editor