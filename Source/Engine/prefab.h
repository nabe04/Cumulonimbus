#pragma once
#include <map>
#include <set>
#include <filesystem>

#include "component_base.h"
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
		 * @brief : エンティティ毎のコンポーネント情報
		 * @remark : 階層構造で保存する為に複数のエンティティ
		 *			 を保存する必要が出てきたため
		 */
		struct ComponentAsset
		{
			template<class Archive>
			void serialize(Archive&& archive)
			{
				archive(
					CEREAL_NVP(components_name)
				);
			}

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
					CEREAL_NVP(components_name)
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
		 * @brief : Prefabの作成
		 * @remark : Hierarchy View上でEntityをPrefab化したい時に使用
		 * @param registry :
		 * @param ent : Prefab化したいEntity
		 * @param path : 保存するプレファブのパス(※ファイルパス + ファイル名 + ファイル拡張子)
		 */
		void CreatePrefab(
			ecs::Registry* registry, const mapping::rename_type::Entity& ent,
			const std::filesystem::path& path);

		void AddComponent(ecs::Registry* registry, const mapping::rename_type::Entity& ent);

		void Save(const std::filesystem::path& path);
		void Load(const std::filesystem::path& path);

		[[nodiscard]]
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>>& GetComponentsAssets()
		{
			return component_assets;
		}

	private:
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>> component_assets;
		std::set<mapping::rename_type::ComponentName> components_name;

		std::map<mapping::rename_type::Entity, ComponentAsset> entity_assets{};
		template<class T>
		void RegistryComponent();
	};
} // cumulonimbus::asset

CEREAL_CLASS_VERSION(cumulonimbus::asset::Prefab, 0);