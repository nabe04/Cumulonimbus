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

		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param path : ���[�h���ꂽ���f���̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param from: ���[�h���ꂽ���f���̃t�@�C���p�X
		 * @param to : �R�s�[��̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		/**
		 * @brief : �I�[�o�[���[�h����Ă���Load�֐��̋��ʏ����L�q��
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : �A�Z�b�g�̖��O�ύX
		 * @remark : �ύX��̃N���X��asset_id�����݂��Ȃ��ꍇ�����𔲂���
		 * @remark : �V�[���̃A�Z�b�g�́u.scene�v�Ɓu.json�v�t�@�C���ƃV�[������
		 *			 �t�H���_�����݂���̂ł����̃t�@�C���������O��ύX����
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param asset_id : ���l�[���Ώۂ̃A�Z�b�gID
		 * @param changed_name : �ύX��̃t�@�C����(���t�@�C���p�X��g���q�͊܂܂Ȃ�)
		 */
		void Rename(AssetManager& asset_manager,const mapping::rename_type::UUID& asset_id, const std::string& changed_name) override;

		/**
		 * @brief : �A�Z�b�g�̍폜
		 * @remark : ��caution : �폜�������p�X�����݂��Ȃ��ꍇ�������΂�
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param path : �폜�������t�@�C���p�X
		 */
		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @breief : �A�Z�b�g�̍폜
		 * @remark :  ��caution : �폜������ID�����݂��Ȃ��ꍇ�������΂�
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param asset_id : �폜�������A�Z�b�g��ID(UUID)
		 */
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override;

		/**
		 * @brief : �w�肳���ID�̃V�[�������݂��邩
		 */
		[[nodiscard]]
		bool HasScene(const mapping::rename_type::UUID& scene_id) const;

		/**
		 * @brief : �w�肳�ꂽ�g���q�̓��[�h�\��
		 * @param extension : �t�@�C���̊g���q
		 * @return	: true -> �T�|�[�g���Ă���
		 * @return  : false -> �T�|�[�g���Ă��Ȃ�
		 */
		bool Supported(std::filesystem::path extension) override;

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

		void DestroyScene(
			scene::SceneManager& scene_manager,
			const mapping::rename_type::UUID& destroy_scene_id);

		void CreateScene(scene::SceneManager& scene_manager);

		/**
		 * @brief : �t�@�C������̃V�[���ǂݍ���
		 * @param scene_manager : �V�[���Ǘ��N���X
		 * @param path : �u.scene�v�܂ł̃t�@�C���p�X
		 */
		void OpenScene(scene::SceneManager& scene_manager, const std::filesystem::path& path);

		void OpenScene(scene::Scene& scene, const std::filesystem::path& path);

		[[nodiscard]]
		std::string Name(const mapping::rename_type::UUID& scene_id) const;

		/**
		 * @brief : ImGui��ł̃V�[���I���֐�
		 * @remark : ��caution(1) : ImGui���g�p����֐����Ŏg�p���邱��
		 * @remark : ��caution(2) : ImGui::Begin()�̒��ł��̊֐����ĂԂ���
		 * @param asset_manager :
		 * @param scene_id : �i�[�����V�[��ID(UUID)
		 */
		bool ImSelectableScene(AssetManager& asset_manager, mapping::rename_type::UUID& scene_id);

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
