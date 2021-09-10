#pragma once
#include <filesystem>
#include <map>
#include <string>

#include "asset_sheet_manager.h"
#include "rename_type_mapping.h"

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

		virtual mapping::rename_type::UUID Convert(const std::filesystem::path& path) = 0;
	};
} // cumulonimbus::asset

