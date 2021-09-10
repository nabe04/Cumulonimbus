#include "texture_loader.h"

#include <set>

#include "texture.h"
#include "locator.h"

namespace cumulonimbus::asset
{
	TextureLoader::~TextureLoader()
	{
	}

	void TextureLoader::Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID id = Convert(path);

		// すでにテクスチャが存在する場合は処理を抜ける
		if (textures.contains(id))
			return;

		// テクスチャの作成
		textures.insert(std::make_pair(
			id,
			std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(),
									  sheet_manager.GetAssetFilename<Texture>(Convert(path)).c_str()))
		);
	}

	mapping::rename_type::UUID TextureLoader::Convert(const std::filesystem::path& path)
	{
		return "";
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