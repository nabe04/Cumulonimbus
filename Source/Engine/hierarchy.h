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
		mapping::rename_type::Entity selected_entity{};

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
		 * @brief : �A�Z�b�g�̍폜
		 * @remark : ��caution : ImGui::MenuItem���ɋL�q���邱��
		 */
		//void DeleteAsset();
	};
} // cumulonimbus::editor