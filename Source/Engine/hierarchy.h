#pragma once
#include "rename_type_mapping.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace editor
	{
		class ProjectView;
	} // editor

	namespace scene
	{
		class Scene;
	}
} // cumulonimbus

namespace cumulonimbus::editor
{
	class Hierarchy final
	{
	public:
		void Render(
			mapping::rename_type::UUID& selected_scene_id,
			const std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes,
			const ProjectView& project_view);
		void Render(ecs::Registry* registry);
		[[nodiscard]]
		mapping::rename_type::Entity GetSelectedEntity() const { return selected_entity; }
	private:
		// �I������Ă���G���e�B�e�B�����q�K�w�̃G���e�B�e�B�S
		std::vector<mapping::rename_type::Entity> sub_hierarchical_entities{};
		// ���ݑI������Ă���G���e�B�e�B
		mapping::rename_type::Entity selected_entity{};
		// �V�[���𒴂��Ă̈ړ��G���e�B�e�B
		mapping::rename_type::Entity scene_transfer_entity{};
		// �폜�Ώۂ̃G���e�B�e�B
		mapping::rename_type::Entity destroyed_entity{};
		// Hierarchy View�̃E�B���h�E�^�C�g���o�[����
		float title_bar_height{};
		// Hierarchy View�̃E�B���h�E�ʒu
		DirectX::SimpleMath::Vector2 window_pos{};
		// Hierarchy View�̃E�B���h�E�T�C�Y
		DirectX::SimpleMath::Vector2 window_size{};
		// �G���e�B�e�B���h���b�O����Ă��邩(�q�G�����L�[�̈ړ��Ɏg�p)
		bool is_dragged_entity{ false };
		// �J�ڌ��̃V�[��
		scene::Scene* transition_source_scene{};
		// �J�ڐ�̃V�[��
		scene::Scene* destination_scene{};
		/**
		 * @brief : �}�E�X�J�[�\����Hierarchy View�E�B���h�E
		 *			���ɂ��邩����
		 * @return : �E�B���h�E���ɑ���
		 */
		[[nodiscard]]
		bool IsWindowHovered() const;

		/**
		 * @brief : Hierarchy View��ł̃A�Z�b�g�E�N���b�N���̑���
		 */
		void ContextMenu(ecs::Registry* registry);
		/**
		 * @brief : �A�Z�b�g�̃v���t�@�u��
		 * @remark : ��caution : ImGui::MenuItem���ɋL�q���邱��
		 */
		void CreatePrefab(ecs::Registry* registry, const mapping::rename_type::Entity& ent) const;
		/**
		 * @brief : �A�Z�b�g�̃v���t�@�u��
		 * @remark : ��caution : ImGui::MenuItem���ɋL�q���邱��
		 */
		void CreatePrefab(ecs::Registry* registry, const std::vector<mapping::rename_type::Entity>& entities) const;
		/**
		 * @brief : �V�[���Ԃ̃G���e�B�e�B�ړ�
		 * @remark : �Ώۂ̃G���e�B�e�B�̎q�G���e�B�e�B���J�ڂ���
		 */
		void SceneTransferEntity();
		/**
		 * @brief : �A�Z�b�g�̍폜
		 * @remark : ��caution : ImGui::MenuItem���ɋL�q���邱��
		 */
		void DeleteEntity(ecs::Registry* registry);

		/**
		 * @brief : �G���e�B�e�B�̐e�q�K�w�̕\��
		 * @param registry :
		 * @param selected_scene_id : �I������Ă���V�[���ύX���i�[�p�ϐ�
		 * @param current_scene_id : ���݊J����Ă���V�[��(�V�[���ύX���ɂ��̒l���K�p�����)
		 * @param ent : ���ݑI������Ă���G���e�B�e�B
		 * @param entity_name : ���ݑI������Ă���G���e�B�e�B��
		 */
		void EntityTree(
			ecs::Registry* registry,
			mapping::rename_type::UUID& selected_scene_id,
			const mapping::rename_type::UUID& current_scene_id,
			const mapping::rename_type::Entity& ent,
			const std::string& entity_name);


		bool HasParentEntity(
			ecs::Registry* registry,
			const mapping::rename_type::Entity& selected_ent,
			const mapping::rename_type::Entity& ent) const;
	};
} // cumulonimbus::editor