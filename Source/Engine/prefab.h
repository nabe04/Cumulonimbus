#pragma once
#include <map>
#include <filesystem>

#include "component_base.h"
#include "rename_type_mapping.h"

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
		explicit Prefab();
		~Prefab() = default;

		template<class Archive>
		void serialize(Archive&& archive);

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

		void Save(const std::filesystem::path& path);

		[[nodiscard]]
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>>& GetComponentsAssets()
		{
			return component_assets;
		}

	private:
		std::map<mapping::rename_type::ComponentName, std::shared_ptr<ecs::ComponentAssetBase>> component_assets;

		template<class T>
		void RegistryComponent();
	};
} // cumulonimbus::asset