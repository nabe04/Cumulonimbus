#pragma once
#include <filesystem>
#include <map>
#include <string>

#include "asset_manager.h"
#include "asset_sheet_manager.h"
#include "rename_type_mapping.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	class Loader
	{
	public:
		explicit Loader() = default;
		virtual ~Loader() = default;

		/**
		 * @brief : アセットのロード
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		virtual void Load(AssetManager& asset_manager, const std::filesystem::path& path) = 0;
		/**
		 * @brief : アセットのロード
		 * @param : asset_manager : AssetManagerクラスの参照
		 * @param from: ロードされたモデルのファイルパス
		 * @param to : コピー先のファイルパス
		 */
		virtual void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) = 0;

		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 */
		virtual void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) = 0;
		
		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		virtual bool Supported(std::filesystem::path extension) = 0;
	protected:
		/**
		 * @param asset_manager :  AssetManagerクラスの参照
		 * @param from : コピー対象のファイル名またはフォルダまでのパス
		 * @param to   : コピー先のフォルダまでのパス
		 *					 (例 : ./Data/Assets/Texture/sample.png(実際に保存されるパスとファイル名)
		 *						  -> ./Data/Assets/Texture(コピー先のフォルダまでのパス))
		 */
		virtual mapping::rename_type::UUID Convert(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) = 0;

		/**
		 * @brief : 同じフォルダ階層のファイル名を取得し
		 *			名前が重複していた場合「重複名(番号).拡張子」
		 *			という形にする
		 * @param asset_manager :  AssetManagerクラスの参照
		 * @param path : ファイルパス(拡張子を含む)
		 */
		template<class T>
		[[nodiscard]]
		std::filesystem::path CompareAndReName(const AssetManager& asset_manager, std::filesystem::path path) const
		{
			int no = 0;
			bool is_loop = true;
			const std::string exe = path.extension().string();
			std::string filename = path.replace_extension().string();
			if(asset_manager.GetAssetSheetManager().GetSheet<T>().sheet.size() == 0)
			{
				return filename;
			}

			while(is_loop)
			{
				for(const auto&[key,value] : asset_manager.GetAssetSheetManager().GetSheet<T>().sheet)
				{
					if (std::filesystem::path{ value }.parent_path().replace_extension().compare(filename) != 0)
					{
						is_loop = false;
					}
					else
					{
						is_loop = true;
						filename = path.string();
						filename += "(" + std::to_string(++no) + ")" + exe;
						break;
					}
				}
			}

			return filename;
		}
	};
} // cumulonimbus::asset

