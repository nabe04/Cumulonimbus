#include "scene_view.h"

#include "cum_imgui_helper.h"
#include "locator.h"

namespace cumulonimbus::editor
{
	SceneView::SceneView()
	{
		scene_view_camera = std::make_unique<camera::SceneViewCamera>(
								locator::Locator::GetWindow()->Width(),
								locator::Locator::GetWindow()->Height());


	}

	void SceneView::Update(const float dt)
	{
		scene_view_camera->Update(dt);
	}

	void SceneView::Render() const
	{
		ImGui::Begin(ICON_FA_BORDER_ALL" Scene");
		const auto size = ImGui::GetWindowSize();
		helper::imgui::Image(*scene_view_camera->GetCamera().GetFrameBufferSRV_Address(), { size.x,size.y });
		ImGui::End();
	}
} // cumulonimbus::editor
