#pragma once
#include <memory>

#include "content_browser.h"
#include "hierarchy.h"
#include "rename_type_mapping.h"

class Scene;

namespace cumulonimbus::editor
{
	class EditorManager final
	{
	private:
		std::unique_ptr<ContentBrowser> content_browser{};
		std::unique_ptr<Hierarchy> hierarchy{};

	public:
		explicit EditorManager();
		~EditorManager() = default;

		void RenderEditor(Scene* scene, ecs::Registry* registry) const;
	};
} // cumulonimbus::editor