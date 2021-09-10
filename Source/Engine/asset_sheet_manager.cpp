#include "asset_sheet_manager.h"

#include "texture.h"

namespace cumulonimbus::asset
{
	template <class Archive>
	void AssetSheet::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(sheet)
		);
	}

	AssetSheetManager::AssetSheetManager()
	{
		Register<Texture>();
	}

	template <class Archive>
	void AssetSheetManager::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(sheets)
		);
	}


	void AssetSheetManager::AddAsset(const std::string& filename)
	{
		//asset_type.insert(std::make_pair(AssetType::Model, std::bind(&AssetSheet::Register, this, model_sheet)));

	}

	template<typename T>
	void AssetSheetManager::Register()
	{
		const auto id = utility::GetHash<T>();
		if (sheets.contains(id))
			return;

		sheets.insert(std::make_pair(id, AssetSheet{}));
	}


} // cumulonimbus::asset
