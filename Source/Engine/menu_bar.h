#pragma once
#include <string>

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
		bool is_menu_open	= false;
		bool no_titlebar	= true;
		bool no_move		= true;

		std::string name;

		/**
		 * @brief : 「File」内項目と詳細
		 */
		void FileMenu(ecs::Registry* registry);

		/**
		 * @brief : 「Window」内のメニューバー設定
		 */
		void MenuSetting();
	};
} // cumulonimbus::editor
