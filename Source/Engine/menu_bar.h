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

		/**
		 * @brief : GUI�`�揈��
		 * @param selected_scene_id : ���ݑI������Ă���V�[��ID
		 * @param active_scenes : ���݊J����Ă�����V�[��(SceneManager::active_scenes)
		 */
		void Render(
			const mapping::rename_type::UUID& selected_scene_id,
			std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes);
		void Render(ecs::Registry* registry);

	private:
		bool is_menu_open		 = false;
		bool is_create_new_scene = false; // �uNew Scene�v���I�����ꂽ�Ƃ��̃|�b�v�A�b�v�t���O
		bool no_titlebar		 = true;
		bool no_move			 = true;

		/**
		 * @brief : �uFile�v�����ڂƏڍ�
		 * @param selected_scene_id : ���ݑI������Ă���V�[��ID
		 * @param active_scenes : ���݊J����Ă�����V�[��(SceneManager::active_scenes)
		 */
		void FileMenu(
			const mapping::rename_type::UUID& selected_scene_id,
			std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes);
		
		/**
		 * @brief : �uFile�v�����ڂƏڍ�
		 */
		void FileMenu(ecs::Registry* registry);

		/**
		 * @brief : �uWindow�v���̃��j���[�o�[�ݒ�
		 */
		void MenuSetting();

		/**
		 * @brief : �uNew Scene�v���I������V�K�V�[����
		 *			 �쐬���ɕ\������|�b�v�A�b�v
		 */
		void PopupCreateScene(ecs::Registry* registry);
	};
} // cumulonimbus::editor
