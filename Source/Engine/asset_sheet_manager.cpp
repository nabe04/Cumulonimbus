#include "asset_sheet_manager.h"

#include "material.h"
#include "model.h"
#include "texture.h"

namespace cumulonimbus::asset
{
	AssetSheetManager::AssetSheetManager()
	{
		Register<Material>();
		Register<Model>();
		Register<Texture>();
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
