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

		void Update(float dt);
		void Render() const;

		[[nodiscard]]
		camera::SceneViewCamera& GetSceneViewCamera() const { return *scene_view_camera.get(); }
	private:
		std::unique_ptr<camera::SceneViewCamera> scene_view_camera{};
	};
} // cumulonimbus::editor
