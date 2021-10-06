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
		 * @brief : プレファブの作成
		 * @ramark : is_once(第4引数)をfalseにした場合アセットシート
		 *			 に登録されているパスと保存したいパスが同じ場合
		 *			 プレファブの追加は行われない
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param registry :
		 * @param ent : プレファブ化したいエンティティ
		 * @param is_once : プレファブの重複を許すか
		 * @param prefab_name : プレファブ名
		 * @return : 作成されたプレファブのID(作成されなかった場合は空文字を返す)
		 */
		mapping::rename_type::UUID CreatePrefab(
			AssetManager& asset_manager, ecs::Registry* registry,
			const mapping::rename_type::Entity& ent,
			bool is_once = {false},
			const std::string& prefab_name = {""});

		/**
		 * @brief : プレファブからコンポーネントの追加
		 * @remark : プレファブIDがなければ処理を中断
		 * @param registry :
		 * @param prefab_id : プレファブのアセットID(UUID)
		 * @return : 作成された
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