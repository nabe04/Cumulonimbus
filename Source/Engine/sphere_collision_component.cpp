#include "sphere_collision_component.h"

#include "scene.h"
#include "light.h"
#include "view.h"
#include "component_list.h"
#include "geometric_primitive_resource.h"
#include "transform.h"
#include "transform_component.h"
#include "collision.h"

Spherecollision_component::Spherecollision_component(Entity* entity, CollisionTag tag)
	:collision_component{ entity ,tag }
{
	type = CollisionType::Sphere;
	mesh = GetEntity()->GetScene()->GetGeomPrimRes()->GetMeshData(GeomPrimType::Sphere);
}

void Spherecollision_component::Update(const float delta_time)
{
	auto ent_world_pos = GetEntity()->GetComponent<TransformComponent>()->GetPosition();
	const auto& ent_transform_matrix = GetEntity()->GetComponent<TransformComponent>()->TransformMat();
	XMMATRIX ent_world_translate_matrix = XMMatrixTranslation(ent_world_pos.x, ent_world_pos.y, ent_world_pos.z);

	XMMATRIX ent_world_matrix = XMLoadFloat4x4(&GetEntity()->GetComponent<TransformComponent>()->GetWorld4x4());

	for (auto& data : sphere_datas)
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
		if(data.bone_matrix == DirectX::SimpleMath::Matrix::Identity)
		{
			data.world_matrix = model_local_matrix * ent_world_matrix;
		}
		else
		{
			data.world_matrix = model_local_matrix * data.bone_matrix;
		}
	}
}

void Spherecollision_component::OnCollision(const collision_component* other)
{

}

bool Spherecollision_component::IntersectSphere(Spherecollision_component* other)
{
	bool isHit = false;

	for (auto& my_data : sphere_datas)
	{
		for (auto& other_data : other->GetSphereData())
		{
			if (intersect::SphereSphere(my_data, other_data))
			{
				isHit = true;
				my_data.color_RGB = other_data.color_RGB = color_red;
				my_data.isHit = other_data.isHit = true;
		#ifndef _DEBUG
					return true;
		#endif
					//	my_data.isHit = other_data.isHit = true;
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


void Spherecollision_component::AddSphere(XMFLOAT3 local_position, float radius)
{
	DirectX::SimpleMath::Matrix matrix;
	matrix.Identity;
	matrix.Translation(local_position);

	SphereCollisionData sphere_data{ matrix, {},local_position, color_blue, radius ,false };
	sphere_datas.emplace_back(sphere_data);
}

void Spherecollision_component::SetRadiusAll(const float radius)
{
	for (auto& data : sphere_datas)
	{
		data.radius = radius;
	}
}

void Spherecollision_component::SetRadius(const float radius, const int index)
{
	sphere_datas.at(index).radius = radius;
}

void Spherecollision_component::RenderImGui()
{
	if (ImGui::TreeNode("Collision(Sphere)"))
	{
		int no = 0;

		for (auto& data : sphere_datas)
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


namespace cumulonimbus::component
{
	SphereCollisionComponent::SphereCollisionComponent(ecs::Registry* registry, ecs::Entity ent, CollisionTag tag)
		:CollisionComponent{ registry , ent ,tag }
	{
		type = CollisionType::Sphere;
		mesh = GetRegistry()->GetScene()->GetGeomPrimRes()->GetMeshData(GeomPrimType::Sphere);
	}

	void SphereCollisionComponent::Update(const float delta_time)
	{
		auto ent_world_pos = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();
		const auto& ent_transform_matrix = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).TransformMat();
		XMMATRIX ent_world_translate_matrix = XMMatrixTranslation(ent_world_pos.x, ent_world_pos.y, ent_world_pos.z);

		XMMATRIX ent_world_matrix = XMLoadFloat4x4(&GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorld4x4());

		for (auto& data : sphere_datas)
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
			if (data.bone_matrix == DirectX::SimpleMath::Matrix::Identity)
			{
				data.world_matrix = model_local_matrix * ent_world_matrix;
			}
			else
			{
				data.world_matrix = model_local_matrix * data.bone_matrix;
			}
		}
	}

	void SphereCollisionComponent::OnCollision(const CollisionComponent* other)
	{

	}

	bool SphereCollisionComponent::IntersectSphere(SphereCollisionComponent* other)
	{
		bool isHit = false;

		for (auto& my_data : sphere_datas)
		{
			for (auto& other_data : other->GetSphereData())
			{
				if (intersect::SphereSphere(my_data, other_data))
				{
					isHit = true;
					my_data.color_RGB = other_data.color_RGB = color_red;
					my_data.isHit = other_data.isHit = true;
#ifndef _DEBUG
					return true;
#endif
					//	my_data.isHit = other_data.isHit = true;
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


	void SphereCollisionComponent::AddSphere(XMFLOAT3 local_position, float radius)
	{
		DirectX::SimpleMath::Matrix matrix;
		matrix.Identity;
		matrix.Translation(local_position);

		SphereCollisionData sphere_data{ matrix, {},local_position, color_blue, radius ,false };
		sphere_datas.emplace_back(sphere_data);
	}

	void SphereCollisionComponent::SetRadiusAll(const float radius)
	{
		for (auto& data : sphere_datas)
		{
			data.radius = radius;
		}
	}

	void SphereCollisionComponent::SetRadius(const float radius, const int index)
	{
		sphere_datas.at(index).radius = radius;
	}

	void SphereCollisionComponent::RenderImGui()
	{
		if (ImGui::TreeNode("Collision(Sphere)"))
		{
			int no = 0;

			for (auto& data : sphere_datas)
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
}