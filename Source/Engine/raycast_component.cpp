#include "raycast_component.h"

namespace cumulonimbus::component
{
	RayCastComponent::RayCastComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag)
		: CollisionComponent{ registry,ent,tag }
	{

	}

	void RayCastComponent::RenderImGui()
	{
		if (ImGui::TreeNode("RayCastComponent"))
		{
			ImGui::DragFloat3("RayCast Offset", (float*)&ray_offset, 0.01f, -50.0f, 50.0f);
			ImGui::Text("Is Hit %d", hit_result.is_hit);
			ImGui::Text("/---Hit Pos---/");
			ImGui::Text("X : %f", hit_result.position.x);
			ImGui::Text("Y : %f", hit_result.position.y);
			ImGui::Text("Z : %f", hit_result.position.z);
			ImGui::Text("/---Hit Normal---/");
			ImGui::Text("X : %f", hit_result.position.x);
			ImGui::Text("Y : %f", hit_result.position.y);
			ImGui::Text("Z : %f", hit_result.position.z);
			ImGui::Text("----------------");
			ImGui::Text("Distance : %f", hit_result.distance);
			ImGui::Text("Terrain Attribute : %d", terrain_attribute);
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

	utility::TerrainAttribute RayCastComponent::GetTerrainAttribute() const
	{
		return terrain_attribute;
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

	void RayCastComponent::SetTerrainAttribute(const utility::TerrainAttribute attribute)
	{
		terrain_attribute = attribute;
	}

} // cumulonimbus::component