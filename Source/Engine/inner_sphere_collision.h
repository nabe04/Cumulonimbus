#pragma once

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "collision_component.h"
#include "geometric_primitive_resource.h"

struct InnerSphereCollisionData
{
	DirectX::SimpleMath::Matrix  world_matrix;
	DirectX::SimpleMath::Vector3 ajust_pos;
	XMFLOAT3		color_RGB;
	float			radius;
	bool			isHit = false;
};

class InnerSpherecollision_component : public collision_component
{
private:
	std::vector<InnerSphereCollisionData> inner_sphere_datas;

	//-- Debug primitive info --//
	geometric_primitive_resource::Mesh mesh;

private:
	bool IntersectInnerSphere(Spherecollision_component* other) override;

public:
	InnerSpherecollision_component(Entity* entity, CollisionTag tag);

	void Update(const float delta_time) override;
	void OnCollision(const collision_component* other) override {};
	void AddSphere(DirectX::SimpleMath::Vector3 position, float radius);

	/*
	* brief : Set all radius
	*/
	void SetRadiusAll(const float radius);

	/*
	* brief : Set the radius by specifying an index number
	*/
	void SetRadius(const float radius, const int index);

	const geometric_primitive_resource::Mesh& GetMesh() { return mesh; }
	std::vector<InnerSphereCollisionData>& GetSphereData() { return inner_sphere_datas; }

	// For ImGui
	void RenderImGui() override;
};