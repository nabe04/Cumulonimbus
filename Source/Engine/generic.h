#pragma once
#include <DirectXMath.h>
#include <cassert>
#include "rename_type_mapping.h"

#define SAFE_RELEASE(p)  if(p) { (p)->Release(); (p)=nullptr; }

static float(* const ToRadian)(float) = DirectX::XMConvertToRadians;	 // �p�x�����W�A����
static float(* const ToDegree)(float) = DirectX::XMConvertToDegrees;	 // ���W�A�����p�x��

DirectX::XMFLOAT3 ScreenToNDC(DirectX::XMFLOAT3 screenPos, const int screenWidht, const int screen_height);
DirectX::XMFLOAT2 ScreenToNDC(DirectX::XMFLOAT2 screenPos, const int screenWidht, const int screen_height);

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
} // cumulonimbus::utility