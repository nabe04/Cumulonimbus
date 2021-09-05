#include "content_browser.h"

#include "locator.h"
#include "resource_manager.h"
#include "scene.h"

#include "mesh_object.h"
#include "fbx_model_component.h"

void ContentBrowser::Render(Scene* scene)
{
	auto* model_resource = cumulonimbus::locator::Locator::GetResourceManager();

	{
		ImGuiWindowFlags window_flags{};
		window_flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;
		bool is_open = true;;
		ImGui::Begin("Content", &is_open, window_flags);

		{// コンテンツの表示(左)
			ImGuiWindowFlags flag = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::BeginChild("ChildL", ImVec2{ ImGui::GetWindowContentRegionWidth() * 0.25f, 300 }, true, flag);

			ImGui::Text("Content");
			ImGui::Separator();

			static int selected = -1;

			for (int n = 0; n < contents.size(); ++n)
			{
				is_selected.reset(n);

				char buf[100];
				sprintf_s(buf, contents.at(n).c_str());
				if (ImGui::Selectable(buf, selected == n))
				{
					selected = n;
				}
				if(selected != -1)
					is_selected.set(selected);
			}

			//for (int n = 0; n < model_resource.FbxModelNames().size(); ++n)
			//{
			//	char buf[100];
			//	sprintf_s(buf, model_resource.FbxModelNames().at(n).c_str());
			//	if (ImGui::Selectable(buf, selected == n))
			//		selected = n;
			//}

			ImGui::EndChild();
		}

		ImGui::SameLine();

		{// コンテンツの詳細表示(右)
			ImGuiWindowFlags flag = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::BeginChild("ChildR", ImVec2{ ImGui::GetWindowContentRegionWidth() * 0.73f, 300 }, true, flag);

			//static int selected = -1;
			//for (int n = 0; n < model_resource.FbxModelNames().size(); ++n)
			//{
			//	char buf[100];
			//	sprintf_s(buf, model_resource.FbxModelNames().at(n).c_str());
			//	if (ImGui::Selectable(buf, selected == n))
			//		selected = n;
			//}

			SelectedContent(scene);

			ImGui::EndChild();
		}

		ImGui::End();
	}
}

void ContentBrowser::SelectedContent(Scene* scene)
{
	if (is_selected.test(Contents::FBX))
	{
		for (int n = 0; n < cumulonimbus::locator::Locator::GetResourceManager()->FbxModelNames().size(); ++n)
		{
			ImGui::PushID(n);
			if (ImGui::ImageButton((void*)cumulonimbus::locator::Locator::GetResourceManager()->GetTextureResource("no_image_256")->GetTextureData()->texture_view.Get(), { 150,150 }))
			{

			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{// 選択されているモデルの追加
				std::string model_name = cumulonimbus::locator::Locator::GetResourceManager()->FbxModelNames().at(n);
				//auto* entity = scene->AddEntity();
				//entity->AddComponent<MeshObject>();
				//entity->AddComponent<FbxModelComponent>(cumulonimbus::locator::Locator::GetResourceManager()->FbxModelResouece(model_name));
			}

			//ImGui::Text(cumulonimbus::locator::Locator::GetResourceManager().FbxModelNames().at(n).c_str());
			ImGui::SameLine();

			// Our buttons are both drag sources and drag targets here!
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Set payload to carry the index of our item (could be anything)
				ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

				// Display preview (could be anything, e.g. when dragging an image we could decide to display
				// the filename and a small preview of the image, etc.)
				// 恐らくImGui上でDragしている時に出てくるテキスト
				// ここに保存したテクスチャを指定すると出てくると思う

				{// ドラック時に出てくるテクスチャ
					ImGui::Image((void*)cumulonimbus::locator::Locator::GetResourceManager()->GetTextureResource("no_image_256")->GetTextureData()->texture_view.Get(), { 150,150 });
					ImGui::Text(cumulonimbus::locator::Locator::GetResourceManager()->FbxModelNames().at(n).c_str());
				}

				POINT po{};
				GetCursorPos(&po);

				// 最終目標
				if (cumulonimbus::locator::Locator::GetWindow()->IsWithinWindow({ po.x,po.y }))
				{// モデルの追加

				}

				ImGui::EndDragDropSource();
			}
			ImGui::PopID();
		}
	}
}