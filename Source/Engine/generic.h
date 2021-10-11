#pragma once
#include <DirectXMath.h>
#include <cassert>
#include <../../Libraries/stduuid/uuid.h>
#include "rename_type_mapping.h"

#define SAFE_RELEASE(p)  if(p) { (p)->Release(); (p)=nullptr; }

static float(* const ToRadian)(float) = DirectX::XMConvertToRadians;	 // �p�x�����W�A����
static float(* const ToDegree)(float) = DirectX::XMConvertToDegrees;	 // ���W�A�����p�x��

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
	 * @brief  : UUID�̍쐬
	 * @return : �쐬���ꂽUUID��������
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
	 * @brief : ���v���W�F�N�g�̐�΃p�X�𑊑΃p�X�ɕϊ�
	 * @remark : �p�X���ɁuCumulonimbus�v�����݂��Ȃ�����O�������o��
	 * @remark : ��) D:/Desktop/Cumulonimbus/Data -> ./Data
	 */
	[[nodiscard]]
	std::string ConvertAbsolutePathToRelativePath(const std::string& path);
} // cumulonimbus::utility