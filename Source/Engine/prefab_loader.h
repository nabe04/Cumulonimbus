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

		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override;
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override;
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override;

		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override;
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override;

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
		 * @brief : �v���t�@�u����R���|�[�l���g�̒ǉ�
		 * @remark : �v���t�@�uID���Ȃ���Ώ����𒆒f
		 * @param registry :
		 * @param prefab_id : �v���t�@�u�̃A�Z�b�gID(UUID)
		 * @return : �쐬���ꂽ
		 */
		[[nodiscard]]
		mapping::rename_type::Entity AddComponent(ecs::Registry* registry, const mapping::rename_type::UUID& prefab_id);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Prefab>> prefabs{};

		mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) override;
	};
} // cumulonimbus::asset