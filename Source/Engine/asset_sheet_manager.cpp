#include "asset_sheet_manager.h"

#include "material.h"
#include "model.h"
#include "texture.h"
#include "prefab.h"
#include "scene_loader.h"

namespace cumulonimbus::asset
{
	AssetSheetManager::AssetSheetManager()
	{
		Register<Material>();
		Register<Model>();
		Register<Texture>();
		Register<Prefab>();
		Register<SceneAsset>();
	}

	std::string AssetSheetManager::GetAssetFilename(const mapping::rename_type::UUID& id) const
	{
		for (const auto& [hash, asset_sheet] : sheets)
		{
			for (const auto& [asset_id, asset_path] : asset_sheet.sheet)
			{
				if (asset_sheet.sheet.contains(id))
					return asset_sheet.sheet.at(id);
			}
		}

		return "";
	}

	mapping::rename_type::UUID AssetSheetManager::Search(const std::filesystem::path& path) const
	{
		for(const auto&[hash,asset_sheet] : sheets)
		{
			for(const auto&[id,asset_path] : asset_sheet.sheet)
			{
				if (path == asset_path)
					return id;
			}
		}
		return {};
	}

	void AssetSheetManager::Register(const mapping::rename_type::Hash& hash)
	{
		if (sheets.contains(hash))
			return;
		// �V�[�g�̓o�^
		sheets.emplace(hash, AssetSheet{});
	}

	template<class T>
	void AssetSheetManager::Register()
	{
		const auto id = utility::GetHash<T>();
		if (sheets.contains(id))
			return;
		// �V�[�g�̓o�^
		sheets.emplace(id, AssetSheet{});
	}
} // cumulonimbus::asset
