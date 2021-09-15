#pragma once
#include "loader.h"
#include "material.h"

namespace cumulonimbus::asset
{
	class MaterialLoader final : public Loader
	{
	public:
		~MaterialLoader() override = default;

		/**
		 * @brief : �A�Z�b�g(�u.mat�v)�̃��[�h
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param path : ���[�h���ꂽ���f���̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : �A�Z�b�g(�u.mat�v)�̃��[�h
		 * @param : asset_manager : AssetManager�N���X�̎Q��
		 * @param from: ���[�h���ꂽ���f���̃t�@�C���p�X
		 * @param to : �R�s�[��̃t�@�C���p�X
		 */
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		/**
		 * @brief : �I�[�o�[���[�h����Ă���Load�֐��̋��ʏ����L�q��
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		//void LoadMaterials();

		/**
		 * @brief : �w�肳�ꂽ�g���q�̓��[�h�\��
		 * @param extension : �t�@�C���̊g���q
		 * @return	: true -> �T�|�[�g���Ă���
		 * @return  : false -> �T�|�[�g���Ă��Ȃ�
		 */
		bool Supported(std::filesystem::path extension) override;

		/**
		 * @brief : �V�K�}�e���A���̍쐬
		 * @remark : material_name���L�q���Ȃ��ꍇ���O���uNew Material�v�ɂ���
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param parent_path : �ۑ�����}�e���A���̐e�K�w�̃t�@�C���p�X
		 * @param material_data : Material�\���̂̃p�����[�^
		 * @param material_name : �}�e���A����(���g���q�s�v)
		 */
		void CreateMaterial(
			AssetManager& asset_manager, const std::filesystem::path& parent_path,
			const MaterialData& material_data, std::string material_name);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Material>> materials{};

		mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
	};
} // cumulonimbus::asset
