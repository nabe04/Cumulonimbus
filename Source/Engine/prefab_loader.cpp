#include "prefab_loader.h"

#include "ecs.h"
#include "arithmetic.h"
#include "generic.h"
#include "locator.h"

namespace
{
	const std::string save_parent_path{ "./Data/Assets/Prefabs" };
}

namespace cumulonimbus::asset
{
	template <class Archive>
	void PrefabLoader::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(prefabs)
		);
	}

	PrefabLoader::PrefabLoader()
	{
	}

	void PrefabLoader::CreatePrefab(
		AssetManager& asset_manager, ecs::Registry* registry,
		const mapping::rename_type::Entity& ent,
		const std::string& prefab_name)
	{

		std::filesystem::path save_path{};
		if (prefab_name.empty())
		{
			const std::string path = save_parent_path + "/" + "New Prefab" + file_path_helper::GetPrefabExtension();
			save_path = CompareAndReName<Prefab>(asset_manager, path);
		}
		else
		{
			const std::string path = save_parent_path + "/" + prefab_name + file_path_helper::GetPrefabExtension();
			save_path = CompareAndReName<Prefab>(asset_manager, path);
		}

		std::unique_ptr<Prefab> prefab = std::make_unique<Prefab>();
		prefab->CreatePrefab(registry, ent, save_path);


		//const mapping::rename_type::UUID& id = utility::GenerateUUID();
		//prefabs.emplace(id, std::make_unique<Prefab>());
		//prefabs.at(id)->CreatePrefab(registry, ent);
	}
} // cumulonimbus::asset
