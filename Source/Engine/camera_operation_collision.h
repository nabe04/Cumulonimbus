#pragma once

#include "inner_sphere_collision.h"

class CameraOperationCollisionComponent : public InnerSphereCollisionComponent
{
public:
	CameraOperationCollisionComponent(Entity* entity, CollisionTag tag);

	void Update(const float delta_time) override;
	void OnCollision(const CollisionComponent* other) override;
	void RenderImgui() override;
};