#include "model_loader.h"

#include <set>

namespace cumulonimbus::asset
{
	void ModelLoader::Load(AssetSheetManager& sheet, const std::filesystem::path& path)
	{

	}

	//mapping::rename_type::UUID ModelLoader::Convert(const std::filesystem::path& path)
	//{
	//	return "";
	//}

	bool ModelLoader::Supported(std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extentions
		{
			".fbx",
			".FBX"
		};

		return extentions.contains(extension);
	}
} // cumulonimbus::asset
