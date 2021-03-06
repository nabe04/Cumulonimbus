#include "arithmetic.h"

#include <random>
#include <limits>

#include "locator.h"

using namespace DirectX;

namespace arithmetic
{
	SimpleMath::Vector2 CalcWindowSize(
		const XMUINT2& aspect_ratio,
		const int width, const int height)
	{
		SimpleMath::Vector2 result{};

		// 横幅のサイズを元に計算
		// aspect_ratio.x : aspect_ratio.y = width : Y
		result.y = (static_cast<float>(width) * static_cast<float>(aspect_ratio.y)) / static_cast<float>(aspect_ratio.x);
		if (result.y < static_cast<float>(height))
		{
			result.x = static_cast<float>(width);
			return result;
		}

		// 縦幅のサイズを元に計算
		// aspect_ratio.x : aspect_ratio.y = X : height
		result.x = (static_cast<float>(height) * static_cast<float>(aspect_ratio.x)) / static_cast<float>(aspect_ratio.y);
		result.y = static_cast<float>(height);
		return result;
	}

	bool IsEqual(float l_val, float r_val)
	{
		return (fabs(l_val - r_val)) <= FLT_EPSILON ? true : false;
	}

	float Clamp(float n, float min, float max)
	{
		if (n < min) return min;
		if (n > max) return max;

		return n;
	}

	float Distance(const DirectX::SimpleMath::Vector3& v1, const DirectX::SimpleMath::Vector3& v2)
	{
		const DirectX::SimpleMath::Vector3 vector_sub = XMVectorSubtract(v1, v2);

		return vector_sub.Length();
	}

	SimpleMath::Vector2 ConvertScreenToNDC(
		const SimpleMath::Vector3& screen_pos,
		const float window_width, const float window_height)
	{
		const SimpleMath::Vector2 result =
		{
			screen_pos.x / window_width * 2.f - 1.f,
			screen_pos.y / window_height * -2.f + 1.f
		};

		return result;
	}

	SimpleMath::Vector2 ConvertScreenToNDC(
		const SimpleMath::Vector2& screen_pos,
		const float window_width, const float window_height)
	{
		const SimpleMath::Vector2 result =
		{
			screen_pos.x / window_width * 2.f - 1.f,
			screen_pos.y / window_height * -2.f + 1.f
		};

		return result;
	}

	SimpleMath::Vector3 ConvertScreenToWorld(
		SimpleMath::Vector3 screen_pos,
		const SimpleMath::Matrix& view_mat,
		const SimpleMath::Matrix& projection_mat)
	{
		using namespace cumulonimbus::locator;

		// ビューポート
		const float viewport_x = 0.0f;
		const float viewport_y = 0.0f;
		const float viewport_w = static_cast<float>(Locator::GetWindow()->Width());
		const float viewport_h = static_cast<float>(Locator::GetWindow()->Height());
		const float viewport_min_z = 0.0f;
		const float viewport_max_z = 1.0f;

		XMFLOAT3 world_pos{};
		const XMVECTOR result = XMVector3Unproject(XMLoadFloat3(&screen_pos),
			viewport_x, viewport_y,
			viewport_w, viewport_h,
			viewport_min_z, viewport_max_z,
			projection_mat,
			view_mat,
			XMMatrixIdentity());
		XMStoreFloat3(&world_pos, result);
		return world_pos;
	}

	DirectX::SimpleMath::Vector3 ConvertQuaternionToEuler(const DirectX::SimpleMath::Quaternion& q)
	{
		SimpleMath::Vector3 r{};
		SimpleMath::Matrix m = DirectX::SimpleMath::Matrix::CreateFromQuaternion(q);

		if(m._32 >= 1.f - 0.0001f)
		{
			r.x = -XM_PIDIV2;
		//	r.y = .0f;
		//	r.z = atan2f(m._21, m._11);
			r.y = -atan2f(m._21, m._11);
			r.z = .0f;
		}
		else if(m._32 <= -1.f + 0.0001f)
		{
			r.x = XM_PIDIV2;
		//	r.y = .0f;
		//	r.z = atan2f(m._21, m._11);

			r.y = atan2f(m._21, m._11);
			r.z = .0f;
		}
		else
		{
			r.x = asinf(-m._32);
			r.y = atan2f(m._31, m._33);
			r.z = atan2f(m._12, m._22);
		}

		r.x = XMConvertToDegrees(r.x);
		r.y = XMConvertToDegrees(r.y);
		r.z = XMConvertToDegrees(r.z);
		return r;
	}

	bool DecomposeMatrix(
		DirectX::SimpleMath::Vector3&    translation,
		DirectX::SimpleMath::Quaternion& rotation,
		DirectX::SimpleMath::Vector3&    scaling,
		const DirectX::SimpleMath::Matrix matrix)
	{
		XMVECTOR s, r, t;

		if (!XMMatrixDecompose(&s, &r, &t, matrix))
			return false;

		XMStoreFloat3(&translation, t);
		XMStoreFloat4(&rotation, r);
		XMStoreFloat3(&scaling, s);

		return true;
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

	float CalcAngleFromTwoVec(
		const DirectX::SimpleMath::Vector3& v0,
		const DirectX::SimpleMath::Vector3& v1)
	{
		float dot = v0.Dot(v1);
		if (dot > 1.0f)
			dot = 1.0f;

		if (dot < -1.0f)
			dot = -1.0f;

		return acosf(dot);
	}

	DirectX::SimpleMath::Vector3 CalcAngleFromMatrix(
		const DirectX::SimpleMath::Matrix& m,
		const AxisFlags axis_flags,
		const bool is_returned_radian)
	{
		const float q_nan = std::numeric_limits<float>::quiet_NaN();
		// 算出された角度を格納
		DirectX::SimpleMath::Vector3 result{ q_nan ,q_nan ,q_nan };

		if(axis_flags ==  UseAllAxis)
		{
			result.x = CalcAngle_X(m, is_returned_radian);
			result.y = CalcAngle_Y(m, is_returned_radian);
			result.z = CalcAngle_Z(m, is_returned_radian);
			return result;
		}

		if(axis_flags | Axis_X)
		{
			result.x = CalcAngle_X(m, is_returned_radian);
		}
		if(axis_flags | Axis_Y)
		{
			result.y = CalcAngle_Y(m, is_returned_radian);
		}
		if(axis_flags | Axis_Z)
		{
			result.z = CalcAngle_Z(m, is_returned_radian);
		}
		return result;
	}

	float CalcAngle_X(const DirectX::SimpleMath::Matrix& m, const bool is_returned_radian)
	{
		DirectX::SimpleMath::Matrix t_m{};
		m.Transpose(t_m);
		// mの1行目とx軸の規定ベクトルからラジアン値を算出
		const float radian = atan2(t_m._32, t_m._33);

		if (is_returned_radian)
			return radian;

		return DirectX::XMConvertToDegrees(radian);
	}

	float CalcAngle_Y(const DirectX::SimpleMath::Matrix& m, const bool is_returned_radian)
	{
		DirectX::SimpleMath::Matrix t_m{};
		m.Transpose(t_m);
		// mの2行目とy軸の規定ベクトルからラジアン値を算出
		//const float radian = CalcAngleFromTwoVec(SimpleMath::Vector3{ 1,0,0 }, SimpleMath::Vector3{ m._21,m._22,m._23 });
		const float radian = atan2(-t_m._31, sqrtf((powf(t_m._32, 2) + powf(t_m._33, 2))));

		if (is_returned_radian)
			return radian;

		return DirectX::XMConvertToDegrees(radian);
	}

	float CalcAngle_Z(const DirectX::SimpleMath::Matrix& m, const bool is_returned_radian)
	{
		DirectX::SimpleMath::Matrix t_m{};
		m.Transpose(t_m);
		// mの3行目とz軸の規定ベクトルからラジアン値を算出
		//const float radian = CalcAngleFromTwoVec(SimpleMath::Vector3{ 1,0,0 }, SimpleMath::Vector3{ m._31,m._32,m._33 });
		const float radian = atan2(t_m._21, t_m._11);

		if (is_returned_radian)
			return radian;

		return DirectX::XMConvertToDegrees(radian);
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

	DirectX::SimpleMath::Vector3 QuaternionToEulerAngle(const DirectX::SimpleMath::Quaternion& q)
	{
		const float sy = 2 * q.x * q.z + 2 * q.y * q.w;
		const bool unlocked = std::fabs(sy) < 0.99999f;

		return SimpleMath::Vector3(
			unlocked ? std::atan2(-(2 * q.y * q.z - 2 * q.x * q.w), 2 * q.w * q.w + 2 * q.z * q.z - 1)
			: std::atan2(2 * q.y * q.z + 2 * q.x * q.w, 2 * q.w * q.w + 2 * q.y * q.y - 1),
			std::asin(sy),
			unlocked ? std::atan2(-(2 * q.x * q.y - 2 * q.z * q.w), 2 * q.w * q.w + 2 * q.x * q.x - 1) : 0
		);
	}


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

	bool IsParallel(
		const SimpleMath::Vector3& v1,
		const SimpleMath::Vector3& v2)
	{
		const float dot = v1.Dot(v2);
		return IsEqual(dot, 0.0f);
	}

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

	SimpleMath::Vector3 ClosestPtPointLine(
		const Line& l,
		const SimpleMath::Vector3& p)
	{
		// 直線l上の任意の点からpまでのベクトル
		const SimpleMath::Vector3 v{ p - l.point };

		SimpleMath::Vector3 normal_l_vec = l.vec;
		normal_l_vec.Normalize();

		// vとnormal_l_vecに射影
		const float t = v.Dot(normal_l_vec);

		return l.vec * t;
	}

	float SqDistPointSegment(
		const SimpleMath::Vector3& a,
		const SimpleMath::Vector3& b,
		const SimpleMath::Vector3& c)
	{
		const SimpleMath::Vector3 ab = b - a;
		const SimpleMath::Vector3 ac = c - a;
		const SimpleMath::Vector3 bc = c - b;
		const float e = ac.Dot(ab);
		// cがabの外側に書影される場合を扱う
		if (e <= 0.0f)
			return ac.Dot(ac);
		const float f = ab.Dot(ab);
		if (e >= f)
			return bc.Dot(bc);
		// cがab上に射影されている場合を扱う
		return ac.Dot(ac) - (e * e) / f;
	}


	void ClosestPtPointSegment(
		const SimpleMath::Vector3& a,
		const SimpleMath::Vector3& b,
		const SimpleMath::Vector3& c,
		SimpleMath::Vector3& d, float& t)
	{
		const SimpleMath::Vector3 ab = b - a;
		// パラメータ化されている位置 d(t) = a + t * (b - a)の計算によりabにcを射影
		t = ab.Dot({ c - a }) / ab.Dot(ab);
		// 線分の外側にある場合、t(従ってd)を最近接点までクランプ
		if (t < 0.0f)
			t = 0.0f;
		if (t > 1.0f)
			t = 1.0f;
		// クランプされているのでtからの射影されている位置を算出
		d = a + t * ab;
	}

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

	float ClosestPtLineLine(
		const Line& l1,
		const Line& l2,
		SimpleMath::Vector3& p1,
		SimpleMath::Vector3& p2)
	{
		if(IsParallel(l1.vec,l2.vec))
		{// 2直線が平行の場合

			// 直線上の最短点の算出
			p1 = ClosestPtPointLine(l1, l2.point);
			p2 = ClosestPtPointLine(l2, l1.point);
			return SimpleMath::Vector3{ p2 - p1 }.Length();
		}

		SimpleMath::Vector3 vec_l1 = l1.vec;
		SimpleMath::Vector3 vec_l2 = l2.vec;
		vec_l1.Normalize();
		vec_l2.Normalize();

		// t1、t2の計算の補助
		// http://marupeke296.com/COL_3D_No19_LinesDistAndPos.html
		const float d1 = vec_l1.Dot({ l2.point - l1.point });
		const float d2 = vec_l2.Dot({ l2.point - l1.point });
		const float dv = vec_l1.Dot(vec_l2);

		const float t1 = (d1 - d2 * dv) / (1 - dv * dv);
		const float t2 = (d2 - d1 * dv) / (dv * dv - 1);

		// それぞれの直線の最近接点を算出
		p1 = l1.point + t1 * vec_l1;
		p2 = l2.point + t2 * vec_l2;

		return SimpleMath::Vector3{ p2 - p1 }.Length();
	}

	float ClosestPtSegmentSegment(
		const SimpleMath::Vector3& p_start,
		const SimpleMath::Vector3& p_end,
		const SimpleMath::Vector3& q_start,
		const SimpleMath::Vector3& q_end,
		float& s, float& t,
		SimpleMath::Vector3& c1,
		SimpleMath::Vector3& c2)
	{
		const SimpleMath::Vector3 d1{ p_end - p_start };  // 線分S1の方向ベクトル
		const SimpleMath::Vector3 d2{ q_end - q_start };	// 線分S2の方向ベクトル
		const SimpleMath::Vector3 r{ p_start - q_start };   // 各線分の始端を結ぶベクトル
		const float a = d1.Dot(d1); // 線分S1の距離の平方、常に非負
		const float e = d2.Dot(d2); // 線分S2の距離の平方、常に非負
		const float f = d2.Dot(r);  //

		// 片方あるいは両方の線分が点に縮退しているがどうかチェック
		if(a <= FLT_EPSILON && e <= FLT_EPSILON)
		{// 両方の線分が点に縮退
			s = t = 0.0f;
			c1 = p_start;
			c2 = q_start;
			return SimpleMath::Vector3{ c1 - c2 }.Length();
		}

		if(a <= FLT_EPSILON)
		{// 線分S1が点に縮退
			s = 0.0f;
			t = f / e;
			t = Clamp(t, 0.0f, 1.0f);
		}
		else
		{
			const float c = d1.Dot(r);
			if(e <= FLT_EPSILON)
			{// 線分S2が点に縮退
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

				// tが[0,1]の中(線分内)にあれば終了。そうでなければtをクランプ
				// sをtの新しい値に対して以下を用いて再計算
				// s = Dot((p_end + d2 * t) - p_start , d2) / Dot(d2, d2) = (b * s  + f) / a
				// そしてsを[0,1]にクランプ
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
