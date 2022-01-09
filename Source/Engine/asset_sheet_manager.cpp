#include "asset_sheet_manager.h"

#include "effect.h"
#include "material.h"
#include "model.h"
#include "prefab.h"
#include "scene_loader.h"
#include "texture.h"

namespace cumulonimbus::asset
{
	AssetSheetManager::AssetSheetManager()
	{
		Register<Material>();
		Register<Model>();
		Register<Texture>();
		Register<Prefab>();
		Register<SceneAsset>();
		Register<Effect>();
	}

	std::string AssetSheetManager::GetAssetFilename(const mapping::rename_type::UUID& id) const
	{
		for (const auto& asset_sheet : sheets | std::views::values)
		{
			for (const auto& [asset_id, asset_path] : asset_sheet.sheet)
			{
				if (asset_sheet.sheet.contains(id))
					return asset_sheet.sheet.at(id);
			}
		}

		return "";
	}

	std::string AssetSheetManager::GetAssetName(const mapping::rename_type::UUID& id)
	{
		for (const auto& asset_sheet : sheets | std::views::values)
		{
			for (const auto& [asset_id, asset_path] : asset_sheet.sheet)
			{
				if (asset_sheet.sheet.contains(id))
					return std::filesystem::path{ asset_sheet.sheet.at(id) }.filename().relative_path().string();
			}
		}

		assert(!"Don't have asset id(AssetSheetManager::GetAssetName)");
		return "";
	}


	mapping::rename_type::UUID AssetSheetManager::Search(const std::filesystem::path& path) const
	{
		for(const auto& asset_sheet : sheets | std::views::values)
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
		// ÉVÅ[ÉgÇÃìoò^
		sheets.emplace(hash, AssetSheet{});
	}

	template<class T>
	void AssetSheetManager::Register()
	{
		const auto id = utility::GetHash<T>();
		if (sheets.contains(id))
			return;
		// ÉVÅ[ÉgÇÃìoò^
		sheets.emplace(id, AssetSheet{});
	}
} // cumulonimbus::asset
