#pragma once
#include "rename_type_mapping.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus

namespace cumulonimbus::editor
{
	class Hierarchy final
	{
	public:
		void Render(ecs::Registry* registry);
		[[nodiscard]]
		mapping::rename_type::Entity GetSelectedEntity() const { return selected_entity; }
	private:
		std::vector<mapping::rename_type::Entity> sub_hierarchical_entities{}; // �I������Ă���G���e�B�e�B�����q�K�w�̃G���e�B�e�B�S
		mapping::rename_type::Entity selected_entity{};
		bool is_dragged_entity{ false };

		/**
		 * @brief : Hierarchy View��ł̃A�Z�b�g�E�N���b�N���̑���
		 */
		void ContextMenu(ecs::Registry* registry);
		/**
		 * @brief : �A�Z�b�g�̃v���t�@�u��
		 * @remark : ��caution : ImGui::MenuItem���ɋL�q���邱��
		 */
		void CreatePrefab(ecs::Registry* registry, const mapping::rename_type::Entity& ent);
		/**
		 * @brief : �A�Z�b�g�̃v���t�@�u��
		 * @remark : ��caution : ImGui::MenuItem���ɋL�q���邱��
		 */
		void CreatePrefab(ecs::Registry* registry, const std::vector<mapping::rename_type::Entity>& entities);
		/**
		 * @brief : �A�Z�b�g�̍폜
		 * @remark : ��caution : ImGui::MenuItem���ɋL�q���邱��
		 */
		//void DeleteAsset();

		/**
		 * @brief : �G���e�B�e�B�̐e�q�K�w�̕\��
		 * @param registry :
		 * @param ent : ���ݑI������Ă���G���e�B�e�B
		 * @param entity_name : ���ݑI������Ă���G���e�B�e�B��
		 */
		void EntityTree(
			ecs::Registry* registry,
			const mapping::rename_type::Entity& ent,
			const std::string& entity_name);


		bool HasParentEntity(
			ecs::Registry* registry,
			const mapping::rename_type::Entity& selected_ent,
			const mapping::rename_type::Entity& ent) const;
	};
} // cumulonimbus::editor