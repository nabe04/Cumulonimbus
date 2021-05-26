#include "arithmetic.h"

#include <random>

using namespace DirectX;

namespace arithmetic
{
	//-- Create a vector of directions from point2 to point1 (point1 - point2) --//
	XMFLOAT2 CalcVecFromTwoPositions(XMFLOAT2 point1, XMFLOAT2 point2)
	{
		XMFLOAT2 vec;
		vec.x = point1.x - point2.x;
		vec.y = point1.y - point2.y;

		return vec;
	}

	//-- Create a vector of directions from point2 to point1 (point1 - point2) --//
	XMFLOAT3 CalcVecFromTwoPositions(XMFLOAT3 point1, XMFLOAT3 point2)
	{
		XMFLOAT3 vec;
		vec.x = point1.x - point2.x;
		vec.y = point1.y - point2.y;
		vec.z = point1.z - point2.z;

		return vec;
	}

	//
	XMFLOAT3 SphereLinear(const XMVECTOR v1, const XMVECTOR v2, float s)
	{
		const XMVECTOR start  = XMVector3Normalize(v1);
		const XMVECTOR end    = XMVector3Normalize(v2);
		const XMVECTOR dotVal = XMVector3Dot(start, end);
		XMFLOAT3 val{};
		XMStoreFloat3(&val, dotVal);
		const float angle = acosf(val.x);

		const float sin_thita = sinf(angle);

		const float start_point = sinf(angle * (1.0f - s));
		const float end_point = sinf(angle * s);

		XMVECTOR out_vec = (start_point * start + end_point * end) / sin_thita;
		out_vec = XMVector3Normalize(out_vec);
		XMFLOAT3 out_f3{};
		XMStoreFloat3(&out_f3, out_vec);
		return out_f3;
	}

	XMFLOAT3 CalcUpVec(const XMFLOAT3& front, const XMFLOAT3& right)
	{
		const XMVECTOR front_vec = XMLoadFloat3(&front);
		const XMVECTOR right_vec = XMLoadFloat3(&right);

		XMFLOAT3 up_f3{};
		XMStoreFloat3(&up_f3, XMVector3Cross(front_vec, right_vec));

		return up_f3;
	}

	XMFLOAT3 CalcRightVec(const XMFLOAT3& up, const XMFLOAT3& front)
	{
		const XMVECTOR up_vec = XMLoadFloat3(&up);
		const XMVECTOR front_vec = XMLoadFloat3(&front);

		XMFLOAT3 right_f3{};
		XMStoreFloat3(&right_f3, XMVector3Cross(up_vec, front_vec));

		return right_f3;
	}

	XMFLOAT3 CalcFrontVec(const XMFLOAT3& right, const XMFLOAT3& up)
	{
		const XMVECTOR right_vec = XMLoadFloat3(&right);
		const XMVECTOR up_vec = XMLoadFloat3(&up);

		XMFLOAT3 front_f3{};
		XMStoreFloat3(&front_f3, XMVector3Cross(right_vec, up_vec));

		return front_f3;
	}

	float CalcAngleFromTwoVec(const DirectX::XMFLOAT3& v0, const DirectX::XMFLOAT3& v1)
	{
		XMVECTOR vec0 = XMLoadFloat3(&v0);
		XMVECTOR vec1 = XMLoadFloat3(&v1);

		XMVECTOR dot_vec = XMVector3Dot(vec0, vec1);
		return XMConvertToDegrees(acosf(XMVectorGetX(dot_vec)));
	}

	float NormalizeAngle(const float angle)
	{
		float normalize_angle = angle;

		if (angle> 180.0f)
		{
			normalize_angle -= 360.0f;
		}
		else if (angle < -180.0f)
		{
			normalize_angle += 360.0f;
		}

		return normalize_angle;
	}

	float CalcAngle_X(const DirectX::SimpleMath::Vector3& v)
	{
		SimpleMath::Vector3 vec_x{ 0,v.y,v.z };
		vec_x.Normalize();

		SimpleMath::Vector3 axis{ 1,0,0 };	// 上下判定に使用
		axis.Normalize();

		const float angle = CalcAngleFromTwoVec(vec_x, { 0,0,1 });	// 第２引数は0度の時のベクトル

		SimpleMath::Vector3 direction;
		axis.Cross(vec_x, direction);

		if (direction.x < 0)
			return -angle;

		return angle;
	}

	float CalcAngle_Y(const DirectX::SimpleMath::Vector3& v)
	{
		SimpleMath::Vector3 vec_y{ v.x,0,v.z };
		vec_y.Normalize();

		SimpleMath::Vector3 axis{ 0,0,1}; // 上下判定に使用
		axis.Normalize();

		const float angle = CalcAngleFromTwoVec(vec_y, { 0,0,1 });

		SimpleMath::Vector3 direction;
		axis.Cross(vec_y, direction);

		if (direction.y < 0)
			return -angle;

		return angle;
	}

	float CalcAngle_Z(const DirectX::SimpleMath::Vector3& v)
	{
		SimpleMath::Vector3 vec_z{ v.x,v.y,0 };
		vec_z.Normalize();


		// 0度の時のzの値
		SimpleMath::Vector3 axis{ 1,0,0 };
		axis.Normalize();

		float angle = CalcAngleFromTwoVec(vec_z, axis);

		SimpleMath::Vector3 direction;
		axis.Cross(vec_z, direction);

		if (direction.z < 0)
			return -angle;

		return angle;
	}

	/*
	 * brief		: 正射影ベクトルの算出
	 *				  project_vec から onto_vecへの正射影ベクトルを算出する
	 * project_vec	: 射影元ベクトル
	 * onto_vec		: 射影先ベクトル
	 */
	DirectX::XMFLOAT3 CalcProjectVector(
		const DirectX::XMFLOAT3& project_vec,
		const DirectX::XMFLOAT3& onto_vec)
	{
		DirectX::XMVECTOR project	= DirectX::XMLoadFloat3(&project_vec);
		DirectX::XMVECTOR onto		= DirectX::XMLoadFloat3(&onto_vec);
		onto	= DirectX::XMVector3Normalize(onto);

		const DirectX::XMVECTOR v_d = DirectX::XMVector3Dot(project, onto);

		const float d = DirectX::XMVectorGetX(v_d);

		if(d > 0)
		{
			DirectX::XMFLOAT3 ret_val{};
			DirectX::XMStoreFloat3(
				&ret_val,
				DirectX::XMVectorMultiply(v_d, onto));
			return ret_val;
		}

		return onto_vec;
	}


	//-----------------< 乱数生成 >-----------------
	int RandomIntInRange(const int min, const int max)
	{
		std::random_device rnd;			// 非決定的な乱数生成器
		std::mt19937 mt(rnd());		// メルセンヌ・ツイスタの32ビット版、引数は初期シード
		const std::uniform_int_distribution<> rand_range(min, max);

		return rand_range(mt);
	}

	float RandomFloatInRange(const float min, const float max)
	{
		std::random_device rnd;			// 非決定的な乱数生成器
		std::mt19937 mt(rnd());		// メルセンヌ・ツイスタの32ビット版、引数は初期シード
		const std::uniform_real_distribution<float> rand_range(min, max);

		return rand_range(mt);
	}


}