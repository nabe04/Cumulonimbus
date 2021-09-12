#pragma once
#include "loader.h"

namespace cumulonimbus::asset
{
	class Texture;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	class TextureLoader final : public Loader
	{
	public:
		~TextureLoader() override = default;
		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param : sheet_manager : AssetSheetManager�N���X�̎Q��
		 * @param path : ���[�h���ꂽ���f���̃t�@�C���p�X
		 */
		void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param : sheet_manager : AssetSheetManager�N���X�̎Q��
		 * @param from: ���[�h���ꂽ���f���̃t�@�C���p�X
		 * @param to : �R�s�[��̃t�@�C���p�X
		 */
		void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;

		/**
		 * @brief : �w�肳�ꂽ�g���q�̓��[�h�\��
		 * @param extension : �t�@�C���̊g���q
		 * @return	: true -> �T�|�[�g���Ă���
		 * @return  : false -> �T�|�[�g���Ă��Ȃ�
		 */
		bool Supported(std::filesystem::path extension) override;
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>> textures{};

		/**
		 * @brief : �I�[�o�[���[�h����Ă���Load�֐��̋��ʏ����L�q��
		 */
		void Load(AssetSheetManager& sheet_manager, const mapping::rename_type::UUID& id) override;
	};
} // cumulonimbus::asset
