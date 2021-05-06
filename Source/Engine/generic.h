#pragma once
#include <DirectXMath.h>

#define SAFE_RELEASE(p)  if(p) { (p)->Release(); (p)=nullptr; }

static float(* const ToRadian)(float) = DirectX::XMConvertToRadians;	 // �p�x�����W�A����
static float(* const ToDegree)(float) = DirectX::XMConvertToDegrees;	 // ���W�A�����p�x��

DirectX::XMFLOAT3 ScreenToNDC(DirectX::XMFLOAT3 screenPos, const int screenWidht, const int screen_height);
DirectX::XMFLOAT2 ScreenToNDC(DirectX::XMFLOAT2 screenPos, const int screenWidht, const int screen_height);