#pragma once
#include "ecs.h"
#include "transform.h"
#include "transform_component.h"

class SampleRotation : public Component
{
private:
	float angle = 0;
public:
	SampleRotation(Entity* entity):Component(entity){}

	void Update(const float delta_time) override
	{
		angle += 0.5f;
		if (angle > 360)
			angle = 0;

		GetEntity()->GetComponent<TransformComponent>()->SetWorldRotation_Y(angle);
	}
};