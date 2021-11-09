#pragma once
#include <vector>
#include <filesystem>
#include <string>

#include <DirectXMath.h>

#include <../../Libraries/stduuid/uuid.h>

#include "rename_type_mapping.h"

namespace cumulonimbus::utility
{
	template<class T>
	bool CheckEqual(T lhs, T rhs)
	{
		if ((lhs - rhs) < FLT_EPSILON)
			return true;

		return false;
	}


	template<class T>
	mapping::rename_type::Hash GetHash()
	{
		return typeid(T).hash_code();
	}

	/**
	 * @brief  : UUIDの作成
	 * @return : 作成されたUUIDも文字列
	*/
	[[nodiscard]]
	inline mapping::rename_type::UUID GenerateUUID()
	{
		std::random_device rd;
		auto seed_data = std::array<int, 6> {};
		std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
		std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
		std::ranlux48_base generator(seq);

		uuids::basic_uuid_random_generator<std::ranlux48_base> gen(&generator);
		uuids::uuid const id = gen();

		return to_string(id);
	}

	/**
	 * @brief : 現プロジェクトの絶対パスを相対パスに変換
	 * @remark : パス名に「Cumulonimbus」が存在しない時例外処理を出す
	 * @remark : 例) D:/Desktop/Cumulonimbus/Data -> ./Data
	 */
	[[nodiscard]]
	std::string ConvertAbsolutePathToRelativePath(const std::string& path);

	/**
	 * @brief : 指定されたファイルパス以下のファイル名を取得する
	 * @return : 指定されたファイルパスが存在しない場合空のパスを返す
	 * @return : 指定されたパス以下のファイル名
	 */
	[[nodiscard]]
	std::vector<std::filesystem::path> GetAllSubDirectoryFilePath(const std::string& path);
} // cumulonimbus::utility