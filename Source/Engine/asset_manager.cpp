#include "asset_manager.h"

#include "asset_sheet_manager.h"
#include "texture.h"
#include "texture_loader.h"

namespace cumulonimbus::asset
{
	AssetManager::AssetManager()
	{
		sheet_manager = std::make_unique<AssetSheetManager>();
		RegisterLoader<TextureLoader>();
	}

	void AssetManager::AddAsset(const std::filesystem::path& path)
	{
		for (auto&& [key, value] : loaders)
		{
			if (value->Supported(path.extension()))
				value->Load(*this, path);
		}
	}
} // cumulonimbus::asset
