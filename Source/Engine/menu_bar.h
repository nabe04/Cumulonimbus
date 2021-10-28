#pragma once

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace scene
	{
		class SceneManager;
	} // scene
} // cumulonimbus

namespace cumulonimbus::editor
{
	class MenuBar final
	{
	public:
		explicit MenuBar()  = default;
		~MenuBar() = default;

		/**
		 * @brief : GUI描画処理
		 * @param selected_scene_id : 現在選択されているシーンID]
		 * @param registry : 現在開かれているシーンのレジストリ
		 * @param active_scenes : 現在開かれていいるシーン(SceneManager::active_scenes)
		 */
		void Render(
			mapping::rename_type::UUID& selected_scene_id,
			std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes);
		void Render(ecs::Registry* registry);

	private:
		bool is_menu_open		 = false;
		bool is_create_new_scene = false; // 「New Scene」が選択されたときのポップアップフラグ
		bool no_titlebar		 = true;
		bool no_move			 = true;

		/**
		 * @brief : 「File」内項目と詳細
		 * @param selected_scene_id : 現在選択されているシーンID
		 * @param active_scenes : 現在開かれていいるシーン(SceneManager::active_scenes)
		 */
		void FileMenu(
			const mapping::rename_type::UUID& selected_scene_id,
			std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes);

		// Todo : 複数シーン対応後削除予定
		/**
		 * @brief : 「File」内項目と詳細
		 */
		void FileMenu(ecs::Registry* registry);

		/**
		 * @brief : 「Window」内のメニューバー設定
		 */
		void MenuSetting();

		/**
		 * @brief : 「New Scene」が選択され新規シーンの
		 *			 作成時に表示するポップアップ
		 */
		void PopupCreateScene(
			mapping::rename_type::UUID& selected_scene_id, 
			ecs::Registry& registry, 
			scene::SceneManager& scene_manager);
	};
} // cumulonimbus::editor
