#pragma once

#include <d3d11.h>

#include <DirectXMath.h>
#include <SimpleMath.h>

namespace arithmetic
{
	DirectX::XMFLOAT2 CalcVecFromTwoPositions(DirectX::XMFLOAT2 vec1, DirectX::XMFLOAT2 vec2);
	DirectX::XMFLOAT3 CalcVecFromTwoPositions(DirectX::XMFLOAT3 vec1, DirectX::XMFLOAT3 vec2);

	/*
	* brief : calculate up vector
	* font  : front vector
	* right : right vector
	*/
	DirectX::XMFLOAT3 CalcUpVec(
		const DirectX::XMFLOAT3& front,
		const DirectX::XMFLOAT3& right);

	/*
	* brief : calculate right vector
	* up    : up vector
	* front : front vector
	*/
	DirectX::XMFLOAT3 CalcRightVec(
		const DirectX::XMFLOAT3& up,
		const DirectX::XMFLOAT3& front);

	/*
	* brief : calculate front vector
	* right : right vector
	* up    : up vector
	*/
	DirectX::XMFLOAT3 CalcFrontVec(
		const DirectX::XMFLOAT3& right,
		const DirectX::XMFLOAT3& up);

	DirectX::XMFLOAT3 SphereLinear(
		const DirectX::XMVECTOR v1,
		const DirectX::XMVECTOR v2, float s);

	float NormalizeAngle(const float angle);

	/*
	 * brief : ��̃x�N�g������p�x���Z�o
	 */
	float CalcAngleFromTwoVec(
		const DirectX::XMFLOAT3& v0,
		const DirectX::XMFLOAT3& v1);

	/*
	 * brief : ������W�n�����ɂ������̂��ꂼ��̊p�x
	 */
	float CalcAngle_X(const DirectX::SimpleMath::Vector3& v);
	float CalcAngle_Y(const DirectX::SimpleMath::Vector3& v);
	float CalcAngle_Z(const DirectX::SimpleMath::Vector3& v);

	/*
	 * brief : ���ˉe�x�N�g���̎Z�o
	 */
	DirectX::XMFLOAT3 CalcProjectVector(
		const DirectX::XMFLOAT3& project_vec,
		const DirectX::XMFLOAT3& onto_vec);

	/*
	 * brief : ��������
	 */
	int   RandomIntInRange(const int min, const int max);
	float RandomFloatInRange(const float min, const float max);
}