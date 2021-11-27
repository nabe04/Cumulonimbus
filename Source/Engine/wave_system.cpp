#include "wave_system.h"

#include <filesystem>

#include "locator.h"
#include "scene_manager.h"
#include "scene_loader.h"
#include "model_component.h"

CEREAL_CLASS_VERSION(cumulonimbus::system::WaveSystem, 0)

namespace
{
	const std::string default_path_and_name{ "./Data/System/wave_system" };
}


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

	void WaveSystem::Initialize(scene::SceneManager& scene_manager)
	{
		wave_state.AddState(WaveState::Start , [&](scene::SceneManager& sm) {StartGame(sm); });
		wave_state.AddState(WaveState::Update, [&](scene::SceneManager& sm) {UpdateGame(sm); });
		wave_state.AddState(WaveState::Change, [&](scene::SceneManager& sm) {ChangeWave(sm); });

		wave_state.SetState(WaveState::Update);

		ChangeWave(scene_manager, true);
	}

	void WaveSystem::Start(scene::SceneManager& scene_manager)
	{
		//ChangeWave(scene_manager, true);
		Initialize(scene_manager);
	}

	void WaveSystem::End(scene::SceneManager& scene_manager)
	{
		std::vector<mapping::rename_type::UUID> delete_scene_ids{};
		for(const auto& wave_id : waves_id)
		{
			for(const auto& scene_id : scene_manager.GetActiveScenes() | std::views::keys)
			{
				if(wave_id != scene_id)
					continue;;

				delete_scene_ids.emplace_back(wave_id);
			}
		}

		for(const auto& delete_scene_id : delete_scene_ids)
		{
			scene_manager.DeleteScene(delete_scene_id);
		}
	}

	void WaveSystem::Update(scene::SceneManager& scene_manager)
	{
		if (waves_id.empty())
			return;

		if (wave_index >= waves_id.size())
			return;

		if (!scene_manager.GetActiveScenes().contains(waves_id.at(wave_index)))
			//assert(!"A scene that doesn't exist(WaveSystem::Update)");
			return;

		wave_state.Update(scene_manager);
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

	void WaveSystem::Load(const std::filesystem::path& load_path)
	{
		static const std::set<std::filesystem::path> extension
		{
			file_path_helper::GetWaveSystemExtension()
		};

		std::filesystem::create_directory(std::filesystem::path{ default_path_and_name }.parent_path());

		// デフォルトのフォルダにある「.wsys」の読み込み
		if(load_path.empty())
		{
			std::filesystem::path default_path{};
			for(const auto paths = utility::GetAllSubDirectoryFilePath(std::filesystem::path{ default_path_and_name }.parent_path().string());
				const auto& p :paths)
			{
				default_path = p;
				if (!extension.contains(default_path.extension()))
					continue;
				{
					std::ifstream ifs(default_path, std::ios_base::binary);
					if (!ifs)
						assert(!"Not found file(WaveSystem::Load)");
					cereal::BinaryInputArchive input_archive(ifs);
					input_archive(*this);
				}
				return;
			}
			// ファイルパスが存在しなかったので現在の情報で保存
			Save();
			return;
		}

		// 任意のフォルダからの読み込み
		if (!extension.contains(load_path.extension()))
			assert(!"The extension is incorrect(WaveSystem::Load)");
		{
			std::ifstream ifs(load_path);
			if (!ifs)
				assert(!"Not found file(WaveSystem::Load)");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(*this);
		}
	}

	void WaveSystem::Save(const std::filesystem::path& save_path)
	{
		if(save_path.empty())
		{// デフォルトパスに保存
			std::ofstream ofs(default_path_and_name + file_path_helper::GetWaveSystemExtension(), std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
		else
		{
			std::ofstream ofs(save_path, std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
	}

	void WaveSystem::RegisterEventChanged(const mapping::rename_type::UUID& id, const std::function<void()>& func)
	{
		on_wave_changed.RegistryEvent(id, func);
	}

	void WaveSystem::AddWave(const mapping::rename_type::UUID& scene_id)
	{
		waves_id.emplace_back(scene_id);
	}

	void WaveSystem::ChangeWave(scene::SceneManager& scene_manager, const bool is_first)
	{
		++wave_index;
		if(is_first)
		{
			wave_index = 0;
			if (waves_id.size() <= 0)
				return;
		}

		if (waves_id.empty())
			return;
		if (wave_index >= waves_id.size())
			return;

		auto& scene_loader = *locator::Locator::GetAssetManager()->GetLoader<asset::SceneLoader>();

		while(true)
		{
			if (wave_index >= waves_id.size())
				break;

			if(!scene_loader.HasScene(waves_id.at(wave_index)))
			{
				++wave_index;
				continue;
			}

			if (wave_index > 0)
				scene_loader.DestroyScene(scene_manager, waves_id.at(wave_index - 1));

			// ウェーブに登録されているシーンの追加読み込み
			scene_loader.AddScene(scene_manager, waves_id.at(wave_index));
			// シーンの開始処理
			scene_manager.GetScene(waves_id.at(wave_index)).StartGame();
			// ウェーブの切り替わり時の登録関数を全て実行
			on_wave_changed.InvokeAll();
			break;
		}
	}

	void WaveSystem::StartGame(scene::SceneManager& scene_manager)
	{
		ChangeWave(scene_manager, true);
		wave_state.SetState(WaveState::Update);
	}

	void WaveSystem::UpdateGame(scene::SceneManager& scene_manager)
	{
		if (ecs::Registry& registry = *scene_manager.GetScene(waves_id.at(wave_index)).GetRegistry();
			registry.GetArray<component::ModelComponent>().Size() == 0)
		{// シーン内のModelComponentが存在しない場合次のウェーブに移行する
			wave_state.SetState(WaveState::Change);
		}
	}

	void WaveSystem::ChangeWave(scene::SceneManager& scene_manager)
	{
		ChangeWave(scene_manager, false);
		wave_state.SetState(WaveState::Update);
	}

} // cumulonimbus::system
