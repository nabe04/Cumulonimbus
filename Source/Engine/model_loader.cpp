#include "model_loader.h"

#include <set>

namespace
{
	// テクスチャを保存するまでのパス
	const std::filesystem::path copy_dir = "./Data/Assets/Models";
}


namespace cumulonimbus::asset
{
	void ModelLoader::Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path)
	{
		const auto id = Convert<Model>(sheet_manager, path, copy_dir);
		Load(sheet_manager, id);
	}

	void ModelLoader::Load(AssetSheetManager& sheet_manager, const std::filesystem::path& from, const std::filesystem::path& to)
	{
		const auto id = Convert<Model>(sheet_manager, from, to);
		Load(sheet_manager, id);
	}

	void ModelLoader::Load(AssetSheetManager& sheet_manager, const mapping::rename_type::UUID& id)
	{

	}

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
