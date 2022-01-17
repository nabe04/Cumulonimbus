#include "actor3d_component.h"

#include "arithmetic.h"
#include "transform_component.h"

namespace cumulonimbus::component
{
	Actor3DComponent::Actor3DComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		: ComponentBase{ registry,ent }
	{

	}

	Actor3DComponent::Actor3DComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const Actor3DComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	Actor3DComponent::Actor3DComponent(const mapping::component_tag::ComponentTag tag)
		: ComponentBase{ tag }
	{

	}

	void Actor3DComponent::Rotate(ecs::Registry* registry, const mapping::rename_type::Entity& ent)
	{
		const std::string name = registry->GetName(ent);

		if (name.empty())
			return;

		const DirectX::SimpleMath::Vector3 ent_pos = registry->GetComponent<TransformComponent>(ent).GetPosition();
		auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		DirectX::SimpleMath::Vector3 self_to_player{ ent_pos - transform_comp.GetPosition() };
		self_to_player.y = 0;
		self_to_player.Normalize();
		DirectX::SimpleMath::Vector3 model_front = transform_comp.GetModelFront();
		model_front.y = 0;
		model_front.Normalize();

		float dot = model_front.Dot(self_to_player);
		dot = arithmetic::Clamp(dot, -1.0f, 1.0f);
		float rad = acosf(dot);
		if (model_front.Cross(self_to_player).y < 0)
			rad *= -1;

		transform_comp.AdjustRotationFromAxis({ 0,1,0 }, rad);
	}

} // cumulonimbus::component
