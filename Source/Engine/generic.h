#pragma once
#include <DirectXMath.h>
#include <cassert>
#include <../../Libraries/stduuid/uuid.h>
#include "rename_type_mapping.h"

#define SAFE_RELEASE(p)  if(p) { (p)->Release(); (p)=nullptr; }

static float(* const ToRadian)(float) = DirectX::XMConvertToRadians;	 // 角度をラジアンに
static float(* const ToDegree)(float) = DirectX::XMConvertToDegrees;	 // ラジアンを角度に

template<class T>
bool CheckEqual(T lhs,T rhs)
{
	if((lhs - rhs) < FLT_EPSILON)
		return true;

	return false;
}

namespace cumulonimbus::utility
{
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
} // cumulonimbus::utility