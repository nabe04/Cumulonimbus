#pragma once
#include <map>
#include <string>
#include <filesystem>
#include <cassert>

#include "rename_type_mapping.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	//class AssetManager;
	class Loader;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	struct AssetSheet
	{
		AssetSheet() = default;
		/**
		 * @brief : アセットのファイルパスシート
		 * @remark key : アセットID
		 * @remark value : アセットまでのファイルパス
		 */
		std::map<mapping::rename_type::UUID, std::string> sheet{};

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(sheet)
			);
		}
	};

	class AssetSheetManager final
	{
	public:
		explicit AssetSheetManager();
		~AssetSheetManager() = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(sheets)
			);
		}

		/**
		 * @brief : AssetSheetに登録されている型(ハッシュ値)が
		 *			存在するかをテンプレートの型(T)から判別
		 */
		template<class T>
		[[nodiscard]]
		bool HasSheet() const
		{
			if (sheets.contains(utility::GetHash<T>()))
				return true;

			return false;
		}

		/**
		 * @brief : AssetSheetに登録されている型(ハッシュ値)が
		 *			存在するかを引数から判別
		 * @param hash : 判別対象のハッシュ値
		 */
		[[nodiscard]]
		bool HasSheet(const mapping::rename_type::Hash hash) const
		{
			if (sheets.contains(hash))
				return true;

			return false;
		}

		/**
		 * @brief  : 任意のクラスのアセットシートの取得
		 * @remark : T型が存在しない場合処理を中断する
		 */
		template<class T>
		[[nodiscard]]
		AssetSheet& GetSheet()
		{
			const auto hash = utility::GetHash<T>();
			if (!sheets.contains(hash))
				assert(!"Not registered sheet(AssetSheet::GetSheet)");

			return sheets.at(hash);
		}

		[[nodiscard]]
		std::map<mapping::rename_type::Hash, AssetSheet> & GetSheets()
		{
			return sheets;
		}

		/**
		 * @brief : 任意のクラスのアセットシートに登録されている
		 *			ファイルパス + ファイル名 + 拡張子の取得
		 * @remark : 引数のidはAssetSheetのsheet(std::map)のキー値になる
		 * @remark : T型が存在しない場合処理を中断する
		 * @param id : アセットのUUID値
		 * @return : アセットのファイルパス + ファイル名 + 拡張子
		 */
		template<class T>
		[[nodiscard]]
		std::string GetAssetFilename(const mapping::rename_type::UUID& id) const
		{
			const auto hash = utility::GetHash<T>();
			if (!sheets.contains(hash))
				assert(!"Not registered sheet(AssetSheet::GetAssetFilename)");

			return sheets.at(hash).sheet.at(id);
		}
		/**
		 * @brief : 任意のクラスのアセットシートに登録されている
		 *			ファイルパス + ファイル名 + 拡張子の取得
		 * @remark : 引数のidはAssetSheetのsheet(std::map)のキー値になる
		 * @remark : idが存在しない場合空の名前("")を返す
		 * @remark : 型を全てのアセットシートに検索をかけるので型情報が
		 *			 わかっている場合はテンプレート版のGetAssetFilenameを使うべし
		 * @param id : アセットのUUID値
		 * @return : アセットのファイルパス + ファイル名 + 拡張子
		 */
		[[nodiscard]]
		std::string GetAssetFilename(const mapping::rename_type::UUID& id) const;

		/**
		 * @brief : アセットシートに登録しているパスからIDを検索
		 * @remark : パスがヒットしなかった場合でも例外処理は出ない
		 * @remark : パスがヒットしなかった場合「""」が変える
		 * @param path : 検索したいファイルパス
		 * @return : 検索でヒットしたパスのID
		 */
		template<class T>
		[[nodiscard]]
		mapping::rename_type::UUID Search(const std::filesystem::path& path) const
		{
			for (const auto& [key, value] : sheets.at(utility::GetHash<T>()).sheet)
			{
				// パスが存在すればパスの持つIDを返す
				if (path.compare(value) == 0)
					return key;
			}

			return "";
		}
		/**
		 * @brief : アセットシートに登録しているパスからIDを検索
		 * @remark : パスがヒットしなかった場合でも例外処理は出ない
		 * @remark : パスがヒットしなかった場合「""」が変える
		 * @reamrk : 型を全てのアセットシートに検索をかけるので型情報が
		 *			 わかっている場合はテンプレート版のSearchを使うべし
		 * @param path : 検索したいファイルパス
		 * @return : 検索でヒットしたパスのID
		 */
		[[nodiscard]]
		mapping::rename_type::UUID Search(const std::filesystem::path& path) const;

		/**
		 * @brief : sheetsの登録
		 * @param : 登録したい型のハッシュ値
		 */
		void Register(const mapping::rename_type::Hash& hash);
	private:
		/**
		 * @brief : (管理させたい)型ごとのシート
		 * @remark key : 登録するアセットクラス(Texture,Modelなど)のハッシュ値
		 * @remark value : AssetSheet構造体
		 */
		std::map<mapping::rename_type::Hash, AssetSheet> sheets{};

		/**
		 * @breif : sheetsの登録
		 * @remark : T : 登録するアセットクラス(Texture,Modelなど)
		 */
		template<class T>
		void Register();
	};
} // cumulonimbus::asset