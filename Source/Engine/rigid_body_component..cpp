#include "rigid_body_component.h"

#include "ecs.h"
#include "transform_component.h"
#include "locator.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::RigidBodyComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::RigidBodyComponent)

namespace cumulonimbus::component
{
	template<class Archive>
	void RigidBodyComponent::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(velocity),
			CEREAL_NVP(acceleration),

			CEREAL_NVP(mass),
			CEREAL_NVP(jump_strength),
			CEREAL_NVP(gravity),
			CEREAL_NVP(current_gravity),

			CEREAL_NVP(is_gravity),
			CEREAL_NVP(is_landing),
			CEREAL_NVP(is_jumping)
		);
	}

	RigidBodyComponent::RigidBodyComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry ,ent }
	{

	}

	void RigidBodyComponent::PreGameUpdate(float dt)
	{
		if (is_gravity)
		{
			current_gravity += gravity;
			AddForce({ 0,current_gravity,0 });
		}
	}

	void RigidBodyComponent::GameUpdate(float dt)
	{

	}

	void RigidBodyComponent::PostGameUpdate(float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		if (transform_comp.GetPosition().y < 0)
		{
			//transform_comp.SetPositionY(0);
			//current_gravity = 0;
		}
		//if (is_gravity)
		//{
		//	current_gravity += gravity;
		//	AddForce({ 0,current_gravity,0 });
		//}

		Integrate(dt);
	}

	void RigidBodyComponent::RenderImGui()
	{
		if (ImGui::TreeNode("RigidBodyComponent"))
		{
			ImGui::Checkbox("Use Gravity", &is_gravity);
			ImGui::Text("Current Gravity : %f", current_gravity);

			ImGui::TreePop();
		}
	}

	void RigidBodyComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void RigidBodyComponent::AddVelocity(const DirectX::SimpleMath::Vector3& v)
	{
		velocity += v;
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

	void RigidBodyComponent::GravityStop(const bool flg)
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


	void RigidBodyComponent::Integrate(const float dt)
	{
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		transform_comp.AdjustPosition(velocity * dt);
		velocity = DirectX::SimpleMath::Vector3{ 0,0,0 };
	}
} // cumulonimbus::component