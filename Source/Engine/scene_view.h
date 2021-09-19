#pragma once
#include <memory>
#include "scene_view_camera.h"

namespace cumulonimbus::editor
{
	class SceneView final
	{
	public:
		explicit SceneView();
		~SceneView() = default;

	private:
		std::unique_ptr<camera::SceneViewCamera> scene_view_camera{};
	};
} // cumulonimbus::editor
