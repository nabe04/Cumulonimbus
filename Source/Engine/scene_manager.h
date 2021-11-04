#pragma once
#include <unordered_map>

#include "scene.h"
#include "rename_type_mapping.h"

class Framework;

namespace cumulonimbus
{
	namespace asset
	{
		class AssetManager;
	} // asset

	namespace renderer
	{
		class RenderPath;
	} // renderer

	namespace collision
	{
		class CollisionManager;
	} // collision
} // cumulonimbus

namespace cumulonimbus::scene
{
	class SceneManager final
	{
	public:
		explicit SceneManager(const std::shared_ptr<Window>& window);
		~SceneManager() = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);
		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Run();

		/**
		 * @brief : �V�K�V�[���̍쐬
		 * @remark : ���݊J����Ă���V�[����S�č폜����
		 *			 �V�����V�[�����쐬����
		 */
		scene::Scene& CreateNewScene();

		/**
		 * @brief : �����V�[���̓ǂݍ���
		 * @remark : ���݊J����Ă���V�[����S�č폜����
		 *			�����V�[���̓ǂݍ���
		 * @param scene_id : �ǂݍ��݂����V�[��ID
		 * @param scene_file_path : �ǂݍ��ރV�[��(�g���q�u.scene�v)�܂ł̃t�@�C���p�X
		 */
		void OpenScene(const mapping::rename_type::UUID & scene_id, const std::filesystem::path& scene_file_path);

		/**
		 * @brief : �V�[���̒ǉ��ǂݍ���
		 * @remark : ���݊J����Ă���V�[���ɒǉ�����
		 *			 �V�����V�[�����쐬����
		 * @remark : �����V�[�����́uNoTitled�v
		 */
		void AddScene();

		/**
		 * @brief : �V�[���̒ǉ��ǂݍ���
		 * @remark : ���݊J����Ă���V�[���ɒǉ�����
		 *			 �V�����V�[�����쐬����
		 * @remark : �����̃V�[����ǂݍ���Œǉ�����
		 * @param scene_id : �ǉ�����V�[����ID
		 * @param scene_file_path : �ǂݍ��ރV�[��(�g���q�u.scene�v)�܂ł̃t�@�C���p�X
		 */
		void AddScene(const mapping::rename_type::UUID& scene_id, const std::filesystem::path& scene_file_path);

		/**
		 * @brief : �V�[���̍폜
		 * @remark : ���̊֐��ł͍폜���Ɂu�ۑ����邩�v�Ȃǂ̊m�F���s��Ȃ����ߒ���
		 * @remark : �폜����ID�����݂��Ȃ��ꍇ�������^�[������(��O�����Ȃǂ͏o���Ȃ�)
		 * @param scene_id : �폜����V�[��ID
		 */
		void DeleteScene(const mapping::rename_type::UUID& scene_id);

		/**
		 * @brief : �S�V�[���̍폜
		 */
		void DeleteAllScene();

		//-- �Q�b�^�[ --//
		[[nodiscard]]
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<Scene>>* GetActiveScenes()
		{
			return &active_scenes;
		}
		[[nodiscard]]
		Scene* GetScene(const mapping::rename_type::UUID& scene_id)
		{
			return active_scenes.at(scene_id).get();
		}
		[[nodiscard]]
		Framework* GetFramework() const
		{
			return framework.get();
		}
		[[nodiscard]]
		editor::EditorManager* GetEditorManager() const
		{
			return editor_manager.get();
		}
		[[nodiscard]]
		collision::CollisionManager* GetCollisionManager() const
		{
			return collision_manager.get();
		}

	private:
		// ���݊J����Ă���V�[��
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<Scene>> active_scenes{};
		// �t���[�����[�N
		std::unique_ptr<Framework>			  framework{};
		// �`�揈����S��
		std::unique_ptr<renderer::RenderPath> render_path{};
		// �G�f�B�^�[�S�̂œ���̐ݒ�
		std::shared_ptr<system::System>		  system{};

		//-- Manager's --//
		// �A�Z�b�g�V�[�g�Ǘ��p�}�l�W���[
		std::shared_ptr<asset::AssetManager>	asset_manager{};
		// �G�f�B�^�[(GUI)�Ǘ��p�}�l�W���[
		std::unique_ptr<editor::EditorManager>	editor_manager{};
		// �����蔻��Ǘ��p�}�l�W���[
		std::unique_ptr<collision::CollisionManager> collision_manager{};

		void Execute();
		void Initialize();
		void UnInitialize();
		void Update(float dt);
		void Render();

		/**
		 * @brief : �Q�[���J�n������
		 * @remark : �uPlay�v�{�^���������ꂽ���̏���
		 */
		void BeginGame();
		/**
		 * @brief : �Q�[���I��������
		 * @remark : �uPlay�v�{�^���������ꂽ���̏���
		 */
		void EndGame();

		// Todo : �X�N���v�g�@�\���o����Ώ���
		/**
		 * @brief : �v���t�@�u�̏����쐬
		 */
		void InitialCreatePrefab();
	};
} // cumulonimbus::scene