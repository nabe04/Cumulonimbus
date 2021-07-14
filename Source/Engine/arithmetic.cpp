#include "arithmetic.h"

#include <random>

using namespace DirectX;

namespace arithmetic
{
	/**
	 * @brief  : �ۂߌ덷���l���������l�֐�
	 * @return : true -> ���l
	 */
	bool IsEqual(float l_val, float r_val)
	{
		return (l_val - r_val) <= FLT_EPSILON ? true : false;
	}

	/**
	 * @brief : [min,max]�͈̔͂܂�n���N�����v
	 */
	float Clamp(float n, float min, float max)
	{
		if (n < min) return min;
		if (n > max) return max;

		return n;
	}



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
	XMFLOAT3 SphereLinear(const XMVECTOR& v1, const XMVECTOR& v2, float s)
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

	/**
	* @brief		: up�x�N�g���̎Z�o
	* @param front  : front vector(z��)
	* @param right  : right vector(x��)
	*/
	XMFLOAT3 CalcUpVec(const XMFLOAT3& front, const XMFLOAT3& right)
	{
		const XMVECTOR front_vec = XMLoadFloat3(&front);
		const XMVECTOR right_vec = XMLoadFloat3(&right);

		XMFLOAT3 up_f3{};
		XMStoreFloat3(&up_f3, XMVector3Cross(front_vec, right_vec));

		return up_f3;
	}

	/**
	* @brief		: right�x�N�g���̎Z�o
	* @param up     : up vector(y��)
	* @param front  : front vector(z��)
	*/
	XMFLOAT3 CalcRightVec(const XMFLOAT3& up, const XMFLOAT3& front)
	{
		const XMVECTOR up_vec = XMLoadFloat3(&up);
		const XMVECTOR front_vec = XMLoadFloat3(&front);

		XMFLOAT3 right_f3{};
		XMStoreFloat3(&right_f3, XMVector3Cross(up_vec, front_vec));

		return right_f3;
	}

	/**
	* @brief	   : calculate front vector
	* @param right : right vector(x��)
	* @param up    : up vector(y��)
	*/
	XMFLOAT3 CalcFrontVec(const XMFLOAT3& right, const XMFLOAT3& up)
	{
		const XMVECTOR right_vec = XMLoadFloat3(&right);
		const XMVECTOR up_vec = XMLoadFloat3(&up);

		XMFLOAT3 front_f3{};
		XMStoreFloat3(&front_f3, XMVector3Cross(right_vec, up_vec));

		return front_f3;
	}

	/**
	 * @brief	 : �p�x��(-180�� �` 180��)�ɐ��K��
	 * @example	 : 190�� -> -170���A-200�� -> 160��
	 */
	float NormalizeAngle(const float angle)
	{
		float normalize_angle = angle;

		if (angle > 180.0f)
		{
			normalize_angle -= 360.0f;
		}
		else if (angle < -180.0f)
		{
			normalize_angle += 360.0f;
		}

		return normalize_angle;
	}

	/**
	 * @brief     : ��̃x�N�g������p�x���Z�o
	 * @return    : �x���@�ł̊p�x
	 */
	float CalcAngleFromTwoVec(const DirectX::XMFLOAT3& v0, const DirectX::XMFLOAT3& v1)
	{
		XMVECTOR vec0 = XMLoadFloat3(&v0);
		XMVECTOR vec1 = XMLoadFloat3(&v1);

		XMVECTOR dot_vec = XMVector3Dot(vec0, vec1);
		return XMConvertToDegrees(acosf(XMVectorGetX(dot_vec)));
	}


	float CalcAngle_X(const DirectX::SimpleMath::Vector3& v)
	{
		SimpleMath::Vector3 vec_x{ 0,v.y,v.z };
		vec_x.Normalize();

		SimpleMath::Vector3 axis{ 1,0,0 };	// �㉺����Ɏg�p
		axis.Normalize();

		const float angle = CalcAngleFromTwoVec(vec_x, { 0,0,1 });	// ��Q������0�x�̎��̃x�N�g��

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

		SimpleMath::Vector3 axis{ 0,0,1}; // �㉺����Ɏg�p
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


		// 0�x�̎���z�̒l
		SimpleMath::Vector3 axis{ 1,0,0 };
		axis.Normalize();

		float angle = CalcAngleFromTwoVec(vec_z, axis);

		SimpleMath::Vector3 direction;
		axis.Cross(vec_z, direction);

		if (direction.z < 0)
			return -angle;

		return angle;
	}

	/**
	 * @brief				: ���ˉe�x�N�g���̎Z�o
	 *						  project_vec ���� onto_vec�ւ̐��ˉe�x�N�g�����Z�o����
	 * @param project_vec	: �ˉe���x�N�g��
	 * @param onto_vec		: �ˉe��x�N�g��
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


	/**
	 * @brief  : ��������(int)
	 * @return : min����max�͈̔͂ł̗���(int)
	 */
	int RandomIntInRange(const int min, const int max)
	{
		std::random_device rnd;			// �񌈒�I�ȗ���������
		std::mt19937 mt(rnd());		// �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h
		const std::uniform_int_distribution<> rand_range(min, max);

		return rand_range(mt);
	}

	/**
	 * @brief  : ��������(float)
	 * @return : min����max�͈̔͂ł̗���(float)
	 */
	float RandomFloatInRange(const float min, const float max)
	{
		std::random_device rnd;			// �񌈒�I�ȗ���������
		std::mt19937 mt(rnd());		// �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h
		const std::uniform_real_distribution<float> rand_range(min, max);

		return rand_range(mt);
	}

	/**
	 * @brief  : 2�̃x�N�g�������s�ǂ�������
	 * @return : true -> ���s�A false -> ���s����Ȃ�
	 */
	bool IsParallel(
		const SimpleMath::Vector3& v1,
		const SimpleMath::Vector3& v2)
	{
		const float dot = v1.Dot(v2);
		return IsEqual(dot, 0.0f);
	}

	/**
	 * @brief			: �����Ɠ_�Ƃ̍ŋߐړ_���Z�o
	 * @param l_start	: �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param l_end		: �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param p			: �C�ӂ̓_
	 * @return			: ������̍ŋߐړ_�̈ʒu
	 */
	SimpleMath::Vector3 ClosestPtPointLine(
		const SimpleMath::Vector3& l_start,
		const SimpleMath::Vector3& l_end,
		const SimpleMath::Vector3& p)
	{
		SimpleMath::Vector3 v1{ l_end - l_start };
		v1.Normalize();
		const Line l{ l_start,v1 };

		return ClosestPtPointLine(l, p);
	}

	/**
	 * @brief	: �����Ɠ_�Ƃ̍ŋߐړ_���Z�o
	 * @param l	: ����l
	 * @param p	: �C�ӂ̓_p
	 * @return  : ������̍ŋߐړ_�̈ʒu
	 */
	SimpleMath::Vector3 ClosestPtPointLine(
		const Line& l,
		const SimpleMath::Vector3& p)
	{
		// ����l��̔C�ӂ̓_����p�܂ł̃x�N�g��
		const SimpleMath::Vector3 v{ p - l.point };

		SimpleMath::Vector3 normal_l_vec = l.vec;
		normal_l_vec.Normalize();

		// v��normal_l_vec�Ɏˉe
		const float t = v.Dot(normal_l_vec);

		return l.vec * t;
	}

	/**
	 * @brief	  : �^����ꂽ����ab����ѓ_c�ɑ΂��āAab��̍ŋߐړ_d���v�Z
	 *			  : d(t) = a + t * (b - a)�ɂ��\�����d�̈ʒu�ɑ΂���t���Ԃ�
	 * @param a	  : ����ab�̒[�_
	 * @param b   : ����ab�̒[�_
	 * @param c	  : �C�ӂ̓_
	 * @param d	  : �ŋߐړ_
	 * @param t	  : d�̈ʒu�ɑ΂���t
	 * @attention : d,t�͎Q�Ɠn�������Ă���̂ŎZ�o���ꂽ���ʂ����邱�Ƃɒ���
	 */
	void ClosestPtPointSegment(
		const SimpleMath::Vector3& a,
		const SimpleMath::Vector3& b,
		const SimpleMath::Vector3& c,
		SimpleMath::Vector3& d, float& t)
	{
		const SimpleMath::Vector3 ab = b - a;
		// �p�����[�^������Ă���ʒu d(t) = a + t * (b - a)�̌v�Z�ɂ��ab��c���ˉe
		t = ab.Dot({ c - a }) / ab.Dot(ab);
		// �����̊O���ɂ���ꍇ�At(�]����d)���ŋߐړ_�܂ŃN�����v
		if (t < 0.0f)
			t = 0.0f;
		if (t > 1.0f)
			t = 1.0f;
		// �N�����v����Ă���̂�t����̎ˉe����Ă���ʒu���Z�o
		d = a + t * ab;
	}

	/**
	 * @brief			: 2�����̍ŒZ�����̎Z�o
	 * @param l1_start	: �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param l1_end    : �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param l2_start  : �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param l2_end    : �����̒[�_(���̂܂܂��Ɛ����ɂȂ�̂Ŋ֐�����Line�ɕϊ�)
	 * @param p1		: ����l1�̐����̈ʒu
	 * @param p2		: ����l2�̐����̈ʒu
	 * @return			: 2�����̍ŒZ����
	 */
	float ClosestPtLineLine(
		const SimpleMath::Vector3& l1_start,
		const SimpleMath::Vector3& l1_end,
		const SimpleMath::Vector3& l2_start,
		const SimpleMath::Vector3& l2_end,
		SimpleMath::Vector3& p1,
		SimpleMath::Vector3& p2)
	{
		SimpleMath::Vector3 v1{ l1_end - l1_start };
		v1.Normalize();
		const Line l1{ l1_start,v1 };
		SimpleMath::Vector3 v2{ l2_end - l2_start };
		v2.Normalize();
		const Line l2{ l2_start,v2 };

		return ClosestPtLineLine(l1, l2, p1, p2);
	}
	
	/**
	 * @brief			: 2�����̍ŒZ�����̎Z�o
	 * @param l1		: �����̐ݒ�
	 * @param l2		: �����̐ݒ�
	 * @param p1		: ����l1�̐����̈ʒu
	 * @param p2		: ����l2�̐����̈ʒu
	 * @return			: 2�����̍ŒZ����
	 */
	float ClosestPtLineLine(
		const Line& l1,
		const Line& l2,
		SimpleMath::Vector3& p1,
		SimpleMath::Vector3& p2)
	{
		if(IsParallel(l1.vec,l2.vec))
		{// 2���������s�̏ꍇ

			// ������̍ŒZ�_�̎Z�o
			p1 = ClosestPtPointLine(l1, l2.point);
			p2 = ClosestPtPointLine(l2, l1.point);
			return SimpleMath::Vector3{ p2 - p1 }.Length();
		}

		SimpleMath::Vector3 vec_l1 = l1.vec;
		SimpleMath::Vector3 vec_l2 = l2.vec;
		vec_l1.Normalize();
		vec_l2.Normalize();

		// t1�At2�̌v�Z�̕⏕
		// http://marupeke296.com/COL_3D_No19_LinesDistAndPos.html
		const float d1 = vec_l1.Dot({ l2.point - l1.point });
		const float d2 = vec_l2.Dot({ l2.point - l1.point });
		const float dv = vec_l1.Dot(vec_l2);

		const float t1 = (d1 - d2 * dv) / (1 - dv * dv);
		const float t2 = (d2 - d1 * dv) / (dv * dv - 1);

		// ���ꂼ��̒����̍ŋߐړ_���Z�o
		p1 = l1.point + t1 * vec_l1;
		p2 = l2.point + t2 * vec_l2;

		return SimpleMath::Vector3{ p2 - p1 }.Length();
	}

	/**
	 * @brief			: �����Ɛ����̍ŋߐړ_�ƍŋߐړ_�Ԃ̋����̎Z�o
	 * @param p_start	: ����p�̎n�_
	 * @param p_end		: ����p�̏I�_
	 * @param q_start	: ����q�̎n�_
	 * @param q_end		: ����q�̏I�_
	 * @param s			: S1(s) = p_start + s * (p_end - p_start)
	 * @param t			: S2(t) = q_start + t * (q_end - q_start)
	 * @param c1		: �_p�̍ŋߐړ_
	 * @param c2		: �_q�̍ŋߐړ_
	 * @return			: �ŋߐړ_�Ԃ̋���(�ŒZ����)
	 */
	float ClosestPtSegmentSegment(
		const SimpleMath::Vector3& p_start,
		const SimpleMath::Vector3& p_end,
		const SimpleMath::Vector3& q_start,
		const SimpleMath::Vector3& q_end,
		float& s, float& t,
		SimpleMath::Vector3& c1,
		SimpleMath::Vector3& c2)
	{
		const SimpleMath::Vector3 d1{ p_end - p_start };  // ����S1�̕����x�N�g��
		const SimpleMath::Vector3 d2{ q_end - q_start };	// ����S2�̕����x�N�g��
		const SimpleMath::Vector3 r{ p_start - q_start };   // �e�����̎n�[�����ԃx�N�g��
		const float a = d1.Dot(d1); // ����S1�̋����̕����A��ɔ�
		const float e = d2.Dot(d2); // ����S2�̋����̕����A��ɔ�
		const float f = d2.Dot(r);  //

		// �Е����邢�͗����̐������_�ɏk�ނ��Ă��邪�ǂ����`�F�b�N
		if(a <= FLT_EPSILON && e <= FLT_EPSILON)
		{// �����̐������_�ɏk��
			s = t = 0.0f;
			c1 = p_start;
			c2 = q_start;
			return SimpleMath::Vector3{ c1 - c2 }.Length();
		}

		if(a <= FLT_EPSILON)
		{// ����S1���_�ɏk��
			s = 0.0f;
			t = f / e;
			t = Clamp(t, 0.0f, 1.0f);
		}
		else
		{
			const float c = d1.Dot(r);
			if(e <= FLT_EPSILON)
			{// ����S2���_�ɏk��
				t = 0.0f;
				s = -c / a;
				s = Clamp(s, 0.0f, 1.0f);
			}
			else
			{
				const float b = d1.Dot(d2);
				const float denom = a * e - b * b;

				if (!IsEqual(denom, 0.0f))
				{
					s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
				}
				else
				{
					s = 0.0f;
				}

				// t = Dot((p_start + d1 * s) - p_end, d2) / Dot(d2,d2) = (b * s + f) / e
				t = (b * s + f) / e;

				// t��[0,1]�̒�(������)�ɂ���ΏI���B�����łȂ����t���N�����v
				// s��t�̐V�����l�ɑ΂��Ĉȉ���p���čČv�Z
				// s = Dot((p_end + d2 * t) - p_start , d2) / Dot(d2, d2) = (b * s  + f) / a
				// ������s��[0,1]�ɃN�����v
				if (t < 0.0f)
				{
					t = 0.0f;
					s = Clamp(-c / a, 0.0f, 1.0f);
				}
				else if (t > 1.0f)
				{
					t = 1.0f;
					s = Clamp((b - c) / a, 0.0f, 1.0f);
				}
			}
		}

		c1 = p_start + d1 * s;
		c2 = q_start + d2 * t;

		return SimpleMath::Vector3{ c1 - c2 }.Length();
	}

} // arithmetic
