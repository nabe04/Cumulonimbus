#pragma once

#include "collision_component.h"
#include "transform.h"
#include "geometric_primitive_resource.h"

struct SphereCollisionData
{
	DirectX::SimpleMath::Matrix  world_matrix;
	DirectX::SimpleMath::Matrix  bone_matrix;
	DirectX::SimpleMath::Vector3 ajust_pos;
	XMFLOAT3		color_RGB;
	float			radius;
	bool			isHit = false;
};

class Spherecollision_component : public collision_component
{
private:
	std::vector<SphereCollisionData> sphere_datas;

	//-- Debug primitive info --//
	geometric_primitive_resource::Mesh mesh;

private:
	bool IntersectSphere(Spherecollision_component* other) override;

public:
	Spherecollision_component(Entity* entity, CollisionTag tag);

	void Update(const float delta_time)override;

	void OnCollision(const collision_component* other) override;

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