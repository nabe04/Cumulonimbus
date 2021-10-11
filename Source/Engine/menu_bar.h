#pragma once

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus

namespace cumulonimbus::editor
{
	class MenuBar final
	{
	public:
		explicit MenuBar()  = default;
		~MenuBar() = default;

		void Render(ecs::Registry* registry);

	private:
		bool is_menu_open		 = false;
		bool is_create_new_scene = false; // 「New Scene」が選択されたときのポップアップフラグ
		bool no_titlebar		 = true;
		bool no_move			 = true;

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
		void PopupCreateScene(ecs::Registry* registry);
	};
} // cumulonimbus::editor
