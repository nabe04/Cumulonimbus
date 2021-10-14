#pragma once
#include <map>
#include <set>
#include <filesystem>

#include "component_base.h"
#include "cereal_helper.h"
#include "rename_type_mapping.h"
#include "asset_manager.h"

namespace cumulonimbus::ecs
{
	class Registry;
	class ComponentAssetBase;
} // cumulonimbus::ecs

namespace cumulonimbus::asset
{
	class Prefab final
	{
	public:
		/**
		 * @brief : �G���e�B�e�B���̃R���|�[�l���g���
		 * @remark : �K�w�\���ŕۑ�����ׂɕ����̃G���e�B�e�B
		 *			 ��ۑ�����K�v���o�Ă�������
		 */
		struct EntityInfo
		{
			EntityInfo();

			template<class Archive>
			void serialize(Archive&& archive)
			{
				archive(
					CEREAL_NVP(entity_name),
					CEREAL_NVP(components_name)
				);
			}

			/**
			 * @brief : �R���|�[�l���g�̓o�^
			 * @remark : �v���t�@�u�ɑΉ��������S�ẴR���|�[�l���g
			 *			 ���R���X�g���N�^�ŏ���
			 */
			template<class T>
			void RegistryComponent();

			std::string entity_name{};
			std::map<mapping::rename_type::ComponentName,
					 std::shared_ptr<ecs::ComponentAssetBase>> component_assets{};
			std::set<mapping::rename_type::ComponentName>      components_name{};
		};

		explicit Prefab();
		~Prefab() = default;

		//template<class Archive>
		//void serialize(Archive&& archive)
		//{
		//	archive(
		//		CEREAL_NVP(components_name)
		//	);
		//}

		template<class Archive>
		void load(Archive&& archive,uint32_t const version)
		{
			if(version == 0)
			{
				archive(
					CEREAL_NVP(entity_assets)
				);
			}

			if(version == 1)
			{
				archive(
					CEREAL_NVP(entity_assets)
				);
			}
		}

		template<class Archive>
		void save(Archive&& archive,uint32_t const version) const
		{
			archive(
				CEREAL_NVP(entity_assets)
			);
		}

		/**
		 * @brief : Prefab�̍쐬
		 * @remark : Hierarchy View���Entity��Prefab�����������Ɏg�p
		 * @param registry :
		 * @param ent : Prefab��������Entity
		 * @param path : �ۑ�����v���t�@�u�̃p�X(���t�@�C���p�X + �t�@�C���� + �t�@�C���g���q)
		 */
		void CreatePrefab(
			ecs::Registry* registry, const mapping::rename_type::Entity& ent,
			const std::filesystem::path& path);

		/**
		 * @brief : Prefab�̍쐬
		 * @remark : Hierarchy View���Entity��Prefab�����������Ɏg�p
		 * @param registry :
		 * @param entities : Prefab��������Entity�S
		 * @param path : �ۑ�����v���t�@�u�̃p�X(���t�@�C���p�X + �t�@�C���� + �t�@�C���g���q)
		 */
		void CreatePrefab(
			ecs::Registry* registry,
			const std::vector<mapping::rename_type::Entity>& entities,
			const std::filesystem::path& path
		);

		/**
		 * @brief : �v���t�@�u�̃C���X�^���X��
		 * @return : �C���X�^���X�����ꂽ�G���e�B�e�B�̐e�̃G���e�B�e�B��Ԃ�
		 */
		mapping::rename_type::Entity Instanciate(ecs::Registry* registry);
		//Todo : �VPrefab System���쐬���ꂽ�����
		void Save(const std::filesystem::path& path);
		void Load(const std::filesystem::path& path);

		void Save(ecs::Registry* registry, const std::filesystem::path& path);
		void Load(ecs::Registry* registry, const std::filesystem::path& path);

		[[nodiscard]]
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>>& GetComponentsAssets()
		{
			return component_assets;
		}

	private:
		std::map<mapping::rename_type::Entity, EntityInfo> entity_assets{};
		/**
		 * @brief : Hierarchy Component�̐e�K�w�擾�̂��߂̃R�l�N�^�[
		 * @remark : �V�[���Ƀh���b�v���ăI�u�W�F�N�g���쐬�����ۂ�
		 *			 �ɈȑO�̃G���e�B�e�B�ƍ쐬��̃G���e�B�e�B��
		 *			 �q���邽�߂̃R�l�N�^�[
		 * @remark : key -> �v���t�@�u�Ƃ��ĕۑ����ꂽ�G���e�B�e�BID(UUID)
		 * @remark : value -> �I�u�W�F�N�g�Ƃ��č쐬���ꂽ�G���e�B�e�BID(UUID)
		 */
		std::map<mapping::rename_type::Entity, mapping::rename_type::Entity> connector{};

		//Todo : �VPrefab System���o�����悯��
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>> component_assets;
		std::set<mapping::rename_type::ComponentName> components_name;
		template<class T>
		void RegistryComponent();
	};
} // cumulonimbus::asset

CEREAL_CLASS_VERSION(cumulonimbus::asset::Prefab, 0);