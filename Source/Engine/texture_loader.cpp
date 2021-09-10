#include "texture_loader.h"

#include <set>

#include "texture.h"
#include "locator.h"

namespace
{
	// テクスチャを保存するまでのパス
	const std::filesystem::path copy_dir = "./Data/Assets/Textures";
}

namespace cumulonimbus::asset
{
	void TextureLoader::Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path)
	{
		auto id = Convert<Texture>(sheet_manager, path, copy_dir);

		// すでにテクスチャが存在する場合は処理を抜ける
		if (textures.contains(id))
			return;

		// テクスチャの作成
		textures.insert(std::make_pair(
			id,
			std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(),
									  sheet_manager.GetAssetFilename<Texture>(id).c_str()))
		);
	}

	bool TextureLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".png", ".PNG",
			".jpeg",".JPEG",
			".tga", ".TGA",
			".dds", ".DDS"
		};
		return extensions.contains(extension);
	}
} // cumulonimbus::asset