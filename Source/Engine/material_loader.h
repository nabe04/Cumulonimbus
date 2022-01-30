#pragma once
#include "loader.h"
#include "material.h"

namespace cumulonimbus::asset
{
	class MaterialLoader final : public Loader
	{
	public:
		explicit MaterialLoader();
		~MaterialLoader() override = default;

		/**
		 * @brief : �A�Z�b�g(�u.mat�v)�̃��[�h
		 * @param  asset_manager : AssetManager�N���X�̎Q��
		 * @param path : ���[�h���ꂽ���f���̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : �A�Z�b�g(�u.mat�v)�̃��[�h
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param from: ���[�h���ꂽ���f���̃t�@�C���p�X
		 * @param to : �R�s�[��̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		/**
		 * @brief : �I�[�o�[���[�h����Ă���Load�֐��̋��ʏ����L�q��
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : �A�Z�b�g�̖��O�ύX
		 * @remark : �ύX��̃N���X��asset_id�����݂��Ȃ��ꍇ�����𔲂���
		 * @remark : �}�e���A���̃A�Z�b�g�ɂ́u.mat�v�Ɓu.json�v�t�@�C����
		 *			 ���݂���̂ł����̃t�@�C������ύX����
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
		 * @brief : �w�肳�ꂽid�̃}�e���A����ێ����Ă��邩
		 */
		[[nodiscard]]
		bool HasMaterial(const mapping::rename_type::UUID& mat_id) const;

		/**
		 * @brief : �V�K�}�e���A���̍쐬
		 * @remark : material_name���L�q���Ȃ��ꍇ���O���uNew Material�v�ɂ���
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param parent_path : �ۑ�����}�e���A���̐e�K�w�̃t�@�C���p�X
		 * @param material_data : Material�\���̂̃p�����[�^
		 * @param material_name : �}�e���A����(���g���q�s�v)
		 * @return : �쐬���ꂽ�}�e���A��ID(UUID)
		 */
		[[nodiscard]]
		mapping::rename_type::UUID CreateMaterial(
			AssetManager& asset_manager, const std::filesystem::path& parent_path,
			const MaterialData& material_data = {}, std::string material_name = { "" });

		/**
		 * @brief : �擾�������}�e���A����ID(UUID)�����Ƀ}�e���A�����擾
		 * @param mat_id : �擾�������}�e���A����ID(UUID)
		 * @return : �擾�����}�e���A���̎Q��
		 */
		Material& GetMaterial(const mapping::rename_type::UUID& mat_id);
		/**
		 * @brief : �}�e���A�����̎擾
		 * @param mat_id : �擾�������}�e���A��ID(UUID)
		 * @return : �w�肳�ꂽ�}�e���A����(ID�����݂��Ȃ��ꍇ�󕶎�""��Ԃ�)
		 */
		[[nodiscard]]
		std::string GetMaterialName(const mapping::rename_type::UUID& mat_id) const;
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Material>> materials{};
		std::unique_ptr<Material> dummy_material{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : �}�e���A���A�Z�b�g�ƃt�H���_���̃}�e���A���t�@�C���̍폜
		 * @param mat_id : �}�e���A��ID(UUID)
		 * @param delete_path : �폜��̃}�e���A���t�H���_�p�X
		 */
		void DeleteMaterial(
			const mapping::rename_type::UUID& mat_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset
