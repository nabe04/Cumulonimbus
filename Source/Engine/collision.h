#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#include "collision_component.h"

struct SphereCollisionData;
struct InnerSphereCollisionData;

namespace intersect
{
	enum class EachOtherType
	{
		SphereAndSphere,
		InterSphereAndSphere,

		End,
	};

	bool SphereSphere(const SphereCollisionData& sphereData1, const SphereCollisionData& sphereData2);
	bool InnerSphereSphere(const InnerSphereCollisionData& inner_sphere_data, const SphereCollisionData& outer_sphere_data);
}

class CollisionManager
{
private:

public:
	explicit CollisionManager() = default;
	~CollisionManager() = default;

	void Update(Scene* scene);

	void CollisionTest(Scene* scene);
};