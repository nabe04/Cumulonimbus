#pragma once
#include <DirectXMath.h>
#include <cassert>

#define SAFE_RELEASE(p)  if(p) { (p)->Release(); (p)=nullptr; }

static float(* const ToRadian)(float) = DirectX::XMConvertToRadians;	 // 角度をラジアンに
static float(* const ToDegree)(float) = DirectX::XMConvertToDegrees;	 // ラジアンを角度に

DirectX::XMFLOAT3 ScreenToNDC(DirectX::XMFLOAT3 screenPos, const int screenWidht, const int screen_height);
DirectX::XMFLOAT2 ScreenToNDC(DirectX::XMFLOAT2 screenPos, const int screenWidht, const int screen_height);

template<class T>
bool CheckEqual(T lhs,T rhs)
{
	if((lhs - rhs) < FLT_EPSILON)
		return true;

	return false;
}