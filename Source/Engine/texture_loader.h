#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

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
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param path : ���[�h���ꂽ���f���̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : �A�Z�b�g�̃��[�h
		 * @param asset_manager : AssetManager�N���X�̎Q��
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
		 * @brief : �A�Z�b�g�̖��O�ύX
		 * @remark : �ύX��̃N���X��asset_id�����݂��Ȃ��ꍇ�����𔲂���
		 * @remark : �e�N�X�`���A�Z�b�g�ɂ̓e�N�X�`���t�@�C���̕ύX����
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param asset_id : ���l�[���Ώۂ̃A�Z�b�gID
		 * @param changed_name : �ύX��̃t�@�C����(���t�@�C���p�X��g���q�͊܂܂Ȃ�)
		 */
		void Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name) override;

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
		 * @brief : �e�N�X�`���A�Z�b�g��ێ����Ă��邩��
		 *			�e�N�X�`��ID���画��
		 * @param tex_id : �e�N�X�`��ID
		 */
		[[nodiscard]]
		bool HasTexture(const mapping::rename_type::UUID& tex_id) const;

		/**
		 * @brief : �t�@�C���p�X����e�N�X�`���̍쐬
		 * @remark : ��Load�֐��Ƃ̈Ⴂ
		 * @remark : �t�H���_�̃R�s�[���s�킸path�Ɏw�肳�ꂽ
		 *			 �t�@�C���p�X�����Ƀe�N�X�`�����쐬����
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param path : �e�N�X�`���܂ł̃t�@�C���p�X(���g���q�܂�)
		 */
		void CreateTexture(AssetManager& asset_manager, const std::filesystem::path& path);

		/**
		 * @brief : �擾�������e�N�X�`����ID(UUID)�����Ƀe�N�X�`�����擾
		 * @param id : �擾�������e�N�X�`����ID(UUID)
		 * @return : �擾�����e�N�X�`���̎Q��
		 */
		Texture& GetTexture(const mapping::rename_type::UUID& id);

		/**
		 * @brief : TextureLoader�����e�N�X�`���S�Ă̎擾
		 */
		const std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>>& GetTextures() const;

		/**
		 * @brief : TextureLoader�����e�N�X�`���S�Ă̎擾
		 */
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>>& GetTextures();

		/**
		 * @brief : ImGui��ł̃e�N�X�`���I���֐�
		 * @remark : ��caution(1) : ImGui���g�p����֐����Ŏg�p���邱��
		 * @remark : ��caution(2) : ImGui::Begin()�̒��ł��̊֐����ĂԂ���
		 * @param asset_manager :
		 * @param tex_id : �i�[�����e�N�X�`��ID(UUID)
		 */
		void SelectableTexture(AssetManager& asset_manager, mapping::rename_type::UUID& tex_id);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Texture>> textures{};
		std::unique_ptr<Texture> dummy_texture{};
		//-- ImGui�p�ϐ� --//
		DirectX::SimpleMath::Vector2 selectable_image_size{ 100.f,100.f };	// �e�N�X�`���I���̉摜�T�C�Y
		DirectX::SimpleMath::Vector2 selectable_size{ 500,50 }; // ImGui::Selectable�T�C�Y
		float selectable_magnification = 1.0f; // �e�N�X�`���I�����ڂ̃T�C�Y�{��

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