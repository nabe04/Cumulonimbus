#include "scene_view.h"

#include <imgui_internal.h>

#include "arithmetic.h"
#include "asset_sheet_manager.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "file_path_helper.h"
#include "locator.h"
#include "project_view.h"
#include "prefab_loader.h"
// components
#include "model_component.h"
#include "scene.h"
#include "transform_component.h"

DirectX::SimpleMath::Vector3 w_pos;

namespace cumulonimbus::editor
{
	void SceneView::MouseHover::Update()
	{

		if(is_current_hovered)
		{
			if(is_old_hovered)
			{
				event = MouseHoverEvent::Hovering;
			}
			else
			{
				event = MouseHoverEvent::Begin_Hovered;
			}
		}
		else
		{
			if(is_old_hovered)
			{
				event = MouseHoverEvent::End_Hovered;
			}
			else
			{
				event = MouseHoverEvent::None;
			}
		}
		is_old_hovered = is_current_hovered;
	}


	SceneView::SceneView()
	{
		scene_view_camera = std::make_unique<camera::SceneViewCamera>(
								static_cast<float>(locator::Locator::GetWindow()->Width()),
								static_cast<float>(locator::Locator::GetWindow()->Height()));


	}

	void SceneView::Update(const float dt)
	{
		mouse_hover.Update();
		scene_view_camera->Update(dt);
	}


	void SceneView::Render(ecs::Registry* registry, const ProjectView* project_view, const Hierarchy* hierarchy)
	{
		ImVec2 mouse_pos{};
		if (ImGui::Begin(ICON_FA_BORDER_ALL" Scene"))
		{
			mouse_pos.x = ImGui::GetCurrentWindow()->Pos.x;
			mouse_pos.y = ImGui::GetCurrentWindow()->Pos.y;
			window_pos  = DirectX::SimpleMath::Vector2{ ImGui::GetCurrentWindow()->Pos.x,ImGui::GetCurrentWindow()->Pos.y + title_bar_height };
			window_size = DirectX::SimpleMath::Vector2{ ImGui::GetContentRegionAvail().x ,ImGui::GetContentRegionAvail().y };
			title_bar_height = ImGui::GetCurrentWindow()->TitleBarHeight();
			// ImGui上のウィンドウサイズに合うように調整
			image_size = arithmetic::CalcWindowSize(Window::aspect_ratio,
				static_cast<int>(ImGui::GetContentRegionAvail().x),
				static_cast<int>(ImGui::GetContentRegionAvail().y));
			// ウィンドウが画面の中央に来るように調整
			window_offset.y = (ImGui::GetContentRegionAvail().y - image_size.y) / 2.f;
			ImGui::Dummy({ .0f,window_offset.y });

			helper::imgui::Image(*scene_view_camera->GetCamera().GetFrameBufferSRV_Address(), { image_size.x,image_size.y });

			//{// ImGuizmoを使ってのギズモ処理
			//	auto* transform_comp = registry->TryGetComponent<component::TransformComponent>(hierarchy->GetSelectedEntity());
			//	if(transform_comp)
			//	{
			//		auto& camera = scene_view_camera->GetCamera();

			//		DirectX::SimpleMath::Matrix world_mat = transform_comp->GetWorld4x4();
			//		DirectX::SimpleMath::Matrix view_mat = camera.GetViewMat();
			//		DirectX::SimpleMath::Matrix proj_mat = camera.GetProjectionMat();
			//		ImGuiIO& io = ImGui::GetIO();
			//		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
			//		ImGuizmo::Manipulate(reinterpret_cast<float*>(&view_mat),
			//							 reinterpret_cast<float*>(&proj_mat),
			//							 ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD,
			//							 reinterpret_cast<float*>(&world_mat));

			//		transform_comp->SetWorld4x4(world_mat);
			//	}
			//}

			if (ImGui::IsWindowHovered())
			{// 画面内にカーソルがあるときに
				// カメラ操作処理
				scene_view_camera->EditCamera(ImGui::GetCurrentWindow());

			}

			//-- ドラッグ & ドロップでのモデル追加 --//
			std::filesystem::path selected_asset = ""; // アセットのリセット
			if (IsWindowHovered())
			{
				if (project_view->DraggingAsset(selected_asset))
				{
					// 「.model」形式をドラッグしているか判別
					const bool is_model  = (selected_asset.extension().string() == file_path_helper::GetModelExtension());
					// 「.prefab」形式をドラッグしているか判別
					const bool is_prefab = (selected_asset.extension().string() == file_path_helper::GetPrefabExtension());
					if (mouse_hover.event == MouseHoverEvent::Begin_Hovered)
					{
						if(is_model)
						{
							// エンティティとコンポーネント(Model)の追加
							AddModel(registry, selected_asset);
						}
						if(is_prefab)
						{
							// プレファブからコンポーネントの追加
							AddPrefab(registry, selected_asset);
						}
					}
					if (mouse_hover.event == MouseHoverEvent::Hovering)
					{
						DraggingAsset(registry);
					}
				}
			}
			else
			{
				if (project_view->DraggingAsset(selected_asset))
				{
					const bool is_model = (selected_asset.extension().string() == file_path_helper::GetModelExtension());
					if (mouse_hover.event == MouseHoverEvent::Hovering)
					{
						// エンティティとコンポーネント(Model)の削除
						registry->Destroy(dragging_entity);
						dragging_entity = {};
					}
				}
			}

		}
		ImGui::End();

		RECT rect{};
		GetClientRect(locator::Locator::GetWindow()->GetHWND(), &rect);
		window_mouse_pos.x = ImGui::GetMousePos().x - mouse_pos.x;
		window_mouse_pos.y = ImGui::GetMousePos().y - mouse_pos.y - title_bar_height - window_offset.y;

		if (ImGui::Begin("Size Text"))
		{

			GetClientRect(locator::Locator::GetWindow()->GetHWND(), &rect);
			window_mouse_pos.x = ImGui::GetMousePos().x - mouse_pos.x;
			window_mouse_pos.y = ImGui::GetMousePos().y - mouse_pos.y - title_bar_height - window_offset.y;

			ImGui::Text("Win Mouse X : %f", window_mouse_pos.x);
			ImGui::Text("Win Mouse Y : %f", window_mouse_pos.y);

			ImGui::Text("Win Pos X : %f", window_pos.x);
			ImGui::Text("Win Pos Y : %f", window_pos.y);

			auto win_pos = ConvertWindowPos();
			ImGui::Text("Convert Win Pos X : %d", win_pos.x);
			ImGui::Text("Convert Win Pos Y : %d", win_pos.y);

			ImGui::Text("Mouse Pos X : %f", ImGui::GetMousePos().x);
			ImGui::Text("Mouse Pos Y : %f", ImGui::GetMousePos().y);

			ImGui::Text("Win Size X %f", window_size.x);
			ImGui::Text("Win Size Y %f", window_size.y);

			//DirectX::XMMatrixMultiply(ndc_pos, inv_proj);
			const DirectX::SimpleMath::Vector3 world_pos = arithmetic::ConvertScreenToWorld({ static_cast<float>(win_pos.x),static_cast<float>(win_pos.y),0 },
				GetSceneViewCamera().GetCamera().GetViewMat(),
				GetSceneViewCamera().GetCamera().GetProjectionMat());

			ImGui::Text("World X : %f", world_pos.x);
			ImGui::Text("World Y : %f", world_pos.y);
			ImGui::Text("World Z : %f", world_pos.z);


			//DirectX::XMMatrixMultiply(ndc_pos, inv_proj);
			const DirectX::SimpleMath::Vector3 world_pos_2 = arithmetic::ConvertScreenToWorld({ static_cast<float>(win_pos.x),static_cast<float>(win_pos.y),1.f },
				GetSceneViewCamera().GetCamera().GetViewMat(),
				GetSceneViewCamera().GetCamera().GetProjectionMat());

			ImGui::Text("World far X : %f", world_pos_2.x);
			ImGui::Text("World far Y : %f", world_pos_2.y);
			ImGui::Text("World far Z : %f", world_pos_2.z);

			//ImGui::Text("Selected : %s", selected_asset.string().c_str());
			ImGui::Text("Hovered %d", IsWindowHovered());
			ImGui::Text("Hover Event %d", mouse_hover.event);

			ImGui::Text("Dragging w pos X : %f", w_pos.x);
			ImGui::Text("Dragging w pos Y : %f", w_pos.y);
			ImGui::Text("Dragging w pos Z : %f", w_pos.z);
		}
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

	bool SceneView::IsWindowHovered()
	{
		const ImVec2 cursor_pos = ImGui::GetMousePos();
		if( cursor_pos.x < window_pos.x ||
			cursor_pos.x > window_pos.x + window_size.x ||
			cursor_pos.y < window_pos.y ||
			cursor_pos.y > window_pos.y + window_size.y)
		{
			mouse_hover.is_current_hovered = false;
			return false;
		}

		mouse_hover.is_current_hovered = true;
		return true;
	}

	void SceneView::AddModel(ecs::Registry* registry, const std::filesystem::path& file_path)
	{
		dragging_entity		= registry->CreateEntity();
		const auto model_id = locator::Locator::GetAssetManager()->GetAssetSheetManager().Search<asset::Model>(file_path);
		registry->AddComponent<component::ModelComponent>(dragging_entity, model_id);
	}

	void SceneView::AddPrefab(ecs::Registry* registry, const std::filesystem::path& file_path)
	{
		const auto prefab_id = locator::Locator::GetAssetManager()->GetAssetSheetManager().Search<asset::Prefab>(file_path);
		dragging_entity = locator::Locator::GetAssetManager()->GetLoader<asset::PrefabLoader>()->AddComponent(registry, prefab_id);
	}

	void SceneView::DraggingAsset(ecs::Registry* registry) const
	{
		const DirectX::XMINT2 win_pos = ConvertWindowPos();
		const DirectX::SimpleMath::Vector3 world_near_pos = arithmetic::ConvertScreenToWorld({ static_cast<float>(win_pos.x),static_cast<float>(win_pos.y),.0f },
																							 GetSceneViewCamera().GetCamera().GetViewMat(),
																							 GetSceneViewCamera().GetCamera().GetProjectionMat());
		const DirectX::SimpleMath::Vector3 world_far_pos = arithmetic::ConvertScreenToWorld({ static_cast<float>(win_pos.x),static_cast<float>(win_pos.y),1.f },
																							GetSceneViewCamera().GetCamera().GetViewMat(),
																							GetSceneViewCamera().GetCamera().GetProjectionMat());
		DirectX::SimpleMath::Vector3 hit_pos{};
		if(registry->GetScene()->GetCollisionManager()->IntersectRayVsDragModel(registry,world_near_pos,world_far_pos,&hit_pos))
		{
			registry->GetComponent<component::TransformComponent>(dragging_entity).SetPosition(hit_pos);
		}
		else
		{
			registry->GetComponent<component::TransformComponent>(dragging_entity).SetPosition(world_far_pos);
			w_pos = world_far_pos;
		}
	}

} // cumulonimbus::editor
