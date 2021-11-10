#include "primitive_data.h"

#include "arithmetic.h"

namespace cumulonimbus::primitive
{
	void AABB::CreateFromHalfWidth(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& halfwidth)
	{
		min = DirectX::SimpleMath::Vector3(center.x - halfwidth.x, center.y - halfwidth.y, center.z - halfwidth.z);
		max = DirectX::SimpleMath::Vector3(center.x + halfwidth.x, center.y + halfwidth.y, center.z + halfwidth.z);
	}
	AABB AABB::Transform(const DirectX::SimpleMath::Matrix& mat) const
	{
		DirectX::SimpleMath::Vector3 corners[8];
		for (int i = 0; i < 8; ++i)
		{
			DirectX::SimpleMath::Vector3 C = Corner(i);
			DirectX::SimpleMath::Vector3 point = XMVector3Transform(XMLoadFloat3(&C), mat);
			XMStoreFloat3(&corners[i], point);
		}
		DirectX::SimpleMath::Vector3 min = corners[0];
		DirectX::SimpleMath::Vector3 max = corners[6];
		for (int i = 0; i < 8; ++i)
		{
			const DirectX::XMFLOAT3& p = corners[i];

			if (p.x < min.x) min.x = p.x;
			if (p.y < min.y) min.y = p.y;
			if (p.z < min.z) min.z = p.z;

			if (p.x > max.x) max.x = p.x;
			if (p.y > max.y) max.y = p.y;
			if (p.z > max.z) max.z = p.z;
		}

		return AABB(min, max);
	}

	DirectX::SimpleMath::Vector3 AABB::GetCenter() const
	{
		return DirectX::SimpleMath::Vector3((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
	}
	DirectX::SimpleMath::Vector3 AABB::GetHalfWidth() const
	{
		DirectX::SimpleMath::Vector3 center = GetCenter();
		return DirectX::SimpleMath::Vector3(abs(max.x - center.x), abs(max.y - center.y), abs(max.z - center.z));
	}
	DirectX::SimpleMath::Matrix AABB::GetAsBoxMatrix() const
	{
		DirectX::SimpleMath::Vector3 ext = GetHalfWidth();
		DirectX::SimpleMath::Matrix  sca = DirectX::XMMatrixScaling(ext.x, ext.y, ext.z);
		DirectX::SimpleMath::Vector3 pos = GetCenter();
		DirectX::SimpleMath::Matrix  tra = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

		return sca * tra;
	}
	float AABB::GetArea() const
	{
		const DirectX::SimpleMath::Vector3 min = GetMin();
		const DirectX::SimpleMath::Vector3 max = GetMax();
		return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
	}
	float AABB::GetRadius() const {
		DirectX::XMFLOAT3 abc = GetHalfWidth();
		return std::max(std::max(abc.x, abc.y), abc.z);
	}

	bool AABB::Intersects(const DirectX::SimpleMath::Vector3& p) const {
		const DirectX::SimpleMath::Vector3 max = GetMax();
		const DirectX::SimpleMath::Vector3 min = GetMin();
		if (p.x > max.x) return false;
		if (p.x < min.x) return false;
		if (p.y > max.y) return false;
		if (p.y < min.y) return false;
		if (p.z > max.z) return false;
		if (p.z < min.z) return false;
		return true;
	}
	bool AABB::Intersects(const Ray& ray) const {
		if (Intersects(ray.origin))
			return true;

		const DirectX::SimpleMath::Vector3 min = GetMin();
		const DirectX::SimpleMath::Vector3 max = GetMax();

		float tx1 = (min.x - ray.origin.x) * ray.direction_inverse.x;
		float tx2 = (max.x - ray.origin.x) * ray.direction_inverse.x;

		float tmin = std::min(tx1, tx2);
		float tmax = std::max(tx1, tx2);

		float ty1 = (min.y - ray.origin.y) * ray.direction_inverse.y;
		float ty2 = (max.y - ray.origin.y) * ray.direction_inverse.y;

		tmin = std::max(tmin, std::min(ty1, ty2));
		tmax = std::min(tmax, std::max(ty1, ty2));

		float tz1 = (min.z - ray.origin.z) * ray.direction_inverse.z;
		float tz2 = (max.z - ray.origin.z) * ray.direction_inverse.z;

		tmin = std::max(tmin, std::min(tz1, tz2));
		tmax = std::min(tmax, std::max(tz1, tz2));

		return tmax >= tmin;
	}
	//bool AABB::Intersects(const Sphere& sphere) const
	//{
	//	return sphere.Intersects(*this);
	//}
	bool AABB::Intersects(const DirectX::BoundingFrustum& frustum) const
	{
		const DirectX::BoundingBox bb = DirectX::BoundingBox(GetCenter(), GetHalfWidth());
		const bool intersection = frustum.Intersects(bb);
		return intersection;
	}
	AABB AABB::operator* (const float a)
	{
		DirectX::SimpleMath::Vector3 min = GetMin();
		DirectX::SimpleMath::Vector3 max = GetMax();
		min.x *= a;
		min.y *= a;
		min.z *= a;
		max.x *= a;
		max.y *= a;
		max.z *= a;
		return AABB(min, max);
	}




	//bool Sphere::Intersects(const AABB& b) const {
	//	DirectX::SimpleMath::Vector3 min = b.GetMin();
	//	DirectX::SimpleMath::Vector3 max = b.GetMax();
	//	DirectX::SimpleMath::Vector3 closestPointInAabb = wiMath::Min(wiMath::Max(center, min), max);
	//	double distanceSquared = wiMath::Distance(closestPointInAabb, center);
	//	return distanceSquared < radius;
	//}
	bool Sphere::Intersects(const Sphere& b)const {

		return arithmetic::Distance(center, b.center) <= radius + b.radius;
	}
	bool Sphere::Intersects(const Ray& b) const {
		DirectX::SimpleMath::Vector3 o = XMLoadFloat3(&b.origin);
		DirectX::SimpleMath::Vector3 r = XMLoadFloat3(&b.direction);
		DirectX::SimpleMath::Vector3 dist = XMVector3LinePointDistance(o, o + r, XMLoadFloat3(&center));
		return DirectX::XMVectorGetX(dist) <= radius;
	}


	bool Capsule::Intersects(const Capsule& other, DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& incident_normal, float& penetration_depth) const
	{
		//if (getAABB().intersects(other.getAABB()) == AABB::INTERSECTION_TYPE::OUTSIDE)
		//	return false;

		//XMVECTOR a_Base = XMLoadFloat3(&base);
		//XMVECTOR a_Tip = XMLoadFloat3(&tip);
		//XMVECTOR a_Radius = XMVectorReplicate(radius);
		//XMVECTOR a_Normal = XMVector3Normalize(a_Tip - a_Base);
		//XMVECTOR a_LineEndOffset = a_Normal * a_Radius;
		//XMVECTOR a_A = a_Base + a_LineEndOffset;
		//XMVECTOR a_B = a_Tip - a_LineEndOffset;

		//XMVECTOR b_Base = XMLoadFloat3(&other.base);
		//XMVECTOR b_Tip = XMLoadFloat3(&other.tip);
		//XMVECTOR b_Radius = XMVectorReplicate(other.radius);
		//XMVECTOR b_Normal = XMVector3Normalize(b_Tip - b_Base);
		//XMVECTOR b_LineEndOffset = b_Normal * b_Radius;
		//XMVECTOR b_A = b_Base + b_LineEndOffset;
		//XMVECTOR b_B = b_Tip - b_LineEndOffset;

		//// Vectors between line endpoints:
		//XMVECTOR v0 = b_A - a_A;
		//XMVECTOR v1 = b_B - a_A;
		//XMVECTOR v2 = b_A - a_B;
		//XMVECTOR v3 = b_B - a_B;

		//// Distances (squared) between line endpoints:
		//float d0 = XMVectorGetX(XMVector3LengthSq(v0));
		//float d1 = XMVectorGetX(XMVector3LengthSq(v1));
		//float d2 = XMVectorGetX(XMVector3LengthSq(v2));
		//float d3 = XMVectorGetX(XMVector3LengthSq(v3));

		//// Select best potential endpoint on capsule A:
		//XMVECTOR bestA;
		//if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
		//{
		//	bestA = a_B;
		//}
		//else
		//{
		//	bestA = a_A;
		//}

		//// Select point on capsule B line segment nearest to best potential endpoint on A capsule:
		//XMVECTOR bestB = wiMath::ClosestPointOnLineSegment(b_A, b_B, bestA);

		//// Now do the same for capsule A segment:
		//bestA = wiMath::ClosestPointOnLineSegment(a_A, a_B, bestB);

		//// Finally, sphere collision:
		//XMVECTOR N = bestA - bestB;
		//XMVECTOR len = XMVector3Length(N);
		//N /= len;
		//float dist = XMVectorGetX(len);

		//XMStoreFloat3(&position, bestA - N * radius);
		//XMStoreFloat3(&incident_normal, N);
		//penetration_depth = radius + other.radius - dist;

		//return penetration_depth > 0;
		return true;
	}


	bool Ray::Intersects(const AABB& b) const {
		return b.Intersects(*this);
	}
	bool Ray::Intersects(const Sphere& b) const {
		return b.Intersects(*this);
	}
} // cumulonimbus::primitive