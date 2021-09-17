#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <filesystem>

#include "asset_manager.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus


namespace cumulonimbus::editor
{
	/**
	 * @brief : Viewer内の項目
	 */
	enum class NavigationType
	{
		AllAssets,
		FileTree,

		End
	};

	class ProjectView final
	{
	public:
		ProjectView();
		void Render(ecs::Registry* registry);
	private:
		NavigationType				current_selected_navigation{NavigationType::AllAssets};  // Navigation Paneで現在選択中の項目
		std::filesystem::path		current_selected_path{}; // Folder Tree内で現在選択中のフォルダパス
		mapping::rename_type::Hash	current_selected_id{};	 // All Assets内で現在選択中のLoader ID(Hash)
		std::map<std::filesystem::path, bool> is_folder_open{}; // Folder Tree上のフォルダが開かれているかどうか

		/**
		 * @brief : ハッシュ値から任意の型の名前に変換するためのコネクター
		 * @remark key : 型のハッシュ値
		 * @remark value : 関連付けたい名前
		 */
		std::map<mapping::rename_type::Hash, std::string> connector{};

		/**
		 * @brief : 指定の型と任意の名前からconnectorの登録
		 * @param name : 型対応させたい名前
		 */
		template<class Hash>
		void Register(const std::string& name);

		/**
		 * @brief : アセットのインポートボタン
		 */
		void ImportMenu();
		/**
		 * @brief : アセットシート内の全てのアセット表示
		 */
		std::filesystem::path ShowAllAssets(const asset::AssetManager& asset_manager);
		/**
		 * @brief : ファイルツリーの表示
		 */
		std::filesystem::path ShowFolderTree(const std::filesystem::path& path);

		/**
		 * @brief : Navigation Pane で選択された項目の表示
		 */
		std::filesystem::path ShowFileAndFolderList(const asset::AssetManager& asset_manager);
	};
} // cumulonimbus::editor