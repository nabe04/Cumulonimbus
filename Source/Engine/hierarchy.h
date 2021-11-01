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
		// 選択されているエンティティよりも子階層のエンティティ郡
		std::vector<mapping::rename_type::Entity> sub_hierarchical_entities{};
		// 現在選択されているエンティティ
		mapping::rename_type::Entity selected_entity{};
		// シーンを超えての移動エンティティ
		mapping::rename_type::Entity scene_transfer_entity{};
		// 削除対象のエンティティ
		mapping::rename_type::Entity destroyed_entity{};
		// Hierarchy Viewのウィンドウタイトルバー高さ
		float title_bar_height{};
		// Hierarchy Viewのウィンドウ位置
		DirectX::SimpleMath::Vector2 window_pos{};
		// Hierarchy Viewのウィンドウサイズ
		DirectX::SimpleMath::Vector2 window_size{};
		// エンティティがドラッグされているか(ヒエラルキーの移動に使用)
		bool is_dragged_entity{ false };
		// 遷移元のシーン
		scene::Scene* transition_source_scene{};
		// 遷移先のシーン
		scene::Scene* destination_scene{};
		/**
		 * @brief : マウスカーソルがHierarchy Viewウィンドウ
		 *			内にあるか判別
		 * @return : ウィンドウ内に存在
		 */
		[[nodiscard]]
		bool IsWindowHovered() const;

		/**
		 * @brief : Hierarchy View上でのアセット右クリック時の操作
		 */
		void ContextMenu(ecs::Registry* registry);
		/**
		 * @brief : アセットのプレファブ化
		 * @remark : ※caution : ImGui::MenuItem内に記述すること
		 */
		void CreatePrefab(ecs::Registry* registry, const mapping::rename_type::Entity& ent) const;
		/**
		 * @brief : アセットのプレファブ化
		 * @remark : ※caution : ImGui::MenuItem内に記述すること
		 */
		void CreatePrefab(ecs::Registry* registry, const std::vector<mapping::rename_type::Entity>& entities) const;
		/**
		 * @brief : シーン間のエンティティ移動
		 * @remark : 対象のエンティティの子エンティティも遷移する
		 */
		void SceneTransferEntity();
		/**
		 * @brief : アセットの削除
		 * @remark : ※caution : ImGui::MenuItem内に記述すること
		 */
		void DeleteEntity(ecs::Registry* registry);

		/**
		 * @brief : エンティティの親子階層の表示
		 * @param registry :
		 * @param selected_scene_id : 選択されているシーン変更時格納用変数
		 * @param current_scene_id : 現在開かれているシーン(シーン変更時にこの値が適用される)
		 * @param ent : 現在選択されているエンティティ
		 * @param entity_name : 現在選択されているエンティティ名
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