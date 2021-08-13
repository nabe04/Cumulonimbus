#include "rigid_body_component.h"

#include "ecs.h"
#include "transform_component.h"
#include "locator.h"


namespace cumulonimbus::component
{
	RigidBodyComponent::RigidBodyComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry ,ent }
	{

	}

	void RigidBodyComponent::NewFrame(float dt)
	{
		velocity = DirectX::SimpleMath::Vector3{ 0,0,0 };
	}

	void RigidBodyComponent::Update(float dt)
	{

	}

	void RigidBodyComponent::PostUpdate(float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		if (transform_comp.GetPosition().y < 0)
		{
			transform_comp.SetPositionY(0);
			current_gravity = 0;
		}

		if (is_gravity)
		{
			current_gravity += gravity;
			AddForce({ 0,current_gravity,0 });
		}

		Integrate(dt);
	}

	void RigidBodyComponent::RenderImGui()
	{

	}

	void RigidBodyComponent::Save(const std::string& file_path)
	{
	}

	void RigidBodyComponent::Load(const std::string& file_path_and_name)
	{
	}

	void RigidBodyComponent::AddForce(const DirectX::SimpleMath::Vector3& force)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		velocity.x += transform_comp.GetModelFront().x * force.x;
		velocity.y += 1								   * force.y;
		velocity.z += transform_comp.GetModelFront().z * force.z;
	}

	void RigidBodyComponent::Jump(const float strength)
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

	void RigidBodyComponent::JumpStop(bool flg)
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


	void RigidBodyComponent::Integrate(float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		transform_comp.AdjustPosition(velocity * dt);
	}
} // cumulonimbus::component