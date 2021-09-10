#include "asset_sheet_manager.h"

namespace cumulonimbus::asset
{
	template <class Archive>
	void AssetSheet::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(sheet)
		);
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

	void AssetSheetManager::Register(std::map<mapping::rename_type::Hash, std::string>& sheet)
	{

	}


} // cumulonimbus::asset
