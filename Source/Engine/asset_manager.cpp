#include "asset_manager.h"

#include "asset_sheet_manager.h"
#include "texture.h"
#include "texture_loader.h"

namespace cumulonimbus::asset
{
	AssetManager::AssetManager()
	{
		RegisterLoader<TextureLoader>();
	}

	

} // cumulonimbus::asset
