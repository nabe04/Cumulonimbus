#include "system_inspector.h"

#include "ecs.h"
#include "locator.h"
#include "texture_loader.h"
#include "cum_imgui_helper.h"
#include "scene.h"

namespace cumulonimbus::editor
{
	void SystemInspector::Update(const float dt)
	{
		//-- CameraComponentのカメラテクスチャ用設定 --//
		//camera_texture.scaling_matrix = DirectX::SimpleMath::Matrix::CreateScale(camera_texture.tex_size.x, camera_texture.tex_size.y, 1.f);
	}

	void SystemInspector::Render(ecs::Registry* registry)
	{
		if (ImGui::Begin(ICON_FA_GLOBE"System"))
		{
			//registry->GetScene()->GetCollisionManager()->RenderImGui(registry);

			//if (ImGui::CollapsingHeader(ICON_FA_VIDEO"Camera Setting", ImGuiTreeNodeFlags_DefaultOpen))
			//{
			//	auto* asset_manager = locator::Locator::GetAssetManager();
			//	auto* texture_loader = asset_manager->GetLoader<asset::TextureLoader>();
			//	// テクスチャ選択
			//	texture_loader->SelectableTexture(*asset_manager, camera_texture.tex_id);
			//	// テクスチャサイズ設定
			//	IMGUI_LEFT_LABEL(ImGui::DragFloat2, "Texture Size", reinterpret_cast<float*>(&camera_texture.tex_size), .5f, 1.f, 100000.f);
			//}

			locator::Locator::GetSystem()->Render(registry);
		}
		ImGui::End();
	}

} // cumulonimbus::editor
