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
			// 保存先のパスを指定(既に存在していた場合は名前を変更する)
			const std::string path =  save_parent_path + "/" +
									  "New Prefab" + "/" +
									  "New Prefab" +
									  file_path_helper::GetPrefabExtension();
			// 保存するファイル名の決定
			const std::string save_name  = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// 保存先のパス(ファイル名、拡張子含む)決定
			save_path = save_parent_path + "/" +
						save_name + "/" +
						save_name +
						file_path_helper::GetPrefabExtension();
		}
		else
		{
			// 保存先のパスを指定(既に存在していた場合は名前を変更する)
			const std::string path = save_parent_path + "/" +
									 prefab_name + "/" +
									 prefab_name +
									 file_path_helper::GetPrefabExtension();
			// 保存するファイル名の決定
			const std::string save_name = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// 保存先のパス(ファイル名、拡張子含む)決定
			save_path = save_parent_path + "/" +
						save_name + "/" +
						save_name +
						file_path_helper::GetPrefabExtension();
		}

		std::unique_ptr<Prefab> prefab = std::make_unique<Prefab>();
		prefab->CreatePrefab(registry, ent, save_path);
	}
} // cumulonimbus::asset
