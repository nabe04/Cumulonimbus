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
		// �V�[�g�̓o�^
		sheets.insert(std::make_pair(id, AssetSheet{}));
	}
} // cumulonimbus::asset
