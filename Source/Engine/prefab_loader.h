#pragma once
#include <map>
#include <memory>

#include "loader.h"
#include "rename_type_mapping.h"
#include "prefab.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::asset
{
	class PrefabLoader final : public Loader
	{
	public:
		explicit PrefabLoader()  = default;
		~PrefabLoader() override = default;

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
		 */
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		/**
		 * @brief : �A�Z�b�g�̖��O�ύX
		 * @remark : �ύX��̃N���X��asset_id�����݂��Ȃ��ꍇ�����𔲂���
		 * @remark : �v���n�u�A�Z�b�g�ɂ́u.prefab�v�ƃv���n�u����
		 *			 �t�H���_�����݂���̂ł����̃t�@�C������ύX����
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param asset_id : ���l�[���Ώۂ̃A�Z�b�gID
		 * @param changed_name : �ύX��̃t�@�C����(���t�@�C���p�X��g���q�͊܂܂Ȃ�)
		 */
		void Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name) override;

		/**
		 * @brief : �폜�������t�@�C���p�X����A�Z�b�g���폜
		 * @param asset_manager :
		 * @param path : �폜�������t�@�C���p�X(�g���q�u.prefab�v�܂ł��܂�)
		 */
		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		/**
		 * @brief : �폜�������v���t�@�u�A�Z�b�gID����A�Z�b�g���폜
		 * @param asset_manager :
		 * @param asset_id : �폜�������A�Z�b�gID
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

		/**
		 * @brief : �v���t�@�u�̍쐬
		 * @ramark : is_once(��4����)��false�ɂ����ꍇ�A�Z�b�g�V�[�g
		 *			 �ɓo�^����Ă���p�X�ƕۑ��������p�X�������ꍇ
		 *			 �v���t�@�u�̒ǉ��͍s���Ȃ�
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param registry :
		 * @param ent : �v���t�@�u���������G���e�B�e�B
		 * @param is_once : �v���t�@�u�̏d����������
		 * @param prefab_name : �v���t�@�u��
		 * @return : �쐬���ꂽ�v���t�@�u��ID(�쐬����Ȃ������ꍇ�͋󕶎���Ԃ�)
		 */
		mapping::rename_type::UUID CreatePrefab(
			AssetManager& asset_manager, ecs::Registry* registry,
			const mapping::rename_type::Entity& ent,
			bool is_once = {false},
			const std::string& prefab_name = {""});

		/**
		 * @brief : �v���t�@�u�̍쐬
		 * @ramark : is_once(��4����)��false�ɂ����ꍇ�A�Z�b�g�V�[�g
		 *			 �ɓo�^����Ă���p�X�ƕۑ��������p�X�������ꍇ
		 *			 �v���t�@�u�̒ǉ��͍s���Ȃ�
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param registry :
		 * @param entities : �v���t�@�u���������G���e�B�e�B�S
		 * @param is_once : �v���t�@�u�̏d����������
		 * @param prefab_name : �v���t�@�u��
		 * @return : �쐬���ꂽ�v���t�@�u��ID(�쐬����Ȃ������ꍇ�͋󕶎���Ԃ�)
		 */
		mapping::rename_type::UUID CreatePrefab(
			AssetManager& asset_manager, ecs::Registry* registry,
			const std::vector<mapping::rename_type::Entity>& entities,
			bool is_once = {false},
			const std::string& prefab_name = {""});

		/**
		 * @brief : �v���t�@�u����R���|�[�l���g�̒ǉ�
		 * @remark : �v���t�@�uID���Ȃ���Ώ����𒆒f
		 * @param registry :
		 * @param prefab_id : �v���t�@�u�̃A�Z�b�gID(UUID)
		 * @return : �쐬���ꂽ
		 */
		[[nodiscard]]
		mapping::rename_type::Entity Instantiate(
			ecs::Registry* registry,
			const mapping::rename_type::UUID& prefab_id);

		/**
		 * @brief : ImGui��ł̃v���n�u�I���֐�
		 * @remark : ��caution(1) : ImGui���g�p����֐����Ŏg�p���邱��
		 * @remark : ��caution(2) : ImGui::Begin()�̒��ł��̊֐����ĂԂ���
		 * @param asset_manager :
		 * @param prefab_id : �i�[�����v���n�uID(UUID)
		 */
		void ImSelectablePrefab(AssetManager& asset_manager, mapping::rename_type::UUID& prefab_id);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Prefab>> prefabs{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;

		/**
		 * @brief : �v���t�@�u�t�@�C���̍폜
		 * @remark : �u.prefab�v�̎��t�H���_���ƍ폜����
		 * @param prefab_id : �A�Z�b�g�V�[�g�ɓo�^����Ă���v���t�@�uID
		 * @param delete_path : �폜�������t�@�C���p�X(�u.prefab�v�g���q�܂ł��܂ރp�X)
		 */
		void DeletePrefab(
			const mapping::rename_type::UUID& prefab_id,
			const std::filesystem::path& delete_path);
	};
} // cumulonimbus::asset