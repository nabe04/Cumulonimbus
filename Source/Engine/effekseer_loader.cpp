#include "effekseer_loader.h"

#include <set>
#include <filesystem>

#include "locator.h"

namespace
{
	// �G�t�F�N�g��ۑ�����܂ł̃p�X
	const std::filesystem::path save_parent_path = "./Data/Assets/Effects";
}

namespace cumulonimbus::asset
{
	void EffekseerLoader::Load(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, save_parent_path);
		Load(asset_manager, id);
	}

	void EffekseerLoader::Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void EffekseerLoader::Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id)
	{
		// ���łɃe�N�X�`�������݂���ꍇ�͏����𔲂���
		if (effects.contains(id))
			return;

		// �e�N�X�`���̍쐬
		effects.insert(std::make_pair(
			id,
			std::make_unique<Effect>(asset_manager.GetAssetSheetManager().GetAssetFilename<Effect>(id).c_str()))
		);
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
	}

	void EffekseerLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id,
		const std::string& changed_name)
	{
		// �A�Z�b�g�����݂��Ȃ��ꍇ�����𔲂���
		if (!effects.contains(asset_id))
			return;

		// �e�N�X�`���g���q
		const std::string extension = std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Effect>(asset_id) }.extension().string();
		// �ύX�O�̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Textures/"�ύX�O�̃e�N�X�`����"
		const std::filesystem::path before_path = std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Effect>(asset_id) }.replace_extension();

		const std::string asset_name = CompareAndReName<Effect>(asset_manager,
			before_path.parent_path().string() + "/" + changed_name + extension
			).filename().replace_extension().string();

		// �ύX��̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Textures/"�ύX��̃e�N�X�`����"
		const std::filesystem::path after_path = before_path.parent_path().string() + "/" + asset_name;

		// �t�@�C�����̕ύX
		// �� : ./Data/Effects/"�G�t�F�N�g�t�@�C����"/"�ύX�O�̃G�t�F�N�g��".�g���q -> ./Data/Textures/"�ύX��̃e�N�X�`����".�g���q
		std::filesystem::rename(before_path.string() + extension,
			after_path.string() + extension);

		// �A�Z�b�g�V�[�g���̃t�@�C���p�X�ύX(�� : ./Data/Materials/"�ύX��̃}�e���A����"/"�ύX��̃}�e���A����.mat")
		asset_manager.GetAssetSheetManager().GetSheet<Effect>().sheet.at(asset_id) = after_path.string() + extension;
		// �A�Z�b�g�V�[�g�̕ۑ�
		asset_manager.Save();
	}

	void EffekseerLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID efk_id = asset_manager.GetAssetSheetManager().Search<Effect>(path);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!effects.contains(efk_id))
			return;

		DeleteEffect(efk_id, path);
		asset_manager.Save();
	}

	void EffekseerLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!effects.contains(asset_id))
			return;

		const std::filesystem::path path = asset_manager.GetAssetSheetManager().GetAssetFilename<Effect>(asset_id);
		DeleteEffect(asset_id, path);
		asset_manager.Save();
	}

	bool EffekseerLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".efk",".efkfc"
		};
		return extensions.contains(extension);
	}

	mapping::rename_type::UUID EffekseerLoader::Convert(AssetManager& asset_manager, const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto filename = std::filesystem::path{ from }.filename().replace_extension().string();
		std::filesystem::create_directory(to.string());
		std::filesystem::create_directory(to.string() + "/" + filename);
		const auto to_path = to.string() + "/" + filename;
		if (!equivalent(from.parent_path(), to_path))
		{
			// �R�s�[��̃t�H���_�쐬&�R�s�[
			std::filesystem::copy(
				from.parent_path(), to_path,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		const std::string copy_str = to_path + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Effect>().sheet)
		{// �����A�Z�b�g�����݂��Ă����ꍇ���̃A�Z�b�g��ID(UUID)��Ԃ�
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{// ID(UUID)�̏d�����Ȃ����m�F
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Effect>().sheet.contains(id))
				continue;
			break;
		}

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Effect>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	void EffekseerLoader::DeleteEffect(const mapping::rename_type::UUID& efk_id, const std::filesystem::path& delete_path)
	{
		// �G�t�F�N�g�A�Z�b�g�̍폜
		effects.erase(efk_id);
		// �t�@�C���̍폜
		std::filesystem::remove(delete_path.parent_path());
	}

} // cumulonimbus::asset
