#pragma once
#include <filesystem>
#include <map>
#include <string>

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
		virtual void Load(AssetSheetManager& sheet_manager, const std::filesystem::path& path) = 0;
		/**
		 * @brief : 指定された拡張子はロード可能か
		 * @param extension : ファイルの拡張子
		 * @return	: true -> サポートしている
		 * @return  : false -> サポートしていない
		 */
		virtual bool Supported(std::filesystem::path extension) = 0;

	protected:
		/**
		 * @brief : アセットを管理する
		 * @remark key : UUIDを文字列に変換したも
		 * @remark value : ファイルまでのパス
		 */
		std::map<mapping::rename_type::UUID, std::string> loader{};

		/**
		 * @param from : コピー対象のファイル名またはフォルダまでのパス
		 * @param to   : コピー先のフォルダまでのパス
		 *					 (例 : ./Data/Assets/Texture/sample.png(実際に保存されるパスとファイル名)
		 *						  -> ./Data/Assets/Texture(コピー先のフォルダまでのパス))
		 */
		template<class T>
		mapping::rename_type::UUID Convert(AssetSheetManager& sheet_manager, const std::filesystem::path& from,const std::filesystem::path& to) const
		{
			// コピー先のフォルダ作成&コピー
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);

			const std::string copy_str = to.string() + "/" + from.filename().string();
			const std::filesystem::path copy_path{ copy_str };

			auto sheet = sheet_manager.GetSheet<T>().sheet;

			for (const auto& [key, value] : sheet_manager.GetSheet<T>().sheet)
			{
				if (copy_path.compare(value) == 0)
					return key;
			}

			mapping::rename_type::UUID id;
			while (true)
			{
				id = utility::GenerateUUID();
				if (sheet_manager.GetSheet<T>().sheet.contains(id))
					continue;
				break;
			}

			// アセットシートの登録
			sheet_manager.GetSheet<T>().sheet.insert(std::make_pair(id, copy_path));
			return id;
		}
	};
} // cumulonimbus::asset

