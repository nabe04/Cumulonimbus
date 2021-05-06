#pragma once

#include <memory>

#include "content_browser.h"

class Scene;

class EditorManager
{
private:
	std::unique_ptr<ContentBrawser> content_brawser{};

public:
	explicit EditorManager();
	~EditorManager() = default;

	void RenderEditor(Scene* scene);
};