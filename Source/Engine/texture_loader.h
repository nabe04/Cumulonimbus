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

		mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
	};
} // cumulonimbus::asset