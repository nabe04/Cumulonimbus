#pragma once
#include "loader.h"
#include "effect.h"

namespace cumulonimbus::asset
{
	class EffekseerLoader final : public Loader
	{
	public:
		explicit EffekseerLoader()  = default;
		~EffekseerLoader() override = default;

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
		[[nodiscard]]
		bool Supported(std::filesystem::path extension) override;

	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Effect>> effects{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		void DeleteEffect(const mapping::rename_type::UUID& efk_id, const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset
