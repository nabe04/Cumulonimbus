#include "prefab_loader.h"

#include <ranges>

#include "ecs.h"
#include "arithmetic.h"
#include "generic.h"
#include "locator.h"

namespace
{
	const std::string save_parent_path{ "./Data/Assets/Prefabs" };
}

namespace cumulonimbus::asset
{
	mapping::rename_type::UUID PrefabLoader::Convert(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		if (!equivalent(from.parent_path(), to))
		{
			// �R�s�[��̃t�H���_�쐬&�R�s�[
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		const std::string copy_str = to.string() + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet)
		{// �����A�Z�b�g�����݂��Ă����ꍇ���̃A�Z�b�g��ID(UUID)��Ԃ�
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{// ID(UUID)�̏d�����Ȃ����m�F
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(id))
				continue;
			break;
		}

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	void PrefabLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, save_parent_path);
		Load(asset_manager, id);
	}

	void PrefabLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void PrefabLoader::Load(
		AssetManager& asset_manager,
		const mapping::rename_type::UUID& id)
	{
		// ���łɃv���t�@�u�����݂���ꍇ�͏����𔲂���
		if (prefabs.contains(id))
			return;
		const std::string& asset_path = asset_manager.GetAssetSheetManager().GetAssetFilename<Prefab>(id);
		// Prefab�쐬
		std::unique_ptr<Prefab> prefab = std::make_unique<Prefab>();
		prefab->Load(asset_path);

		prefabs.emplace(id, std::move(prefab));
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
	}

	void PrefabLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name)
	{
		// �A�Z�b�g�����݂��Ȃ��ꍇ�����𔲂���
		if (!prefabs.contains(asset_id))
			return;

		// �ύX�O�̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Assets/Prefabs/"�ύX�O�̃v���n�u��"/"�ύX�O�̃v���n�u��"
		const std::filesystem::path	before_path = std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<Prefab>(asset_id) }.replace_extension();
		// �ύX�O�̐e�t�@�C���p�X(�g���q���܂܂Ȃ�) ./Data/Assets/Prefabs/"�ύX�O�̃v���n�u��"
		const std::filesystem::path before_parent_path = before_path.parent_path();

		// �d���m�F�ς݂̃t�@�C�����擾(�g���q���܂܂Ȃ�)
		// asset_name = �d���m�F�ς݂̃V�[����(�t�@�C�����̂݁A�g���q���܂܂Ȃ�)
		const std::string asset_name = CompareAndReName<Prefab>(asset_manager,
																before_parent_path.parent_path().string() + "/" +
																changed_name + "/" + changed_name +
																file_path_helper::GetPrefabExtension()
																).filename().replace_extension().string();

		// �ύX��̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Assets/Prefabs/"�ύX��̃v���n�u��"/"�ύX��̃v���n�u��"
		const std::filesystem::path after_path = before_parent_path.parent_path().string() + "/" + asset_name + "/" + asset_name;
		//
		// -- �t�@�C�� & �t�H���_���̕ύX --//
		// �u.prefab�v�t�@�C���̃t�@�C�����ύX
		// �� : ./Data/Assets/Prefabs/"�ύX�O�̃v���n�u��"/"�ύX�O�̃v���n�u��.prefab" -> ./Data/Assets/Prefabs/"�ύX�O�̃v���n�u��"/"�ύX��̃v���n�u��".prefab
		std::filesystem::rename(before_path.string()		+ file_path_helper::GetPrefabExtension(),
								before_parent_path.string() + "/" + asset_name + file_path_helper::GetPrefabExtension());
		// �t�H���_���̕ύX(�v���n�u�t�H���_)
		// �� : ./Data/Assets/"�ύX�O�̃v���n�u��" -> ./Data/Assets/"�ύX��̃v���n�u��"
		std::filesystem::rename(before_parent_path, after_path.parent_path());

		// �A�Z�b�g�V�[�g���̃t�@�C���p�X�ύX(�� : ./Data/Materials/"�ύX��̃v���n�u��"/"�ύX��̃v���n�u��.mat")
		asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.at(asset_id) = after_path.string() + file_path_helper::GetPrefabExtension();
		// �A�Z�b�g�V�[�g�̕ۑ�
		asset_manager.Save();
	}

	void PrefabLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID prefab_id = asset_manager.GetAssetSheetManager().Search<Prefab>(path);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!prefabs.contains(prefab_id))
			return;

		DeletePrefab(prefab_id, path);
		asset_manager.Save();
	}

	void PrefabLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
		const std::filesystem::path path = asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Prefab>(asset_id);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!prefabs.contains(asset_id))
			return;

		DeletePrefab(asset_id, path);
		asset_manager.Save();
	}

	bool PrefabLoader::Supported(std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			file_path_helper::GetPrefabExtension()
		};

		return extensions.contains(extension);
	}

	mapping::rename_type::UUID PrefabLoader::CreatePrefab(
		AssetManager& asset_manager, ecs::Registry* registry,
		const mapping::rename_type::Entity& ent,
		const bool is_once, const std::string& prefab_name)
	{
		std::filesystem::path save_path{};
		if (prefab_name.empty())
		{
			// �ۑ���̃p�X���w��(���ɑ��݂��Ă����ꍇ�͖��O��ύX����)
			const std::string path =  save_parent_path + "/" +
									  "New Prefab" + "/" +
									  "New Prefab" +
									  file_path_helper::GetPrefabExtension();
			if(is_once)
			{// �A�Z�b�g�V�[�g�Ɋ��ɑ��݂��Ă���Ώ����𔲂���
				if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(path))
					return {};
			}

			// �ۑ�����t�@�C�����̌���
			const std::string save_name  = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// �ۑ���̃p�X(�t�@�C�����A�g���q�܂�)����
			save_path = save_parent_path + "/" +
						save_name + "/" +
						save_name +
						file_path_helper::GetPrefabExtension();
		}
		else
		{
			// �ۑ���̃p�X���w��(���ɑ��݂��Ă����ꍇ�͖��O��ύX����)
			const std::string path = save_parent_path + "/" +
									 prefab_name + "/" +
									 prefab_name +
									 file_path_helper::GetPrefabExtension();
			if (is_once)
			{// �A�Z�b�g�V�[�g�Ɋ��ɑ��݂��Ă���Ώ����𔲂���
				if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(path))
					return {};
			}
			// �ۑ�����t�@�C�����̌���
			const std::string save_name = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// �ۑ���̃p�X(�t�@�C�����A�g���q�܂�)����
			save_path = save_parent_path + "/" +
						save_name + "/" +
						save_name +
						file_path_helper::GetPrefabExtension();
		}

		std::unique_ptr<Prefab> prefab = std::make_unique<Prefab>();
		// �v���t�@�u�쐬(�G���e�B�e�B���ێ����Ă���R���|�[�l���g�̒ǉ�)
		//prefab->CreatePrefab(registry, ent, save_path);

		mapping::rename_type::UUID id;
		while (true)
		{
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(id))
				continue;
			break;
		}

		// ���łɃv���t�@�u�����݂���ꍇ�͏����𔲂���
		if (prefabs.contains(id))
			return id;

		prefabs.emplace(id, std::move(prefab));

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.emplace(id, save_path.string());
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();

		return id;
	}

	mapping::rename_type::UUID PrefabLoader::CreatePrefab(
		AssetManager& asset_manager,
		ecs::Registry* const registry,
		const std::vector<mapping::rename_type::Entity>& entities,
		const bool is_once, const std::string& prefab_name)
	{
		std::filesystem::path save_path{};
		if (prefab_name.empty())
		{
			// �ۑ���̃p�X���w��(���ɑ��݂��Ă����ꍇ�͖��O��ύX����)
			const std::string path = save_parent_path + "/" +
				"New Prefab" + "/" +
				"New Prefab" +
				file_path_helper::GetPrefabExtension();
			if (is_once)
			{// �A�Z�b�g�V�[�g�Ɋ��ɑ��݂��Ă���Ώ����𔲂���
				if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(path))
					return {};
			}

			// �ۑ�����t�@�C�����̌���
			const std::string save_name = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// �ۑ���̃p�X(�t�@�C�����A�g���q�܂�)����
			save_path = save_parent_path + "/" +
				save_name + "/" +
				save_name +
				file_path_helper::GetPrefabExtension();
		}
		else
		{
			// �ۑ���̃p�X���w��(���ɑ��݂��Ă����ꍇ�͖��O��ύX����)
			const std::string path = save_parent_path + "/" +
				prefab_name + "/" +
				prefab_name +
				file_path_helper::GetPrefabExtension();
			if (is_once)
			{// �A�Z�b�g�V�[�g�Ɋ��ɑ��݂��Ă���Ώ����𔲂���
				if (!asset_manager.GetAssetSheetManager().Search<asset::Prefab>(path).empty())
					return {};
			}
			// �ۑ�����t�@�C�����̌���
			const std::string save_name = CompareAndReName<Prefab>(asset_manager, path).filename().replace_extension().string();
			// �ۑ���̃p�X(�t�@�C�����A�g���q�܂�)����
			save_path = save_parent_path + "/" +
				save_name + "/" +
				save_name +
				file_path_helper::GetPrefabExtension();
		}

		std::unique_ptr<Prefab> prefab = std::make_unique<Prefab>();
		// �v���t�@�u�쐬(�G���e�B�e�B���ێ����Ă���R���|�[�l���g�̒ǉ�)
		prefab->CreatePrefab(registry, entities, save_path);

		mapping::rename_type::UUID id;
		while (true)
		{// �A�Z�b�gID�̏d����h��
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.contains(id))
				continue;
			break;
		}

		// ���łɃv���t�@�u�����݂���ꍇ�͏����𔲂���
		if (prefabs.contains(id))
			return id;

		prefabs.emplace(id, std::move(prefab));

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<Prefab>().sheet.emplace(id, save_path.string());
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();

		return id;
	}

	mapping::rename_type::Entity PrefabLoader::Instantiate(ecs::Registry* registry, const mapping::rename_type::UUID& prefab_id)
	{
		// Prefab�ɓo�^���ꂽID�������Ă��Ȃ���Ώ����𔲂���
		if (!prefabs.contains(prefab_id))
			assert(!"Don't have prefab(PrefabLoader::Instanciate)");

		return prefabs.at(prefab_id)->Instantiate(registry);
	}

	void PrefabLoader::ImSelectablePrefab(AssetManager& asset_manager, mapping::rename_type::UUID& prefab_id)
	{
		std::filesystem::path prefab_filename{};
		bool is_dummy = false;
		const asset::AssetSheet& prefab_sheet = asset_manager.GetAssetSheetManager().GetSheet<asset::Prefab>();
		if (prefab_sheet.sheet.contains(prefab_id))
		{
			prefab_filename = prefab_sheet.sheet.at(prefab_id);
			prefab_filename.filename().replace_extension();
			is_dummy = true;
		}
		else
		{
			prefab_filename = "None";
		}

		if (ImGui::BeginCombo("Prefabs", prefab_filename.string().c_str()))
		{
			{// �_�~�[�e�N�X�`���p
				ImGui::SameLine();
				if (ImGui::Selectable(prefab_filename.string().c_str(),
					is_dummy, 0))
				{
					prefab_id = { "" };
				}
				if (is_dummy)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			for (const auto& [id, tex_filepath] : prefab_sheet.sheet)
			{
				const bool is_selected = (prefab_id == id);

				if (std::filesystem::path asset_filename = tex_filepath;
					ImGui::Selectable(asset_filename.filename().replace_extension().string().c_str(),
									  is_selected, 0))
				{
					prefab_id = id;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	void PrefabLoader::DeletePrefab(const mapping::rename_type::UUID& prefab_id, const std::filesystem::path& delete_path)
	{
		/*
		 * ������ : �udelete_path�v�ɂ͊g���q�܂Ŋ܂܂ꂽ�p�X�������Ă���
		 *			 �̂ŕK�����̐e�K�w�̃p�X����폜����
		 */

		// �o�^�����v���t�@�u���̍폜
		prefabs.erase(prefab_id);
		// ���v���t�@�u�̃t�@�C�����ƍ폜
		std::filesystem::remove_all(delete_path.parent_path());
	}
} // cumulonimbus::asset
