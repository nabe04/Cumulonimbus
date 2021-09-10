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
		 * @brief : �e�N�X�`���̃��[�h
		 *
		 */
		void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path) override;

		/**
		 * @brief : �w�肳�ꂽ�g���q�̓��[�h�\��
		 * @param extension : �t�@�C���̊g���q
		 * @return	: true -> �T�|�[�g���Ă���
		 * @return  : false -> �T�|�[�g���Ă��Ȃ�
		 */
		bool Supported(std::filesystem::path extension) override;

	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>> textures;

		/**
		 * @brief : �C�ӂ̃t�H���_����w�肳�ꂽ�摜�t�@�C��(�u.png�v,�u.jpeg�v,�u.tga�v,�u.dds�v)��
		 *			�����̊Ǘ�����e�N�X�`���t�H���_�ɃR�s�[
		 * @remark : �߂�n��UUID�n��textures�̃L�[�l�ɂȂ�
		 * @param path : �t�@�C���I�����̃p�X�ƃt�@�C����(�g���q�܂�)
		 * @return : �t�@�C���p�X��UUID
		 */
		//mapping::rename_type::UUID Convert(const std::filesystem::path& path) override;
	};
} // cumulonimbus::asset
