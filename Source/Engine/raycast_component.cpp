#include "raycast_component.h"

namespace cumulonimbus::component
{
	RayCastComponent::RayCastComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag)
		: CollisionComponent{ registry,ent,tag }
	{

	}

	void RayCastComponent::RenderImGui()
	{
		if (ImGui::TreeNode("PlayerComponent"))
		{
			ImGui::DragFloat3("RayCast Offset", (float*)&ray_offset, 0.01f, -50.0f, 50.0f);
			
			ImGui::TreePop();
		}
	}


	const DirectX::SimpleMath::Vector3& RayCastComponent::GetRayStartPos() const
	{
		return ray_start;
	}

	const DirectX::SimpleMath::Vector3& RayCastComponent::GetRayEndPos() const
	{
		return ray_end;
	}

	const DirectX::SimpleMath::Vector3& RayCastComponent::GetRayOffset() const
	{
		return ray_offset;
	}

	void RayCastComponent::SetRayStartPos(const DirectX::SimpleMath::Vector3& pos)
	{
		ray_start = pos;
	}

	void RayCastComponent::SetRayEndPos(const DirectX::SimpleMath::Vector3& pos)
	{
		ray_end   = pos;
	}

	void RayCastComponent::SetRayOffset(const DirectX::SimpleMath::Vector3& offset)
	{
		ray_offset = offset;
	}
} // cumulonimbus::component