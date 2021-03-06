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
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param path : ロードされたモデルのファイルパス
		 */
		virtual void Load(AssetManager& asset_manager, const std::filesystem::path& path) = 0;
		/**
		 * @brief : アセットのロード
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param from: ロードされたモデルのファイルパス
		 * @param to : コピー先のファイルパス
		 */
		virtual void Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to) = 0;
		/**
		 * @brief : オーバーロードされているLoad関数の共通処理記述部
		 */
		virtual void Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id) = 0;

		/**
		 * @brief : アセットの名前変更
		 * @remark : 変更先のクラスでasset_idが存在しない場合処理を抜ける
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param asset_id : リネーム対象のアセットID
		 * @param changed_name : 変更後のファイル名(※ファイルパスや拡張子は含まない)
		 */
		virtual void Rename(AssetManager& asset_manager,const mapping::rename_type::UUID& asset_id, const std::string& changed_name) = 0;

		/**
		 * @brief : アセットの削除
		 * @remark : ※caution : 削除したいパスが存在しない場合処理を飛ばす
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param path : 削除したいファイルパス
		 */
		virtual void Delete(AssetManager& asset_manager, const std::filesystem::path& path) = 0;
		/**
		 * @breief : アセットの削除
		 * @remark :  ※caution : 削除したいIDが存在しない場合処理を飛ばす
		 * @param asset_manager : AssetManagerクラスの参照
		 * @param asset_id : 削除したいアセットのID(UUID)
		 */
		virtual void Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id) = 0;

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
		virtual mapping::rename_type::UUID Convert(
			AssetManager& asset_manager,
			const std::filesystem::path& from,
			const std::filesystem::path& to) = 0;

		/**
		 * @brief : 同じフォルダ階層のファイル名を取得し
		 *			名前が重複していた場合「重複名(番号).拡張子」
		 *			という形にする
		 * @param asset_manager :  AssetManagerクラスの参照
		 * @param path : ファイルパス(拡張子を含む)
		 * @return : 重複確認がとれたファイルパス(拡張子を含む)
		 */
		template<class T>
		[[nodiscard]]
		std::filesystem::path CompareAndReName(const AssetManager& asset_manager, std::filesystem::path path) const
		{
			int no		 = 0;
			bool is_loop = true;
			std::filesystem::path copy_path = path;
			std::string			  filename	= copy_path.replace_extension().string();
			const std::string	  exe	    = path.extension().string();
			// T型のアセットシートが空の場合そのままのパスで返す
			if(asset_manager.GetAssetSheetManager().GetSheet<T>().sheet.size() == 0)
			{
				return path;
			}

			while(is_loop)
			{
				// 登録されているアセット分比較し、重複しなければ処理を抜ける
				for(const auto&[key,value] : asset_manager.GetAssetSheetManager().GetSheet<T>().sheet)
				{
					if (std::filesystem::path{ value }.replace_extension().compare(filename) != 0)
					{// 重複していないので次の名前比較に移る
						is_loop = false;
					}
					else
					{// 重複したので"名前(番号)"を付けてもう一度処理をやり直す
						is_loop = true;
						filename = copy_path.string();
						filename += "(" + std::to_string(++no) + ")";
						break;
					}
				}
			}

			filename += exe;
			return filename;
		}
	};
} // cumulonimbus::asset

