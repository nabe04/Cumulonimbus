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
		std::vector<mapping::rename_type::Entity> sub_hierarchical_entities{}; // 選択されているエンティティよりも子階層のエンティティ郡
		mapping::rename_type::Entity selected_entity{};
		bool is_dragged_entity{ false };

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
		 * @brief : アセットのプレファブ化
		 * @remark : ※caution : ImGui::MenuItem内に記述すること
		 */
		void CreatePrefab(ecs::Registry* registry, const std::vector<mapping::rename_type::Entity>& entities);
		/**
		 * @brief : アセットの削除
		 * @remark : ※caution : ImGui::MenuItem内に記述すること
		 */
		//void DeleteAsset();

		/**
		 * @brief : エンティティの親子階層の表示
		 * @param registry :
		 * @param ent : 現在選択されているエンティティ
		 * @param entity_name : 現在選択されているエンティティ名
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