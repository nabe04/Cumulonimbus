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
		MenuBar()  = default;
		~MenuBar() = default;

		void Render(ecs::Registry* registry);

	private:
		bool is_menu_open	= false;
		bool no_titlebar	= true;
		bool no_move		= true;

		/**
		 * @brief : �uFile�v�����ڂƏڍ�
		 */
		void FileMenu();

		/**
		 * @brief : �uWindow�v���̃��j���[�o�[�ݒ�
		 */
		void MenuSetting();
	};
} // cumulonimbus::editor
