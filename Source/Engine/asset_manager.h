#pragma once
#include <memory>
#include <filesystem>

#include "asset_sheet_manager.h"
#include "rename_type_mapping.h"
#include "generic.h"
//#include "loader.h"

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
				CEREAL_NVP(sheet_manager)
			);
		}

		/**
		 * @brief : AssetSheetとLoader両方にアセットの追加
		 */
		void AddAsset(const std::filesystem::path& path);

		/**
		 *
		 */
		template<class Sheet>
		void RenameAsset(const mapping::rename_type::UUID& asset_id,
						 const std::filesystem::path& path)
		{
			if(sheet_manager->GetSheet<Sheet>().sheet.contains(asset_id))
			{
				auto& sheet = sheet_manager->GetSheet<Sheet>().sheet.at(asset_id);
				std::filesystem::rename(sheet, path);
				sheet_manager->GetSheet<Sheet>().sheet.at(asset_id) = path;
				Save();
			}
		}
		/**
		 * Todo : アセット名の変更
		 */
		void RenameAsset(const mapping::rename_type::UUID& asset_id,
						 const std::filesystem::path& path);


		/**
		 * @brief : AssetSheetとLoader両方のアセット情報削除
		 * @remark : テンプレート型(Loader)にはLoaderに登録した
		 *			 型を使用する
		 * @remark : テンプレート型(Sheet)にはAssetSheetに登録
		 *			 した型を使用する
		 */
		template<class Sheet, class Loader>
		void DeleteAssetAndLoader(const std::filesystem::path& path)
		{
			DeleteAsset<Sheet>(path);
			DeleteLoader<Loader>(path);
		}
		/**
		 * @brief : AssetSheetとLoader両方のアセット情報削除
		 * @remark : 引数のpathからアセットシート全てに検索をかけ
		 *			 ヒットしたものを削除するため型がわかっている
		 *			 場合はテンプレート側のDeleteAssetAndLoader関数を使用
		 *			 する方が速度が早い
		 */
		void DeleteAssetAndLoader(const std::filesystem::path& path);
		/**
		 * @brief : AssetSheetのアセット情報削除
		 */
		template<class Sheet>
		void DeleteAsset(const std::filesystem::path& path)
		{
			const auto& asset_id = sheet_manager->Search<Sheet>(path);
			auto& sheet = sheet_manager->GetSheet<Sheet>().sheet;
			if (sheet.contains(asset_id))
				sheet.erase(asset_id);
		}
		/**
		 * @brief : AssetSheetのアセット情報削除
		 * @remark : 引数のpathからアセットシート全てに検索をかけ
		 *			 ヒットしたものを削除するため型がわかっている
		 *			 場合はテンプレート側のDeleteAssetSheet関数を使用
		 *			 する方が速度が早い
		 */
		void DeleteAsset(const std::filesystem::path& path) const;
		/**
		 * @brief : Loaderのアセット情報削除
		 */
		template<class Loader>
		void DeleteLoader(const std::filesystem::path& path)
		{
			loaders.at(utility::GetHash<Loader>())->Delete(*this, path);
		}
		/**
		 * @brief : Loaderのアセット情報削除
		 * @remark : 引数のpathからアセットシート全てに検索をかけ
		 *			 ヒットしたものを削除するため型がわかっている
		 *			 場合はテンプレート側のDeleteLoader関数を使用
		 *			 する方が速度が早い
		 */
		void DeleteLoader(const std::filesystem::path& path);

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
