#include "texture_loader.h"

#include <set>
#include <filesystem>

#include "asset_sheet_manager.h"
#include "texture.h"
#include "locator.h"
//
//CEREAL_REGISTER_TYPE(cumulonimbus::asset::TextureLoader)
//CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::asset::Loader, cumulonimbus::asset::TextureLoader)


namespace
{
	// テクスチャを保存するまでのパス
	const std::filesystem::path copy_dir = "./Data/Assets/Textures";
}

namespace cumulonimbus::asset
{
	TextureLoader::TextureLoader()
	{
		const DirectX::SimpleMath::Vector4 color = { 1.f,1.f,1.f,1.f };
		dummy_texture = std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(), color);
	}

	mapping::rename_type::UUID TextureLoader::Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to)
	{
		//const auto filename = std::filesystem::path{ from }.filename().replace_extension().string();
		//std::filesystem::create_directory(to.string() + "/" + filename);
		//const auto to_path = to.string() + "/" + filename;
		if (!equivalent(from.parent_path(), to))
		{
			// コピー先のフォルダ作成&コピー
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}


		const std::string copy_str = to.string() + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

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
		asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.insert(std::make_pair(id, copy_path.string()));
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
		// アセットシート(更新後)の保存
		asset_manager.Save();
	}

	void TextureLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID tex_id = asset_manager.GetAssetSheetManager().Search<Texture>(path);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!textures.contains(tex_id))
			return;

		textures.erase(tex_id);
	}

	bool TextureLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".png" , ".PNG",
			".jpeg", ".JPEG",
			".tga" , ".TGA",
			".dds" , ".DDS"
		};
		return extensions.contains(extension);
	}

	void TextureLoader::CreateTexture(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet)
		{
			if (path.compare(value) == 0)
				return;
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
		asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.insert(std::make_pair(id, path.string()));

		// すでにテクスチャが存在する場合は処理を抜ける
		if (textures.contains(id))
			return;

		// テクスチャの作成
		textures.insert(std::make_pair(
			id,
			std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(),
				asset_manager.GetAssetSheetManager().GetAssetFilename<Texture>(id).c_str()))
		);
		// アセットシート(更新後)の保存
		asset_manager.Save();
	}

	Texture& TextureLoader::GetTexture(const mapping::rename_type::UUID& id)
	{
		if (!textures.contains(id))
			return *dummy_texture.get();
			//assert(!"Not found texture id(TextureLoader::GetTexture)");

		return *textures.at(id).get();
	}
} // cumulonimbus::asset