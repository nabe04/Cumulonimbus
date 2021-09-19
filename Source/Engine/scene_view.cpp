#include "scene_view.h"

#include "locator.h"

namespace cumulonimbus::editor
{
	SceneView::SceneView()
	{
		scene_view_camera = std::make_unique<camera::SceneViewCamera>(
								locator::Locator::GetWindow()->Width(),
								locator::Locator::GetWindow()->Height());

		
	}

} // cumulonimbus::editor
