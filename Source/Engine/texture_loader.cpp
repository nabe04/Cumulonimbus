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
	// �e�N�X�`����ۑ�����܂ł̃p�X
	const std::filesystem::path copy_dir = "./Data/Assets/Textures";
}

namespace cumulonimbus::asset
{
	mapping::rename_type::UUID TextureLoader::Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to)
	{
		// �R�s�[��̃t�H���_�쐬&�R�s�[
		std::filesystem::copy(
			from, to,
			std::filesystem::copy_options::recursive |
			std::filesystem::copy_options::overwrite_existing);

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

		// �A�Z�b�g�V�[�g�̓o�^
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
		// ���łɃe�N�X�`�������݂���ꍇ�͏����𔲂���
		if (textures.contains(id))
			return;

		// �e�N�X�`���̍쐬
		textures.insert(std::make_pair(
			id,
			std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(),
				asset_manager.GetAssetSheetManager().GetAssetFilename<Texture>(id).c_str()))
		);
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
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

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Texture>().sheet.insert(std::make_pair(id, path.string()));

		// ���łɃe�N�X�`�������݂���ꍇ�͏����𔲂���
		if (textures.contains(id))
			return;

		// �e�N�X�`���̍쐬
		textures.insert(std::make_pair(
			id,
			std::make_unique<Texture>(locator::Locator::GetDx11Device()->device.Get(),
				asset_manager.GetAssetSheetManager().GetAssetFilename<Texture>(id).c_str()))
		);
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
	}

} // cumulonimbus::asset