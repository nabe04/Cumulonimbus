#include "inner_sphere_collision.h"

#include "scene.h"
#include "view.h"
#include "geometric_primitive_resource.h"
#include "transform.h"
#include "transform_component.h"
#include "collision.h"
#include "sphere_collision_component.h"

InnerSpherecollision_component::InnerSpherecollision_component(Entity* entity, CollisionTag tag)
	:collision_component{ entity ,tag }
{
	type = CollisionType::InnerSphere;
	mesh = GetEntity()->GetScene()->GetGeomPrimRes()->GetMeshData(GeomPrimType::Sphere);
}

bool InnerSpherecollision_component::IntersectInnerSphere(Spherecollision_component* other)
{
	bool isHit = false;

	for (auto& my_data : inner_sphere_datas)
	{
		for (auto& other_data : other->GetSphereData())
		{
			if (intersect::InnerSphereSphere(my_data, other_data))
			{
#ifdef _DEBUG
				isHit = true;
				my_data.isHit		= other_data.isHit = true;
				my_data.color_RGB	= other_data.color_RGB = color_red;
				return true;
#else
				my_data.isHit = other_data.isHit = true;
				return true;
#endif
			}
			else
			{
				// Color of the judgment range
				my_data.color_RGB = color_blue;
			}
		}
	}

#ifdef _DEBUG
	for (auto& other_data : other->GetSphereData())
	{
		if (!isHit)
			other_data.color_RGB = color_blue;
	}
#else


#endif

	return isHit;
}

void InnerSpherecollision_component::Update(const float delta_time)
{
	auto ent_world_pos = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetPosition();
	const auto& ent_transform_matrix = GetEntity()->GetComponent<TransformComponent>()->GetTransform()->TransformMat();
	XMMATRIX ent_world_translate_matrix = XMMatrixTranslation(ent_world_pos.x, ent_world_pos.y, ent_world_pos.z);

	XMMATRIX ent_world_matrix = XMLoadFloat4x4(&GetEntity()->GetComponent<TransformComponent>()->GetTransform()->GetWorld4x4());

	for (auto& data : inner_sphere_datas)
	{
		// Scaling
		DirectX::SimpleMath::Matrix s = XMMatrixScaling(data.radius * 2, data.radius * 2, data.radius * 2);
		// Rotation
		DirectX::SimpleMath::Matrix r = XMMatrixIdentity();
		// Parallel movement
		DirectX::SimpleMath::Matrix t;
		t.Translation(data.ajust_pos);
		// Local matrix
		DirectX::SimpleMath::Matrix model_local_matrix = s * r * t;
		// World matrix
		data.world_matrix = model_local_matrix * ent_world_matrix;

	}
}

void InnerSpherecollision_component::AddSphere(DirectX::SimpleMath::Vector3 local_position, float radius)
{
	DirectX::SimpleMath::Matrix matrix;
	matrix.Identity;
	matrix.Translation(local_position);

	InnerSphereCollisionData sphere_data{ matrix, local_position, color_blue, radius ,false };
	inner_sphere_datas.emplace_back(sphere_data);
}

void InnerSpherecollision_component::SetRadiusAll(const float radius)
{
	for (auto data : inner_sphere_datas)
	{
		data.radius = radius;
	}
}

void InnerSpherecollision_component::SetRadius(const float radius, const int index)
{
	inner_sphere_datas.at(index).radius = radius;
}

void InnerSpherecollision_component::RenderImGui()
{
	if (ImGui::TreeNode("Collision(Sphere)"))
	{
		int no = 0;

		for (auto& data : inner_sphere_datas)
		{
			if (ImGui::TreeNode((void*)(intptr_t)no, "Info %d", no))
			{
				ImGui::DragFloat3("Position", (float*)&data.ajust_pos, 0.01f, -100.0f, 100.0f);
				ImGui::DragFloat("Radius", &data.radius, 0.1f, 0.1f, 50);

				ImGui::TreePop();
			}
			no++;
		}
		ImGui::TreePop();
	}
}
