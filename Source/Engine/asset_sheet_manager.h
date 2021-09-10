#pragma once
#include <map>
#include <string>
#include <filesystem>
#include <cassert>

#include "rename_type_mapping.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	class Loader;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	struct AssetSheet
	{
		/**
		 * @brief : アセットのファイルパスシート
		 * @remark key : アセットID
		 * @remark value : アセットまでのファイルパス
		 */
		std::map<mapping::rename_type::UUID, std::filesystem::path> sheet{};

		template<class Archive>
		void serialize(Archive&& archive);
	};

	class AssetSheetManager final
	{
	public:
		explicit AssetSheetManager();
		~AssetSheetManager() = default;

		template<class Archive>
		void serialize(Archive&& archive);

		void AddAsset(const std::string& filename);

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

		/**
		 * @brief : 任意のクラスのアセットシートに登録されている
		 *			ファイルパス + ファイル名 + 拡張子の取得
		 * @remark : ・引数のidはAssetSheetのsheet(std::map)のキー値になる
		 * @remark : ・T型が存在しない場合処理を中断する
		 * @param id : アセットのUUID値
		 * @return : アセットのファイルパス + ファイル名 + 拡張子
		 */
		template<class T>
		[[nodiscard]]
		std::string GetAssetFilename(const mapping::rename_type::UUID id) const
		{
			const auto hash = utility::GetHash<T>();
			if (!sheets.contains(hash))
				assert(!"Not registered sheet(AssetSheet::GetAssetFilename)");

			return sheets.at(hash).sheet.at(id).string();
		}

	private:
		/**
		 * @brief : (管理させたい)型ごとのシート
		 * @details key    : 型のハッシュ値
		 * @details	value : AssetSheet構造体
		 */
		std::map<mapping::rename_type::Hash, AssetSheet> sheets{};

		template<typename  T>
		void Register();
	};
} // cumulonimbus::asset
