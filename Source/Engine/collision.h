#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#include "collision_component.h"

struct SphereCollisionData;

namespace intersect
{
	enum class EachOtherType
	{
		SphereAndSphere,
		InterSphereAndSphere,

		End,
	};

	bool SphereSphere(const SphereCollisionData& sphereData1, const SphereCollisionData& sphereData2);
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