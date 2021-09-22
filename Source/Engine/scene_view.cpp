#include "scene_view.h"

#include <imgui_internal.h>

#include "arithmetic.h"
#include "cum_imgui_helper.h"
#include "locator.h"

namespace cumulonimbus::editor
{
	SceneView::SceneView()
	{
		scene_view_camera = std::make_unique<camera::SceneViewCamera>(
								static_cast<float>(locator::Locator::GetWindow()->Width()),
								static_cast<float>(locator::Locator::GetWindow()->Height()));


	}

	void SceneView::Update(const float dt)
	{
		scene_view_camera->Update(dt);
	}

	void SceneView::Render() const
	{
		ImGui::Begin(ICON_FA_BORDER_ALL" Scene");
		if (ImGui::IsWindowHovered())
			scene_view_camera->EditCamera(ImGui::GetCurrentWindow());
		// ImGui��̃E�B���h�E�T�C�Y�ɍ����悤�ɒ���
		const auto size = arithmetic::CalcWindowSize(Window::aspect_ratio,
													 static_cast<int>(ImGui::GetContentRegionAvail().x),
													 static_cast<int>(ImGui::GetContentRegionAvail().y));
		// �E�B���h�E����ʂ̒����ɗ���悤�ɒ���
		ImGui::Dummy({ .0f,(ImGui::GetContentRegionAvail().y - size.y) / 2.f });

		helper::imgui::Image(*scene_view_camera->GetCamera().GetFrameBufferSRV_Address(), { size.x,size.y });
		ImGui::End();
	}
} // cumulonimbus::editor
