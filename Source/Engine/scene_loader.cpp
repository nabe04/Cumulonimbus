#include "scene_loader.h"

#include "scene.h"

namespace
{
	// �V�[����ۑ�����܂ł̃p�X
	const std::filesystem::path save_parent_path = "./Data/Scenes";
}

namespace cumulonimbus::asset
{
	void SceneLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, save_parent_path);
		Load(asset_manager, id);
	}

	void SceneLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void SceneLoader::Load(
		AssetManager& asset_manager,
		const mapping::rename_type::UUID& id)
	{
		// ���łɃV�[�������݂���ꍇ�����𔲂���
		if (scenes.contains(id))
			return;

		//�V�[��(�u.scene�v�t�@�C���܂ł̃p�X)�̓o�^
		SceneAsset scene_asset{};
		scene_asset.scene_file_path = asset_manager.GetAssetSheetManager().GetAssetFilename<SceneAsset>(id).c_str();
		scenes.emplace(id, scene_asset);
		// �A�Z�b�g�V�[�g(�X�V��)�̕ۑ�
		asset_manager.Save();
	}

	void SceneLoader::Save(
		scene::Scene& scene,
		const std::filesystem::path& path)
	{
		std::filesystem::create_directory(path);
		// �I�����ꂽ�t�@�C���p�X���A�Z�b�g�ɑ��݂��Ă��Ȃ��ꍇ�o�^����
		AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		if (asset_manager.GetAssetSheetManager().Search<SceneAsset>(path).empty())
		{
			Load(asset_manager, path);
		}
		scene.SaveScene(path.string(), path.filename().string());
	}

	void SceneLoader::CreateScene(scene::Scene& scene)
	{
		scene.CreateScene();
	}

	void SceneLoader::OpenScene(
		scene::Scene& scene,
		const std::filesystem::path& path)
	{
		// �I�����ꂽ�t�@�C���p�X���A�Z�b�g�ɑ��݂��Ă��Ȃ��ꍇ�o�^����
		AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		if(asset_manager.GetAssetSheetManager().Search<SceneAsset>(path).empty())
		{
			Load(asset_manager, path);
		}

		locator::Locator::GetSystem()->SetCurrentScenePath(path.string());
		scene.LoadScene(path.parent_path().string(), path.filename().string());
	}

	void SceneLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID scene_id = asset_manager.GetAssetSheetManager().Search<SceneAsset>(path);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!scenes.contains(scene_id))
			return;

		DeleteScene(scene_id, path);
		asset_manager.Save();
	}

	void SceneLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
		const std::filesystem::path delete_path = asset_manager.GetAssetSheetManager().GetAssetFilename<SceneAsset>(asset_id);
		// �A�Z�b�g(ID)�����݂��Ă��Ȃ���Ώ����𔲂���
		if (!scenes.contains(asset_id))
			return;

		DeleteScene(asset_id, delete_path);
		asset_manager.Save();
	}

	bool SceneLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			file_path_helper::GetSceneExtension()
		};

		return extensions.contains(extension);
	}

	mapping::rename_type::UUID SceneLoader::Convert(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		/*
		 * from�ɂ́u.scene�v�t�H���_�܂ł̃p�X�������Ă���̂�
		 * ���̐e�̐e�t�@�C��(./Data/Scenes)�t�H���_�������̃v���W�F�N�g
		 * �ɑ��݂��邩�Ŕ��f����
		 * (�� : ./Data/Scenes/"�V�[����"/"�V�[����".scene)
		 */
		if (!equivalent(from.parent_path().parent_path(), to))
		{
			// �R�s�[��̃t�H���_�쐬&�R�s�[
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		const std::string filename = std::filesystem::path{ from.filename() }.replace_extension().string();
		const std::string copy_str = to.string() + "/" + filename + "/" + filename + file_path_helper::GetSceneExtension();
		const std::filesystem::path copy_path{ copy_str };

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<SceneAsset>().sheet)
		{// �����A�Z�b�g�����݂��Ă����ꍇ���̃A�Z�b�g��ID(UUID)��Ԃ�
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{// ID(UUID)�̏d�����Ȃ����m�F
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<SceneAsset>().sheet.contains(id))
				continue;
			break;
		}

		// �A�Z�b�g�V�[�g�̓o�^
		asset_manager.GetAssetSheetManager().GetSheet<SceneAsset>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	void SceneLoader::DeleteScene(const mapping::rename_type::UUID& scene_id, const std::filesystem::path& delete_path)
	{
		/*
		 * ������ : �udelete_path�v�ɂ͊g���q(�u.scene�v)�܂Ŋ܂܂ꂽ�p�X�������Ă���
		 *			 �̂ŕK�����̐e�K�w("�V�[����")�̃p�X����폜����
		 *		��) ./Scenes/"�V�[����"/"�V�[����".scene
		 */

		 // �o�^�����v���t�@�u���̍폜
		scenes.erase(scene_id);
		// ���v���t�@�u�̃t�@�C�����ƍ폜
		std::filesystem::remove_all(delete_path.parent_path());
	}

} // cumulonimbus::asset