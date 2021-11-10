#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

namespace cumulonimbus::primitive
{
	struct Sphere;
	struct Ray;
	struct AABB;

	struct AABB
	{
		enum INTERSECTION_TYPE
		{
			OUTSIDE,
			INTERSECTS,
			INSIDE,
		};

		DirectX::SimpleMath::Vector3 min{};
		DirectX::SimpleMath::Vector3 max{};

		explicit AABB(
			const DirectX::SimpleMath::Vector3& min = DirectX::SimpleMath::Vector3(FLT_MAX, FLT_MAX, FLT_MAX),
			const DirectX::SimpleMath::Vector3& max = DirectX::SimpleMath::Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX))
		: min(min), max(max) {}
		void CreateFromHalfWidth(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& half_width);
		AABB Transform(const DirectX::SimpleMath::Matrix& mat) const;
		//AABB transform(const XMFLOAT4X4& mat) const;
		DirectX::SimpleMath::Vector3 GetCenter() const;
		DirectX::SimpleMath::Vector3 GetHalfWidth() const;
		DirectX::SimpleMath::Matrix GetAsBoxMatrix() const;
		float GetArea() const;
		float GetRadius() const;
		INTERSECTION_TYPE Gntersects2D(const AABB& b) const;
		INTERSECTION_TYPE Gntersects(const AABB& b) const;
		bool Intersects(const DirectX::SimpleMath::Vector3& p) const;
		bool Intersects(const Ray& ray) const;
		bool Intersects(const Sphere& sphere) const;
		bool Intersects(const DirectX::BoundingFrustum& frustum) const;
		AABB operator* (float a);
		static AABB Merge(const AABB& a, const AABB& b);

		DirectX::SimpleMath::Vector3 GetMin() const { return min; }
		DirectX::SimpleMath::Vector3 GetMax() const { return max; }
		DirectX::SimpleMath::Vector3 Corner(const int index) const
		{
			switch (index)
			{
			case 0: return min;
			case 1: return DirectX::SimpleMath::Vector3(min.x, max.y, min.z);
			case 2: return DirectX::SimpleMath::Vector3(min.x, max.y, max.z);
			case 3: return DirectX::SimpleMath::Vector3(min.x, min.y, max.z);
			case 4: return DirectX::SimpleMath::Vector3(max.x, min.y, min.z);
			case 5: return DirectX::SimpleMath::Vector3(max.x, max.y, min.z);
			case 6: return max;
			case 7: return DirectX::SimpleMath::Vector3(max.x, min.y, max.z);
			}
			assert(0);
			return DirectX::SimpleMath::Vector3(0, 0, 0);
		}
	};
	struct Sphere
	{
		DirectX::SimpleMath::Vector3 center;
		float radius;
		Sphere() :center(DirectX::SimpleMath::Vector3(0, 0, 0)), radius(0) {}
		Sphere(const DirectX::SimpleMath::Vector3& c, const float r) :center(c), radius(r) {}
		//bool Intersects(const AABB& b) const;
		bool Intersects(const Sphere& b) const;
		bool Intersects(const Ray& b) const;
	};
	struct Capsule
	{
		DirectX::SimpleMath::Vector3 base {DirectX::SimpleMath::Vector3(0, 0, 0)};
		DirectX::SimpleMath::Vector3 tip  {DirectX::SimpleMath::Vector3(0, 0, 0)};
		float radius{ 0 };
		Capsule() = default;
		Capsule(
			const DirectX::SimpleMath::Vector3& base,
			const DirectX::SimpleMath::Vector3& tip, float radius)
		:base(base), tip(tip), radius(radius) {}
		Capsule(const Sphere& sphere, const float height)
			:base(DirectX::SimpleMath::Vector3(sphere.center.x, sphere.center.y - sphere.radius, sphere.center.z)),
			tip(DirectX::SimpleMath::Vector3(base.x, base.y + height, base.z)),
			radius(sphere.radius)
		{}
		AABB GetAABB() const
		{
			const DirectX::SimpleMath::Vector3 half_width = DirectX::SimpleMath::Vector3(radius, radius, radius);
			AABB base_aabb;
			base_aabb.CreateFromHalfWidth(base, half_width);
			AABB tip_aabb;
			tip_aabb.CreateFromHalfWidth(tip, half_width);
			return AABB::Merge(base_aabb, tip_aabb);
		}
		bool Intersects(
			const Capsule& b,
			DirectX::SimpleMath::Vector3& position,
			DirectX::SimpleMath::Vector3& incident_normal,
			float& penetration_depth) const;
	};
	struct Ray
	{
		DirectX::SimpleMath::Vector3 origin, direction, direction_inverse;

		Ray(
			const DirectX::SimpleMath::Vector3& new_origin    = DirectX::SimpleMath::Vector3(0, 0, 0),
			const DirectX::SimpleMath::Vector3& new_direction = DirectX::SimpleMath::Vector3(0, 0, 1))
			: origin{ new_origin },
			  direction{ new_direction }{}
		bool Intersects(const AABB& b) const;
		bool Intersects(const Sphere& b) const;
	};

} // cumulonimbus::primitive