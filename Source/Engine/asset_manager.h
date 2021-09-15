#pragma once
#include <memory>
#include <filesystem>

#include "rename_type_mapping.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	class AssetSheetManager;
	class Loader;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	class AssetManager final
	{
	public:
		AssetManager();

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(sheet_manager),
				CEREAL_NVP(connector)
			);
		}

		void AddAsset(const std::filesystem::path& path);

		/**
		 * @breif : アセットシートの保存
		 * @remark : save_path(第一引数)を空白にした場合デフォルトのパス
		 *			(./Data/AssetSheet/asset_sheet.sheet)に保存される
		 * @param save_path : 保存先のパス(※拡張子「.sheet」含む)
		 */
		void Save(const std::filesystem::path& save_path = {""});

		/**
		 * @brief : T型のLoaderの取得
		 * @remark : T型はLoaderクラスから派生したクラスを指定する必要がある
		 * @return : T型のポインタ
		 */
		template<class T>
		T* GetLoader()
		{
			return static_cast<T*>(loaders.at(typeid(T).hash_code()).get());
		}

		[[nodiscard]]
		AssetSheetManager& GetAssetSheetManager() const { return *sheet_manager.get(); }

		/**
		 * @brief : Loaderとconnectorの登録
		 * @remark : AssetT : 登録するアセットクラス(Texture,Modelなど)
		 * @remark : LoaderT : AssetTタイプに対応するLoaderクラス
		 */
		template<class AssetT,class LoaderT>
		void RegisterLoader()
		{
			// Loaderの登録
			if (!loaders.contains(utility::GetHash<LoaderT>()))
				loaders.insert(std::make_pair(utility::GetHash<LoaderT>(), std::make_unique<LoaderT>()));

			if (!connector.contains(utility::GetHash<AssetT>()))
				// コネクターの登録
				connector.insert(std::make_pair(
					utility::GetHash<AssetT>(),
					utility::GetHash<LoaderT>()));
		}
	private:
		std::unique_ptr<AssetSheetManager>   sheet_manager{};
		/**
		 * @brief : 管理するLoaderごとのマップ
		 * @remark : key : 登録するLoaderクラスのハッシュ値
		 * @remark : value : Loaderのポインタ
		 */
		std::map<mapping::rename_type::Hash, std::unique_ptr<Loader>> loaders{};
		/**
		 * @brief : sheetsで管理するアセットのハッシュ値と結びつく
		 *			AssetSheetManager::loadersのハッシュ値のマップ
		 * @remark : AssetSheetManager::Load時のLoaderへのコネクターとして使用
		 * @remark : key : アセットのハッシュ値
		 * @remark : value : Loaderのハッシュ値
		 */
		std::map<mapping::rename_type::Hash, mapping::rename_type::Hash> connector{};

		/**
		 * @brief : アセットシートの読み込み
		 * @ramark : load_path(第一引数)を空白にした場合デフォルトのパスにある
		 *			 フォルダの最初にヒットした「.sheet」拡張子のファイルを読み込む
		 * @param load_path : 読み込み先のパス(拡張子「.sheet」含む)
		 */
		void Load(const std::filesystem::path& load_path = { "" });

		/**
		 * @brief : sheet_managerの情報(アセットまでのパス)を元に
		 *			アセットをLoaderにロード
		 */
		void AssetLoad();
 	};
} // cumulonimbus::asset
