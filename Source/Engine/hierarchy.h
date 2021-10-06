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
		 * @brief : Hierarchy View上でのアセット右クリック時の操作
		 */
		void ContextMenu(ecs::Registry* registry);
		/**
		 * @brief : アセットのプレファブ化
		 * @remark : ※caution : ImGui::MenuItem内に記述すること
		 */
		void CreatePrefab(ecs::Registry* registry, const mapping::rename_type::Entity& ent);
		/**
		 * @brief : アセットの削除
		 * @remark : ※caution : ImGui::MenuItem内に記述すること
		 */
		//void DeleteAsset();
	};
} // cumulonimbus::editor