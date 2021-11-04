#include "scene_loader.h"

#include "scene.h"
#include "scene_manager.h"

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

	void SceneLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name)
	{
		// �A�Z�b�g�����݂��Ȃ��ꍇ�����𔲂���
		if (!scenes.contains(asset_id))
			return;

		// �ύX�O�̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Scenes/"�ύX�O�̃V�[����"/"�ύX�O�̃V�[����"
		const std::filesystem::path	before_path			= std::filesystem::path{ scenes.at(asset_id).scene_file_path }.replace_extension();
		// �ύX�O�̐e�t�@�C���p�X(�g���q���܂܂Ȃ�) ./Data/Scenes/"�ύX�O�̃V�[����"
		const std::filesystem::path before_parent_path	= before_path.parent_path();

		// �d���m�F�ς݂̃t�@�C�����擾(�g���q���܂܂Ȃ�)
		// asset_name = �d���m�F�ς݂̃V�[����(�t�@�C�����̂݁A�g���q���܂܂Ȃ�)
		const std::string asset_name = CompareAndReName<SceneAsset>(asset_manager,
																	before_parent_path.parent_path().string() + "/" +
																	changed_name + "/" + changed_name +
																	file_path_helper::GetSceneExtension()
																	).filename().replace_extension().string();

		// �ύX��̃t�@�C���p�X(�g���q���܂܂Ȃ�)   ./Data/Scenes/"�ύX��̃V�[����"/"�ύX��̃V�[����"
		const std::filesystem::path after_path = before_parent_path.parent_path().string() + "/" + asset_name + "/" + asset_name;

		// -- �t�@�C�� & �t�H���_���̕ύX --//
		// �u.scene�v�t�@�C���̃t�@�C�����ύX
		// �� : ./Data/Scenes/"�ύX�O�̃V�[����"/"�ύX�O�̃V�[����.scene" -> ./Data/Scenes/"�ύX�O�̃V�[����"/"�ύX��̃V�[����.scene"
		std::filesystem::rename(before_path.string()		+ file_path_helper::GetSceneExtension(),
								before_parent_path.string() + "/" + asset_name + file_path_helper::GetSceneExtension());
		// �u.json�v�t�@�C���̃t�@�C�����ύX
		// �� : ./Data/Scenes/"�ύX�O�̃V�[����"/"�ύX�O�̃V�[����.json" -> ./Data/Scenes/"�ύX�O�̃V�[����"/"�ύX��̃V�[����.json"
		std::filesystem::rename(before_path.string()		+ file_path_helper::GetJsonExtension(),
								before_parent_path.string() + "/" + asset_name + file_path_helper::GetJsonExtension());
		// �t�H���_���̕ύX
		// �� : ./Data/Scenes/"�ύX�O�̃V�[����" -> ./Data/Scenes/"�ύX��̃V�[����"
		std::filesystem::rename(before_parent_path, after_path.parent_path());

		// SceneAsset�̃t�@�C���p�X�ύX( ./Data/Scenes/"�ύX��̃V�[����"/"�ύX��̃V�[����.scene")
		scenes.at(asset_id).scene_file_path = after_path.string() + file_path_helper::GetSceneExtension();
		// �A�Z�b�g�V�[�g���̃t�@�C���p�X�ύX(�� :  ./Data/Scenes/"�ύX��̃V�[����"/"�ύX��̃V�[����.scene")
		asset_manager.GetAssetSheetManager().GetSheet<SceneAsset>().sheet.at(asset_id) = after_path.string() + file_path_helper::GetSceneExtension();
		// �A�Z�b�g�V�[�g�̕ۑ�
		asset_manager.Save();
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

	void SceneLoader::SaveAs(
		scene::Scene& scene,
		const std::filesystem::path& path)
	{
		std::filesystem::create_directory(path);
		// �I�����ꂽ�t�@�C���p�X���A�Z�b�g�ɑ��݂��Ă��Ȃ��ꍇ�o�^����
		if (AssetManager& asset_manager = *locator::Locator::GetAssetManager();
			asset_manager.GetAssetSheetManager().Search<SceneAsset>(path.string() + "/" +
																	path.filename().string() +
																	file_path_helper::GetSceneExtension()).empty())
		{
			Load(asset_manager, path);
		}
		scene.SaveScene(path.string(), path.filename().string());
	}

	void SceneLoader::Save(
		scene::Scene& scene,
		const std::filesystem::path& scene_file_dir)
	{
		scene.SaveScene(scene_file_dir.parent_path().string(), scene_file_dir.filename().replace_extension().string());
	}

	void SceneLoader::Save(
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes,
		const mapping::rename_type::UUID& save_scene_id)
	{
		if (!active_scenes.contains(save_scene_id))
			assert(!"Don't have scene(SceneLoader::Save)");
		if(!scenes.contains(save_scene_id))
			assert(!"Don't have scene(SceneLoader::Save)");
		// �ۑ��Ώۂ̃V�[�����ۑ�����Ă���t�@�C���܂ł̃p�X���擾
		const std::filesystem::path& scene_file_dir = scenes.at(save_scene_id).scene_file_path;
		// �V�[���ۑ�
		active_scenes.at(save_scene_id)->SaveScene(scene_file_dir.parent_path().string(), scene_file_dir.filename().replace_extension().string());
	}

	void SceneLoader::AddScene(
		scene::SceneManager& scene_manager,
		const mapping::rename_type::UUID& add_scene_id,
		const std::filesystem::path& add_scene_path)
	{
		scene_manager.AddScene(add_scene_id, add_scene_path);
	}

	void SceneLoader::AddScene(
		scene::SceneManager& scene_manager,
		const mapping::rename_type::UUID& add_scene_id)
	{
		const std::filesystem::path& add_scene_path = scenes.at(add_scene_id).scene_file_path;
		AddScene(scene_manager, add_scene_id, add_scene_path);
	}
	void SceneLoader::AddScene(
		scene::SceneManager& scene_manager,
		const std::filesystem::path& add_scene_path)
	{
		const mapping::rename_type::UUID add_scene_id = locator::Locator::GetAssetManager()->GetAssetSheetManager().Search<asset::SceneAsset>(add_scene_path);
		AddScene(scene_manager, add_scene_id, add_scene_path);
	}

	void SceneLoader::CreateScene(scene::SceneManager& scene_manager)
	{
		scene_manager.DeleteAllScene();
		scene_manager.CreateNewScene();
	}

	void SceneLoader::OpenScene(
		scene::SceneManager& scene_manager,
		const std::filesystem::path& path)
	{
		// �I�����ꂽ�t�@�C���p�X���A�Z�b�g�ɑ��݂��Ă��Ȃ��ꍇ�o�^����
		if (AssetManager& asset_manager = *locator::Locator::GetAssetManager();
			asset_manager.GetAssetSheetManager().Search<SceneAsset>(path).empty())
		{
			Load(asset_manager, path);
		}

		// �ǂݍ��ݐ�̃V�[��ID�̎擾
		const mapping::rename_type::UUID scene_id = locator::Locator::GetAssetManager()->GetAssetSheetManager().Search<SceneAsset>(path);
		if (scene_id.empty())
			assert(!"Don't have scene id(SceneLoader::OpenScene)");
		scene_manager.OpenScene(scene_id, path);
		
		//scene_manager.DeleteAllScene();
		//scene_manager.CreateNewScene();
		//scene::Scene& open_scene = *scene_manager.GetActiveScenes()->begin()->second.get();
		//mapping::rename_type::UUID scene_id = locator::Locator::GetAssetManager()->GetAssetSheetManager().Search<SceneAsset>(path);
		//open_scene.LoadScene(path.parent_path().string(), path.filename().string());
		//scene_id = scene_manager.GetActiveScenes()->begin()->first;
	}

	void SceneLoader::OpenScene(
		scene::Scene& scene,
		const std::filesystem::path& path)
	{
		// �I�����ꂽ�t�@�C���p�X���A�Z�b�g�ɑ��݂��Ă��Ȃ��ꍇ�o�^����
		if(AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		   asset_manager.GetAssetSheetManager().Search<SceneAsset>(path).empty())
		{
			Load(asset_manager, path);
		}
		// �V�X�e���Ɍ��݂̃V�[������o�^
		locator::Locator::GetSystem()->SetCurrentScenePath(path.string());
		scene.LoadScene(path.parent_path().string(), path.filename().string());
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