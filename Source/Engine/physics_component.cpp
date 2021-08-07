#include <assert.h>

#include "ecs.h"
#include "physics_component.h"
#include "transform_component.h"

namespace cumulonimbus::component
{
	PhysicsComponent::PhysicsComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{

	}

	void PhysicsComponent::Update(float dt)
	{
		Integrate(dt);
	}

	void PhysicsComponent::RenderImGui()
	{
	}

	void PhysicsComponent::Save(const std::string& file_path)
	{
	}

	void PhysicsComponent::Load(const std::string& file_path_and_name)
	{
	}

	void PhysicsComponent::AddForce(const DirectX::SimpleMath::Vector3& force)
	{
		this->force += force;
	}

	void PhysicsComponent::Integrate(float dt)
	{
		if (mass < 0)
			assert(!"mass is less than zero(PhysicsComponent::Integrate)");

		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		// F = ma : a = m/F
		acceleration = (force / mass);
		velocity.x += transform_comp.GetModelFront().x * acceleration.x * dt;
		velocity.y += 1								   * acceleration.y * dt;
		velocity.z += transform_comp.GetModelFront().z * acceleration.z * dt;
		transform_comp.AdjustPosition(velocity * dt);

		force = DirectX::SimpleMath::Vector3{ 0,0,0 };
	}
} // cumulonimbus::component