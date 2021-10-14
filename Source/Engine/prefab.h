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
		 * @brief : エンティティ毎のコンポーネント情報
		 * @remark : 階層構造で保存する為に複数のエンティティ
		 *			 を保存する必要が出てきたため
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
			 * @brief : コンポーネントの登録
			 * @remark : プレファブに対応したい全てのコンポーネント
			 *			 をコンストラクタで書く
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
		 * @brief : Prefabの作成
		 * @remark : Hierarchy View上でEntityをPrefab化したい時に使用
		 * @param registry :
		 * @param ent : Prefab化したいEntity
		 * @param path : 保存するプレファブのパス(※ファイルパス + ファイル名 + ファイル拡張子)
		 */
		void CreatePrefab(
			ecs::Registry* registry, const mapping::rename_type::Entity& ent,
			const std::filesystem::path& path);

		/**
		 * @brief : Prefabの作成
		 * @remark : Hierarchy View上でEntityをPrefab化したい時に使用
		 * @param registry :
		 * @param entities : Prefab化したいEntity郡
		 * @param path : 保存するプレファブのパス(※ファイルパス + ファイル名 + ファイル拡張子)
		 */
		void CreatePrefab(
			ecs::Registry* registry,
			const std::vector<mapping::rename_type::Entity>& entities,
			const std::filesystem::path& path
		);

		/**
		 * @brief : プレファブのインスタンス化
		 * @return : インスタンス化されたエンティティの親のエンティティを返す
		 */
		mapping::rename_type::Entity Instanciate(ecs::Registry* registry);
		//Todo : 新Prefab Systemが作成されたら消す
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
		 * @brief : Hierarchy Componentの親階層取得のためのコネクター
		 * @remark : シーンにドロップしてオブジェクトを作成した際に
		 *			 に以前のエンティティと作成後のエンティティを
		 *			 繋げるためのコネクター
		 * @remark : key -> プレファブとして保存されたエンティティID(UUID)
		 * @remark : value -> オブジェクトとして作成されたエンティティID(UUID)
		 */
		std::map<mapping::rename_type::Entity, mapping::rename_type::Entity> connector{};

		//Todo : 新Prefab Systemが出来次第けす
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>> component_assets;
		std::set<mapping::rename_type::ComponentName> components_name;
		template<class T>
		void RegistryComponent();
	};
} // cumulonimbus::asset

CEREAL_CLASS_VERSION(cumulonimbus::asset::Prefab, 0);