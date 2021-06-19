#pragma once

#include "collision_component.h"
#include "transform.h"
#include "geometric_primitive_resource.h"
#include "component_base.h"

struct SphereCollisionData
{
	DirectX::SimpleMath::Matrix  world_matrix;
	DirectX::SimpleMath::Matrix  bone_matrix;
	DirectX::SimpleMath::Vector3 ajust_pos;
	XMFLOAT3		color_RGB;
	float			radius;
	bool			isHit = false;
};

namespace cumulonimbus::component
{
	class SphereCollisionComponent : public CollisionComponent
	{
	private:
		std::vector<SphereCollisionData> sphere_datas;

		//-- Debug primitive info --//
		geometric_primitive_resource::Mesh mesh;

	private:
		bool IntersectSphere(component::SphereCollisionComponent* other) override;

	public:
		explicit SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
		explicit SphereCollisionComponent() = default;
		~SphereCollisionComponent() = default;

		void Update(const float delta_time)override;

		void OnCollision(const CollisionComponent* other) override;

		void AddSphere(XMFLOAT3 position, float radius);

		/*
		* brief : Set all radius
		*/
		void SetRadiusAll(const float radius);

		/*
		* brief : Set the radius by specifying an index number
		*/
		void SetRadius(const float radius, const int index);

		const geometric_primitive_resource::Mesh& GetMesh() { return mesh; }
		std::vector<SphereCollisionData>& GetSphereData() { return sphere_datas; }

		// For ImGui
		void RenderImGui() override;
	};
}


CEREAL_REGISTER_TYPE(cumulonimbus::component::SphereCollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::CollisionComponent, cumulonimbus::component::SphereCollisionComponent)