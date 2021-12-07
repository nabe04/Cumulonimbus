#include "rigid_body_component.h"

#include "ecs.h"
#include "transform_component.h"
#include "locator.h"
#include "cum_imgui_helper.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::RigidBodyComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::RigidBodyComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::RigidBodyComponent, 0)

namespace cumulonimbus::component
{
	template<class Archive>
	void RigidBodyComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
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

	template<class Archive>
	void RigidBodyComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
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

	RigidBodyComponent::RigidBodyComponent(
		ecs::Registry* const registry,
		const mapping::rename_type::Entity ent)
		:ComponentBase{ registry ,ent }
	{

	}

	RigidBodyComponent::RigidBodyComponent(
		ecs::Registry* const registry,
		const mapping::rename_type::Entity ent,
		const RigidBodyComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}


	void RigidBodyComponent::PreGameUpdate(float dt)
	{
		auto name = GetRegistry()->GetName(GetEntity());

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
		Integrate(dt);
	}

	void RigidBodyComponent::RenderImGui()
	{
		if(GetRegistry()->CollapsingHeader<RigidBodyComponent>(GetEntity(),"Rigid Body"))
		{
			ImGui::Checkbox("Use Gravity", &is_gravity);
			ImGui::Text("Current Gravity : %f", current_gravity);
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Gravity", &gravity, .5f);
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Jump Strength", &jump_strength, .5f);
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
		// 力を加える
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