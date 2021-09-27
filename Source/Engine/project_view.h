#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <filesystem>

#include "asset_manager.h"
#include "cum_imgui_helper.h"

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
	 * @brief : Viewer���̍���
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
		void Render(const ecs::Registry* registry);

		/**
		 * @brief : Project View���Ō��ݑI��(�h���b�O)����Ă���t�@�C���̎擾
		 * @return : true -> �A�Z�b�g���h���b�O����Ă����
		 * @return : false -> �A�Z�b�g���h���b�O����Ă��Ȃ����
		 */
		[[nodiscard]]
		bool DraggingAsset(std::filesystem::path& path) const;
	private:
		NavigationType							selected_navigation{NavigationType::AllAssets};  // Navigation Pane�Ō��ݑI�𒆂̍���
		std::filesystem::path					selected_path{};   // Navigation Pane���Ō��ݑI�𒆂̃t�H���_�p�X
		std::filesystem::path					selected_file{};   // File and Folder���Ō��ݑI�𒆂̃t�@�C����
		std::filesystem::path					delete_file{};	   // File and Folder�őI�����ꂽ�A�Z�b�g�̍폜�t�@�C���p�X
		mapping::rename_type::Hash				selected_id{};	   // All Assets���Ō��ݑI�𒆂�Loader ID(Hash)
		std::map<std::filesystem::path, bool>	is_folder_open{};  // Folder Tree��̃t�H���_���J����Ă��邩�ǂ���
		helper::imgui::IMButtonState			button_state{};    // �{�^���̉������
		float									item_size{ 150 };  // File and Folder List�ɕ\������e�N�X�`���T�C�Y
		bool									is_dragged{ false }; // File and Folder List���Ń��f�����h���b�O����Ă��邩
		bool									is_rename{ false }; // �A�Z�b�g�̃��l�[���t���O
		bool									is_rename_win_open{ false };
		std::string								rename{};			// �A�Z�b�g���l�[�����̖��O�i�[�ꏊ
		mapping::rename_type::UUID				rename_id{};		// ���l�[���Ώۂ̃A�Z�b�gID


		/**
		 * @brief : �n�b�V���l����C�ӂ̌^�̖��O�ɕϊ����邽�߂̃R�l�N�^�[
		 * @remark key : �^�̃n�b�V���l
		 * @remark value : �֘A�t���������O
		 */
		std::map<mapping::rename_type::Hash, std::string> connector{};

		/**
		 * @brief : �w��̌^�ƔC�ӂ̖��O����connector�̓o�^
		 * @param name : �^�Ή������������O
		 */
		template<class Hash>
		void Register(const std::string& name);

		/**
		 * @brief : �A�Z�b�g�̃C���|�[�g�{�^��
		 */
		void ImportMenu(asset::AssetManager& asset_manager) const;

		/**
		 * @brief : File and Folder List���̃A�Z�b�g�E�N���b�N���̑���
		 */
		void ContextMenu();

		/**
		 * @brief : �A�Z�b�g�̃��l�[��
		 */
		void RenameItem();

		/**
		 * @brief : �A�Z�b�g�V�[�g���̑S�ẴA�Z�b�g�\��
		 */
		std::filesystem::path ShowAllAssets(const asset::AssetManager& asset_manager);
		/**
		 * @brief : �t�@�C���c���[�̕\��
		 */
		std::filesystem::path ShowFolderTree(const std::filesystem::path& path);

		/**
		 * @brief : Navigation Pane �őI�����ꂽ���ڂ̕\��
		 */
		std::filesystem::path ShowFileAndFolderList(asset::AssetManager& asset_manager);
	};
} // cumulonimbus::editor