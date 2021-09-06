#pragma once
#include <map>
#include <string>
#include <memory>

#include "rename_type_mapping.h"
#include "component_tag_mapping.h"
#include "component_base.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus

namespace cumulonimbus::editor
{
	class Inspector final
	{
	public:
		explicit Inspector();
		~Inspector() = default;

		/**
		 * @brief			: インスペクタービューの描画
		 * @param registry	: レジストリー
		 * @param ent		: インスペクターを表示したいエンティティ
		 */
		void Render(ecs::Registry* registry, mapping::rename_type::Entity ent);

	private:
		std::map< std::string/*「Add Component」内で表示される名前*/
				, std::pair<std::string/*実際のコンポーネント名*/, std::unique_ptr<component::ComponentBase>>> component_map;

		/**
		 * @brief : 「Add Component」ボタンの実装
		 */
		void AddComponentButton(ecs::Registry* registry, mapping::rename_type::Entity ent);

		/**
		 * @brief : 「Add Component」内のメニューカテゴリの実装
		 * @param registry	: レジストリー
		 * @param ent		: 現在選択中のエンティティ
		 * @param menu_name	: メニュー名
		 * @param tag		: 比較用タグ
		 */
		void ComponentMenu(
			ecs::Registry* registry		, mapping::rename_type::Entity ent,
			const std::string& menu_name, mapping::component_tag::ComponentTag tag);

		/**
		 * @brief			: 「Add Component」ボタン内のコンポーネントの登録
		 * @details			: 登録されたコンポーネントは「Add Component」ボタン内で実装され追加できるようになる
		 * @details			: キー値が重複した場合例外処理が起きる
		 * @param comp_name	: component_map内のキー値として使用
		 */
		template <typename T>
		void RegisterComponent(const std::string& comp_name, mapping::component_tag::ComponentTag tag);
	};
} // cumulonimbus::editor
