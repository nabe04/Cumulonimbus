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

	void SceneView::Render()
	{
		ImVec2 mouse_pos{};
		ImGui::Begin(ICON_FA_BORDER_ALL" Scene");
		mouse_pos.x = ImGui::GetCurrentWindow()->Pos.x;
		mouse_pos.y = ImGui::GetCurrentWindow()->Pos.y;
		title_bar_height = ImGui::GetCurrentWindow()->TitleBarHeight();
		// ImGui上のウィンドウサイズに合うように調整
		image_size = arithmetic::CalcWindowSize(Window::aspect_ratio,
												static_cast<int>(ImGui::GetContentRegionAvail().x),
												static_cast<int>(ImGui::GetContentRegionAvail().y));
		// ウィンドウが画面の中央に来るように調整
		window_offset.y = (ImGui::GetContentRegionAvail().y - image_size.y) / 2.f;
		ImGui::Dummy({ .0f,window_offset.y });

		helper::imgui::Image(*scene_view_camera->GetCamera().GetFrameBufferSRV_Address(), { image_size.x,image_size.y });

		auto win = ImGui::GetCurrentWindow();
		if (ImGui::IsWindowHovered())
		{
			scene_view_camera->EditCamera(ImGui::GetCurrentWindow());
		}

		ImGui::End();

		ImGui::Begin("Size Text");


		RECT rect{};
		GetClientRect(locator::Locator::GetWindow()->GetHWND(), &rect);
		window_mouse_pos.x = ImGui::GetMousePos().x - mouse_pos.x;
		window_mouse_pos.y = ImGui::GetMousePos().y - mouse_pos.y - title_bar_height - window_offset.y;

		ImGui::Text("Mouse X : %f", window_mouse_pos.x);
		ImGui::Text("Mouse Y : %f", window_mouse_pos.y);

		auto win_pos = ConvertWindowPos();
		ImGui::Text("Win X : %d", win_pos.x);
		ImGui::Text("Win Y : %d", win_pos.y);


		auto ndc_pos = arithmetic::ConvertScreenToNDC(window_mouse_pos, locator::Locator::GetWindow()->Width(), locator::Locator::GetWindow()->Height());
		auto inv_view = scene_view_camera->GetCamera().GetViewMat().Invert();
		auto inv_proj = scene_view_camera->GetCamera().GetProjectionMat().Invert();

		//DirectX::XMMatrixMultiply(ndc_pos, inv_proj);
		const DirectX::SimpleMath::Vector3 world_pos = arithmetic::ConvertScreenToWorld({ static_cast<float>(win_pos.x),static_cast<float>(win_pos.y),0 },
																						GetSceneViewCamera().GetCamera().GetViewMat(),
																						GetSceneViewCamera().GetCamera().GetProjectionMat());

		ImGui::Text("World X : %f" ,world_pos.x);
		ImGui::Text("World Y : %f", world_pos.y);
		ImGui::Text("World Z : %f", world_pos.z);


		//DirectX::XMMatrixMultiply(ndc_pos, inv_proj);
		const DirectX::SimpleMath::Vector3 world_pos_2 = arithmetic::ConvertScreenToWorld({ static_cast<float>(win_pos.x),static_cast<float>(win_pos.y),1.f },
																						GetSceneViewCamera().GetCamera().GetViewMat(),
																						GetSceneViewCamera().GetCamera().GetProjectionMat());

		ImGui::Text("World far X : %f", world_pos_2.x);
		ImGui::Text("World far Y : %f", world_pos_2.y);
		ImGui::Text("World far Z : %f", world_pos_2.z);



		ImGui::End();
	}

	DirectX::XMINT2 SceneView::ConvertWindowPos() const
	{
		// ウィンドウ外にマウスがある場合
		if(window_mouse_pos.x < 0 ||
		   window_mouse_pos.y < 0)
		{
			return { -1,-1 };
		}

		if(window_mouse_pos.x > image_size.x ||
		   window_mouse_pos.y > image_size.y)
		{
			return { -1,-1 };
		}

		RECT rect{};
		GetClientRect(locator::Locator::GetWindow()->GetHWND(), &rect);
		const DirectX::SimpleMath::Vector2 magnification
		{
			rect.right / image_size.x,
			rect.bottom / image_size.y
		};

		const DirectX::XMINT2 result
		{
			static_cast<int>(window_mouse_pos.x * magnification.x),
			static_cast<int>(window_mouse_pos.y * magnification.y)
		};

		return result;
	}

} // cumulonimbus::editor
