#include "prefab_loader.h"

#include "ecs.h"
#include "arithmetic.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	PrefabLoader::PrefabLoader()
	{
	}

	void PrefabLoader::CreatePrefab(AssetManager& asset_manager,ecs::Registry* registry, const mapping::rename_type::Entity& ent)
	{
		const mapping::rename_type::UUID& id = utility::GenerateUUID();
		prefabs.emplace(id, std::make_unique<Prefab>());
		prefabs.at(id)->CreatePrefab(registry, ent);
	}
} // cumulonimbus::asset
