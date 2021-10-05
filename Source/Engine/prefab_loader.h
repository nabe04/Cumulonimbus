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
		explicit PrefabLoader();
		~PrefabLoader() override = default;

		template<class Archive>
		void serialize(Archive&& archive);

		void Load(AssetManager& asset_manager, const std::filesystem::path& path) override{}
		void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override{}
		void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) override{}

		void Delete(AssetManager& asset_manager, const std::filesystem::path& path) override{}
		void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) override{}

		bool Supported(std::filesystem::path extension) override { return true; }

		/**
		 * @brief : プレファブの作成
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param registry :
		 * @param ent : プレファブ化したいエンティティ
		 * @param prefab_name : 
		 */
		void CreatePrefab(
			AssetManager& asset_manager, ecs::Registry* registry,
			const mapping::rename_type::Entity& ent,
			const std::string& prefab_name = {""});
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Prefab>> prefabs{};

		mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) override { return { "" }; }
	};
} // cumulonimbus::asset