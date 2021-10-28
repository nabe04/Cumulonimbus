#pragma once
#include <map>
#include "loader.h"

namespace cumulonimbus::scene
{
	class Scene;
	class SceneManager;
} // cumulonimbus::scene

namespace cumulonimbus::asset
{
	struct SceneAsset
	{
		std::filesystem::path scene_file_path{}; // �u.scene�v�t�@�C���܂ł̃p�X
	};

	class SceneLoader final : public Loader
	{
	public:
		explicit SceneLoader()  = default;
		~SceneLoader() override = default;

		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : �V�[���̖��O��t���Ă̕ۑ�
		 * @remark : �ۑ����ɕۑ�����p�X�ƃV�[���̊m�F�����
		 * @param scene : ���݂̃V�[��
		 * @param path : �u.scene�v�t�@�C��������e�̃p�X
		 *				  ��) ./Data/Scenes/Sample/sample.scene -> ./Data/Scenes/Sample
		 */
		void SaveAs(scene::Scene& scene, const std::filesystem::path& path);

		// Todo : �V�[���}�l�[�W���[�ł�Save�������s�v�ɂȂ邩������Ȃ�
		/**
		 * @brief : �V�[���̕ۑ�
		 * @remark : SaveAs�Ƃ̈Ⴂ�̓V�[�������ɑ��݂��Ă��Ă��邱�Ƃ�O���
		 *			 �ۑ����s���̂ŁA�V�[���ۑ����ɖ��O��t����scene_file_dir��
		 *			 �w�肳�ꂽ�p�X�����̕ۑ����s��
		 * @remark : ������ �ۑ��������V�[���t�@�C�������݂��Ȃ��ꍇ��O�������o��
		 * @param scene : ���݂̃V�[��
		 * @param scene_file_dir : �u.scene�v�t�@�C���܂ł̃p�X
		 *							��) ./Data/Scenes/Sample/sample.scene -> ./Data/Scenes/Sample/sample.scene
		 *
		 */
		void Save(scene::Scene& scene, const std::filesystem::path& scene_file_dir);

		/**
		 * @brief : �V�[���̕ۑ�
		 * @remark : SaveAs�Ƃ̈Ⴂ�̓V�[�������ɑ��݂��Ă��Ă��邱�Ƃ�O���
		 *			 �ۑ����s���̂ŁA�V�[���ۑ����ɖ��O��t����scene_file_dir��
		 *			 �w�肳�ꂽ�p�X�����̕ۑ����s��
		 * @remark : ������ �ۑ��������V�[���t�@�C�������݂��Ȃ��ꍇ��O�������o��
		 * @param active_scenes : ���݊J����Ă���V�[��(SceneManager::active_scenes)
		 * @param save_scene_id : �ۑ��������V�[��ID
		 */
		void Save(
			std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes,
			const mapping::rename_type::UUID& save_scene_id);

		/**
		 * @brief : �V�[���̒ǉ��ǂݍ���
		 * @param scene_manager : �V�[���Ǘ��N���X
		 * @param add_scene_id : �ǉ��������V�[����ID
		 * @param add_scene_path : �ǉ��œǂݍ��ރV�[��(�g���q�u.scene�v)�܂ł̃t�@�C���p�X
		 */
		void AddScene(
			scene::SceneManager& scene_manager,
			const mapping::rename_type::UUID& add_scene_id,
			const std::filesystem::path& add_scene_path);
		/**
		 * @brief : �V�[���̒ǉ��ǂݍ���
		 * @param scene_manager : �V�[���Ǘ��N���X
		 * @param add_scene_id : �ǉ��������V�[����ID
		 */
		void AddScene(
			scene::SceneManager& scene_manager,
			const mapping::rename_type::UUID& add_scene_id);
		/**
		 * @brief : �V�[���̒ǉ��ǂݍ���
		 * @param scene_manager : �V�[���Ǘ��N���X
		 * @param add_scene_path : �ǉ��œǂݍ��ރV�[��(�g���q�u.scene�v)�܂ł̃t�@�C���p�X
		 */
		void AddScene(
			scene::SceneManager& scene_manager,
			const std::filesystem::path& add_scene_path);

		void CreateScene(scene::Scene& scene);
		void OpenScene(scene::Scene& scene, const std::filesystem::path& path);

		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override;

		bool Supported(std::filesystem::path extension) override;

	private:
		std::map<mapping::rename_type::UUID, SceneAsset> scenes{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : �V�[���̃t�H���_�ƑΉ�����scenes��ID�̍폜
		 * @param scene_id : �V�[��ID(UUID)
		 * @param delete_path : �u.scene�v�܂ł̃t�@�C���p�X
		 */
		void DeleteScene(
			const mapping::rename_type::UUID& scene_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset
