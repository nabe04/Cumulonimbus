#include "raycast_component.h"

#include "cereal_helper.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::RayCastComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::CollisionComponent, cumulonimbus::component::RayCastComponent)

namespace cumulonimbus::collision
{
	template <class Archive>
	void Ray::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(hit_result),
			CEREAL_NVP(ray_start),
			CEREAL_NVP(ray_end),
			CEREAL_NVP(ray_offset),
			CEREAL_NVP(block_pos),
			CEREAL_NVP(terrain_attribute),
			CEREAL_NVP(is_block),
			CEREAL_NVP(is_block_hit)
		);
	}

} // cumulonimbus::collision

namespace cumulonimbus::component
{
	template <class Archive>
	void RayCastComponent::serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<CollisionComponent>(this),
			CEREAL_NVP(rays)
		);
	}

	RayCastComponent::RayCastComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent, 
		const CollisionTag tag)
		: CollisionComponent{ registry,ent,tag }
	{

	}

	RayCastComponent::RayCastComponent(
		ecs::Registry* registry, 
		const mapping::rename_type::Entity ent, 
		const RayCastComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}


	void RayCastComponent::RenderImGui()
	{
		if (ImGui::TreeNode("RayCastComponent"))
		{
			for (auto& ray : rays)
			{
				if (ImGui::TreeNode(ray.first.c_str()))
				{
					ImGui::Checkbox("Is Block", &ray.second.is_block);
					ImGui::DragFloat3("RayCast Offset", (float*)&ray.second.ray_offset, 0.01f, -500.0f, 500.0f);
					ImGui::Text("Is Hit %d", ray.second.hit_result.is_hit);
					ImGui::Text("/---Hit Pos---/");
					ImGui::Text("X : %f", ray.second.hit_result.position.x);
					ImGui::Text("Y : %f", ray.second.hit_result.position.y);
					ImGui::Text("Z : %f", ray.second.hit_result.position.z);
					ImGui::Text("/---Hit Normal---/");
					ImGui::Text("X : %f", ray.second.hit_result.position.x);
					ImGui::Text("Y : %f", ray.second.hit_result.position.y);
					ImGui::Text("Z : %f", ray.second.hit_result.position.z);
					ImGui::Text("----------------");
					ImGui::Text("Distance : %f", ray.second.hit_result.distance);
					ImGui::Text("Terrain Attribute : %d", ray.second.terrain_attribute);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}

	void RayCastComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void RayCastComponent::AddRay(const std::string& ray_name, const collision::Ray& ray)
	{
		if(rays.contains(ray_name))
		{
			rays.at(ray_name) = ray;
		}

		rays.insert(std::make_pair(ray_name, ray));
	}

	const DirectX::SimpleMath::Vector3& RayCastComponent::GetRayStartPos(const std::string& ray_name) const
	{
		if(!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::GetRayStartPos)");
		return rays.at(ray_name).ray_start;
	}

	const DirectX::SimpleMath::Vector3& RayCastComponent::GetRayEndPos(const std::string& ray_name) const
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::GetRayEndPos)");
		return rays.at(ray_name).ray_end;
	}

	const DirectX::SimpleMath::Vector3& RayCastComponent::GetRayOffset(const std::string& ray_name) const
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::GetRayOffset)");
		return rays.at(ray_name).ray_offset;
	}

	const DirectX::SimpleMath::Vector3& RayCastComponent::GetBlockPos(const std::string& ray_name) const
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::GetBlockPos)");
		return rays.at(ray_name).block_pos;
	}

	utility::TerrainAttribute RayCastComponent::GetTerrainAttribute(const std::string& ray_name) const
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::GetTerrainAttribute)");
		return rays.at(ray_name).terrain_attribute;
	}

	bool RayCastComponent::GetIsBlock(const std::string& ray_name) const
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::GetIsBlock)");
		return rays.at(ray_name).is_block;
	}

	bool RayCastComponent::GetIsBlockHit(const std::string& ray_name) const
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::GetIsBlockHit)");
		return rays.at(ray_name).is_block_hit;
	}

	std::unordered_map<std::string, collision::Ray>& RayCastComponent::GetRays()
	{
		return rays;
	}

	void RayCastComponent::SetRayStartPos(const std::string& ray_name, const DirectX::SimpleMath::Vector3& pos)
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::SetRayStartPos)");
		rays.at(ray_name).ray_start = pos;
	}

	void RayCastComponent::SetRayEndPos(const std::string& ray_name, const DirectX::SimpleMath::Vector3& pos)
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::SetRayEndPos)");
		rays.at(ray_name).ray_end = pos;
	}

	void RayCastComponent::SetRayOffset(const std::string& ray_name, const DirectX::SimpleMath::Vector3& offset)
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::SetRayOffset)");
		rays.at(ray_name).ray_offset = offset;
	}

	void RayCastComponent::SetBlockPos(const std::string& ray_name, const DirectX::SimpleMath::Vector3& pos)
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::SetBlockPos)");
		rays.at(ray_name).block_pos = pos;
	}


	void RayCastComponent::SetTerrainAttribute(const std::string& ray_name, const utility::TerrainAttribute attribute)
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::SetTerrainAttribute)");
		rays.at(ray_name).terrain_attribute = attribute;
	}

	void RayCastComponent::SetIsBlock(const std::string& ray_name, const bool flg)
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::SetIsBlock)");
		rays.at(ray_name).is_block = flg;
	}

	void RayCastComponent::SetIsBlockHit(const std::string& ray_name, const bool flg)
	{
		if (!rays.contains(ray_name))
			assert(!"Name is not registered(RayCastComponent::SetIsBlockHit)");
		rays.at(ray_name).is_block_hit = flg;
	}
} // cumulonimbus::component