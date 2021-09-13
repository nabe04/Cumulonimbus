#include "texture_loader.h"

#include <set>

#include "asset_sheet_manager.h"
#include "texture.h"
#include "locator.h"

namespace
{
	// テクスチャを保存するまでのパス
	const std::filesystem::path copy_dir = "./Data/Assets/Textures";
}

namespace cumulonimbus::asset
{
	mapping::rename_type::UUID TextureLoader::Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) const
	{
		// コピー先のフォルダ作成&コピー
		std::filesystem::copy(
			from, to,
			std::filesystem::copy_options::recursive |
			std::filesystem::copy_options::overwrite_existing);

		const std::string copy_str = to.string() + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		auto sheet = asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet;

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet)
		{
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.contains(id))
				continue;
			break;
		}

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.insert(std::make_pair(id, copy_path));
		return id;
	}

	void TextureLoader::Load(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, copy_dir);
		Load(asset_manager, id);
	}

	void TextureLoader::Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void TextureLoader::Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id)
	{
		// すでにテクスチャが存在する場合は処理を抜ける
		if (textures.contains(id))
			return;

		// テクスチャの作成
		textures.insert(std::make_pair(
			id,
			std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(),
				asset_manager.GetAssetSheetManager().GetAssetFilename<Texture>(id).c_str()))
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