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
		explicit TextureLoader();
		~TextureLoader() override = default;

		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param path : ���[�h���ꂽ���f���̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param from: ���[�h���ꂽ���f���̃t�@�C���p�X
		 * @param to : �R�s�[��̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;

		/**
		 * @brief : �I�[�o�[���[�h����Ă���Load�֐��̋��ʏ����L�q��
		 * @remark : id�ɓo�^����Ă���A�Z�b�g�̍쐬�݂̂��s��
		 *			 ���t�H���_�̃R�s�[�A�쐬�Ȃǂ͍s��Ȃ�
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : �A�Z�b�g�̍폜
		 * @remark : ��caution : �폜�������p�X�����݂��Ȃ��ꍇ�������΂�
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param path : �폜�������t�@�C���p�X
		 */
		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @breief : �A�Z�b�g�̍폜
		 * @remark :  ��caution : �폜������ID�����݂��Ȃ��ꍇ�������΂�
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param asset_id : �폜�������A�Z�b�g��ID(UUID)
		 */
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override;

		/**
		 * @brief : �w�肳�ꂽ�g���q�̓��[�h�\��
		 * @param extension : �t�@�C���̊g���q
		 * @return	: true -> �T�|�[�g���Ă���
		 * @return  : false -> �T�|�[�g���Ă��Ȃ�
		 */
		bool Supported(std::filesystem::path extension) override;

		/**
		 * @brief : �t�@�C���p�X����e�N�X�`���̍쐬
		 * @remark : ��Load�֐��Ƃ̈Ⴂ
		 * @remark : �t�H���_�̃R�s�[���s�킸path�Ɏw�肳�ꂽ
		 *			 �t�@�C���p�X�����Ƀe�N�X�`�����쐬����
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param path : �e�N�X�`���܂ł̃t�@�C���p�X(���g���q�܂�)
		 */
		void CreateTexture(AssetManager& asset_manager, const std::filesystem::path& path);

		/**
		 * @brief : �擾�������e�N�X�`����ID(UUID)�����Ƀe�N�X�`�����擾
		 * @param id : �擾�������e�N�X�`����ID(UUID)
		 * @return : �擾�����e�N�X�`���̎Q��
		 */
		Texture& GetTexture(const mapping::rename_type::UUID& id);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>> textures{};
		std::unique_ptr<Texture> dummy_texture{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : �e�N�X�`���A�Z�b�g�A�t�@�C���̍폜
		 * @param tex_id : �폜�������e�N�X�`��ID(UUID)
		 * @param delete_path : �폜�������e�N�X�`���p�X
		 */
		void DeleteTexture(
			const mapping::rename_type::UUID& tex_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset