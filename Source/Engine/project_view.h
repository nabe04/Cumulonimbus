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
		void Render(ecs::Registry* registry);
	private:
		NavigationType				selected_navigation{NavigationType::AllAssets};  // Navigation Pane�Ō��ݑI�𒆂̍���
		std::filesystem::path		selected_path{}; // Navigation Pane���Ō��ݑI�𒆂̃t�H���_�p�X
		std::filesystem::path		selected_file{}; // File and Folder���Ō��ݑI�𒆂̃t�@�C����
		mapping::rename_type::Hash	selected_id{};	 // All Assets���Ō��ݑI�𒆂�Loader ID(Hash)
		std::map<std::filesystem::path, bool> is_folder_open{}; // Folder Tree��̃t�H���_���J����Ă��邩�ǂ���
		float item_size{ 150 };
		helper::imgui::IMButtonState button_state{};

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
		void ImportMenu();
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
		std::filesystem::path ShowFileAndFolderList(const asset::AssetManager& asset_manager);
	};
} // cumulonimbus::editor