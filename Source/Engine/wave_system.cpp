#include "wave_system.h"

#include <filesystem>

#include "locator.h"
#include "scene_manager.h"
#include "scene_loader.h"
#include "model_component.h"

CEREAL_CLASS_VERSION(cumulonimbus::system::WaveSystem, 0)

namespace cumulonimbus::system
{
	template <class Archive>
	void WaveSystem::load(Archive&& archive, uint32_t version)
	{
		archive(
			CEREAL_NVP(waves_id)
		);
	}

	template <class Archive>
	void WaveSystem::save(Archive&& archive, uint32_t version) const
	{
		archive(
			CEREAL_NVP(waves_id)
		);
	}

	void WaveSystem::Start()
	{
		wave_index = 0;
	}

	void WaveSystem::Update(const scene::SceneManager& scene_manager)
	{
		if (waves_id.empty())
			return;

		if (!scene_manager.GetActiveScenes().contains(waves_id.at(wave_index)))
			//assert(!"A scene that doesn't exist(WaveSystem::Update)");
			return;

		if(ecs::Registry& registry = *scene_manager.GetScene(waves_id.at(wave_index)).GetRegistry();
		   registry.GetArray<component::ModelComponent>().Size() == 0)
		{// シーン内のModelComponentが存在しない場合次のウェーブに移行する
			ChangeWave();
		}
	}

	void WaveSystem::RenderImGui(ecs::Registry* registry)
	{
		if (const ImGuiTreeNodeFlags tree_flg{ ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth };
			ImGui::CollapsingHeader(ICON_FA_WATER"Wave System", tree_flg))
		{
			const auto& scene_asset = locator::Locator::GetAssetManager()->GetAssetSheetManager().GetSheet<asset::SceneAsset>();
			const auto& scene_loader = locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>();

			std::vector<mapping::rename_type::UUID> scenes_id{};
			std::vector<std::string> scenes_name{};
			scenes_id.reserve(scene_asset.sheet.size());
			for (const auto& [scene_id, scene_file_path] : scene_asset.sheet)
			{
				scenes_id.emplace_back(scene_id);
				scenes_name.emplace_back(scene_loader->Name(scene_id));
			}

			ImGui::Text("Add Wave");
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PLUS))
			{
				waves_id.emplace_back();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_MINUS))
			{
				if (!waves_id.empty())
				{
					waves_id.pop_back();
				}
			}

			for (u_int wave_no = 0;
				auto & wave_id: waves_id)
			{
				//if (!scene_asset.sheet.contains(wave_id))
				//	continue;
				//const std::string wave_name = scene_asset.sheet.at(wave_id);

				if (std::string scene_name = scene_loader->Name(wave_id);
					helper::imgui::Combo("Wave" + std::to_string(wave_no + 1), scene_name, scenes_name))
				{
					for (int i = 0; i < scenes_name.size(); ++i)
					{
						if (scene_name == scenes_name.at(i))
						{
							wave_id = scenes_id.at(i);
							break;
						}
					}
				}
				++wave_no;
			}
		}
	}

	void WaveSystem::AddWave(const mapping::rename_type::UUID& scene_id)
	{
		waves_id.emplace_back(scene_id);
	}

	void WaveSystem::ChangeWave()
	{
	}
} // cumulonimbus::system
