#include "wave_system.h"

#include "locator.h"
#include "scene_manager.h"
#include "model_component.h"

CEREAL_CLASS_VERSION(cumulonimbus::system::WaveSystem, 0)

namespace cumulonimbus::system
{
	template <class Archive>
	void WaveSystem::load(Archive&& archive, uint32_t version)
	{
		archive(
			CEREAL_NVP(waves)
		);
	}

	template <class Archive>
	void WaveSystem::save(Archive&& archive, uint32_t version) const
	{
		archive(
			CEREAL_NVP(waves)
		);
	}

	void WaveSystem::Update(const scene::SceneManager& scene_manager)
	{
		if (waves.empty())
			return;

		if (!scene_manager.GetActiveScenes().contains(waves.at(wave_index)))
			assert(!"A scene that doesn't exist(WaveSystem::Update)");


		if(ecs::Registry& registry = *scene_manager.GetScene(waves.at(wave_index)).GetRegistry();
		   registry.GetArray<component::ModelComponent>().Size() == 0)
		{// シーン内のModelComponentが存在しない場合次のウェーブに移行する
			ChangeWave();
		}
	}

	void WaveSystem::RenderImGui(ecs::Registry* registry)
	{

	}

	void WaveSystem::AddWave(const mapping::rename_type::UUID& scene_id)
	{
	}

	void WaveSystem::ChangeWave()
	{
	}
} // cumulonimbus::system
