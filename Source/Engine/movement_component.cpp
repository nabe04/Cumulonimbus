#include "movement_component.h"

#include "ecs.h"
#include "transform_component.h"
#include "locator.h"


namespace cumulonimbus::component
{
	MovementComponent::MovementComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry ,ent }
	{

	}

	void MovementComponent::Update(float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		if (transform_comp.GetPosition().y < 0)
		{
			transform_comp.SetPositionY(0);
			current_gravity = 0;
		}

		if(is_gravity)
		{
			current_gravity += gravity;
			AddForce({ 0,current_gravity,0 });
		}

		Integrate(dt);
	}

	void MovementComponent::RenderImGui()
	{
	}

	void MovementComponent::Save(const std::string& file_path)
	{
	}

	void MovementComponent::Load(const std::string& file_path_and_name)
	{
	}

	void MovementComponent::AddForce(const DirectX::SimpleMath::Vector3& force)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		velocity.x += transform_comp.GetModelFront().x * force.x;
		velocity.y += 1								   * force.y;
		velocity.z += transform_comp.GetModelFront().z * force.z;
	}

	void MovementComponent::Jump(const float strength)
	{
		if (strength < 0)
		{
			current_gravity = jump_strength;
		}
		else
		{
			current_gravity = strength;
		}

		AddForce({ 0,current_gravity,0 });
	}

	void MovementComponent::JumpStop(bool flg)
	{
		if(flg)
		{
			is_gravity = false;
			current_gravity = 0.0f;
		}
		else
		{
			is_gravity = true;
		}
	}


	void MovementComponent::Integrate(float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		transform_comp.AdjustPosition(velocity * dt);

		velocity = DirectX::SimpleMath::Vector3{ 0,0,0 };
	}
} // cumulonimbus::component