#include "asset_sheet_manager.h"

#include "material.h"
#include "model.h"
#include "texture.h"
#include "prefab.h"

namespace cumulonimbus::asset
{
	AssetSheetManager::AssetSheetManager()
	{
		Register<Material>();
		Register<Model>();
		Register<Texture>();
		Register<Prefab>();
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

	template<class T>
	void AssetSheetManager::Register()
	{
		const auto id = utility::GetHash<T>();
		if (sheets.contains(id))
			return;
		// ƒV[ƒg‚Ì“o˜^
		sheets.insert(std::make_pair(id, AssetSheet{}));
	}
} // cumulonimbus::asset
