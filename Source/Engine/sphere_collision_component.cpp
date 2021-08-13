#include "sphere_collision_component.h"

#include <cassert>

#include "ecs.h"
#include "transform_component.h"
#include "fbx_model_resource.h"
#include "fbx_model_component.h"

namespace cumulonimbus::component
{
	SphereCollisionComponent::SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag)
		:CollisionComponent{ registry,ent ,tag }
	{
	}

	void SphereCollisionComponent::NewFrame(float dt)
	{
	}

	void SphereCollisionComponent::Update(float dt)
	{

	}

	void SphereCollisionComponent::PostUpdate(float dt)
	{
		// 判定用(球)データの更新
		for (auto& sphere : spheres)
		{
			// Scaling
			const DirectX::SimpleMath::Matrix s = DirectX::XMMatrixScaling(sphere.second.radius, sphere.second.radius, sphere.second.radius);
			// Rotation
			const DirectX::SimpleMath::Matrix r = DirectX::XMMatrixIdentity();
			// Parallel movement
			DirectX::SimpleMath::Matrix t		= DirectX::SimpleMath::Matrix::Identity;
			t.Translation(sphere.second.offset);
			// Local matrix
			const DirectX::SimpleMath::Matrix model_local_matrix = s * r * t;

			if(sphere.second.bone_name.empty())
			{
				// モデルが持つワールド変換行列
				DirectX::SimpleMath::Matrix world_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorld4x4();
				world_transform._11 = world_transform._22 = world_transform._33 = world_transform._44 = 1.0f;
				sphere.second.world_transform_matrix = model_local_matrix * world_transform;
			}
			else
			{
				sphere.second.world_transform_matrix = model_local_matrix * GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).GetNodeMatrix(sphere.second.bone_name.c_str());
			}
		}
	}


	void SphereCollisionComponent::RenderImGui()
	{

	}

	void SphereCollisionComponent::Save(const std::string& file_path)
	{
	}

	void SphereCollisionComponent::Load(const std::string& file_path_and_name)
	{
	}

	std::string SphereCollisionComponent::AddSphere(const std::string& name, const collision::Sphere& sphere)
	{
		if(name == "")
		{// 名前の指定がない場合は「sphere(番号)」という名前にする
			int no = spheres.size();
			std::string new_name{};
			new_name = "CollisionTag tag(" + std::to_string(no) + ")";
			while(true)
			{
				if (spheres.contains(new_name))
				{// 名前の重複があったので番号を+1する
					++no;
				}
				else
				{
					spheres.emplace(new_name, sphere);
					return new_name;
				}
			}
		}
		else
		{
			if (spheres.contains(name))
				assert((!"The sphere name already exists(SphereCollisionComponent::AddSphere)"));

			spheres.emplace(name, sphere);
			return name;
		}
	}

	std::string SphereCollisionComponent::AddSphereAndRegisterBoneName(
		const std::string&		 bone_name,
		const std::string&		 sphere_name,
		const collision::Sphere& sphere)
	{
		const std::string name = AddSphere(sphere_name, sphere);
		spheres.at(name).bone_name = bone_name;
		return name;
	}

	void SphereCollisionComponent::SetOffset(const std::string& sphere_name, const DirectX::SimpleMath::Vector3& offset)
	{
		if (!spheres.contains(sphere_name))
			assert(!"Name is not registered(SphereCollisionComponent::SetOffset)");
		spheres.at(sphere_name).offset = offset;
	}

	void SphereCollisionComponent::SetAllOffset(const DirectX::SimpleMath::Vector3& offset)
	{
		for(auto& sphere : spheres)
		{
			sphere.second.offset = offset;
		}
	}

	void SphereCollisionComponent::SetRadius(const std::string& sphere_name, float radius)
	{
		if (!spheres.contains(sphere_name))
			assert(!"Name is not registered(SphereCollisionComponent::SetOffset)");
		spheres.at(sphere_name).radius = radius;
	}

	void SphereCollisionComponent::SetAllRadius(float radius)
	{
		for (auto& sphere : spheres)
		{
			sphere.second.radius = radius;
		}
	}

	const std::unordered_map<std::string, collision::Sphere>& SphereCollisionComponent::GetSpheres() const
	{
		return spheres;
	}

} // cumulonimbus::component

//
//#include <cereal/types/bitset.hpp>
//
//#include "scene.h"
//#include "geometric_primitive_resource.h"
//#include "transform_component.h"
//#include "collision.h"
//
//namespace cumulonimbus::component
//{
//	SphereCollisionComponent::SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag)
//		:CollisionComponent{ registry , ent ,tag }
//	{
//		type = CollisionType::Sphere;
//		mesh = GetRegistry()->GetScene()->GetGeomPrimRes()->GetMeshData(GeomPrimType::Sphere);
//	}
//
//	void SphereCollisionComponent::Update(const float delta_time)
//	{
//		auto ent_world_pos = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();
//		const auto& ent_transform_matrix = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).TransformMat();
//		XMMATRIX ent_world_translate_matrix = XMMatrixTranslation(ent_world_pos.x, ent_world_pos.y, ent_world_pos.z);
//
//		XMMATRIX ent_world_matrix = XMLoadFloat4x4(&GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorld4x4());
//
//		for (auto& data : sphere_datas)
//		{
//			// Scaling
//			DirectX::SimpleMath::Matrix s = XMMatrixScaling(data.radius * 2, data.radius * 2, data.radius * 2);
//			// Rotation
//			DirectX::SimpleMath::Matrix r = XMMatrixIdentity();
//			// Parallel movement
//			DirectX::SimpleMath::Matrix t;
//			t.Translation(data.ajust_pos);
//			// Local matrix
//			DirectX::SimpleMath::Matrix model_local_matrix = s * r * t;
//
//			// World matrix
//			if (data.bone_matrix == DirectX::SimpleMath::Matrix::Identity)
//			{
//				data.world_matrix = model_local_matrix * ent_world_matrix;
//			}
//			else
//			{
//				data.world_matrix = model_local_matrix * data.bone_matrix;
//			}
//		}
//	}
//
//	void SphereCollisionComponent::OnCollision(const CollisionComponent* other)
//	{
//
//	}
//
//	bool SphereCollisionComponent::IntersectSphere(SphereCollisionComponent* other)
//	{
//		bool isHit = false;
//
//		for (auto& my_data : sphere_datas)
//		{
//			for (auto& other_data : other->GetSphereData())
//			{
//				if (intersect::SphereSphere(my_data, other_data))
//				{
//					isHit = true;
//					my_data.color_RGB = other_data.color_RGB = color_red;
//					my_data.isHit = other_data.isHit = true;
//#ifndef _DEBUG
//					return true;
//#endif
//					//	my_data.isHit = other_data.isHit = true;
//				}
//				else
//				{
//					// Color of the judgment range
//					my_data.color_RGB = color_blue;
//				}
//			}
//		}
//
//#ifdef _DEBUG
//		for (auto& other_data : other->GetSphereData())
//		{
//			if (!isHit)
//				other_data.color_RGB = color_blue;
//		}
//#else
//
//
//#endif
//
//		return isHit;
//	}
//
//
//	void SphereCollisionComponent::AddSphere(XMFLOAT3 local_position, float radius)
//	{
//		DirectX::SimpleMath::Matrix matrix;
//		matrix.Identity;
//		matrix.Translation(local_position);
//
//		SphereCollisionData sphere_data{ matrix, {},local_position, color_blue, radius ,false };
//		sphere_datas.emplace_back(sphere_data);
//	}
//
//	void SphereCollisionComponent::SetRadiusAll(const float radius)
//	{
//		for (auto& data : sphere_datas)
//		{
//			data.radius = radius;
//		}
//	}
//
//	void SphereCollisionComponent::SetRadius(const float radius, const int index)
//	{
//		sphere_datas.at(index).radius = radius;
//	}
//
//	void SphereCollisionComponent::RenderImGui()
//	{
//		if (ImGui::TreeNode("Collision(Sphere)"))
//		{
//			int no = 0;
//
//			for (auto& data : sphere_datas)
//			{
//				if (ImGui::TreeNode((void*)(intptr_t)no, "Info %d", no))
//				{
//					ImGui::DragFloat3("Position", (float*)&data.ajust_pos, 0.01f, -100.0f, 100.0f);
//					ImGui::DragFloat("Radius", &data.radius, 0.1f, 0.1f, 50);
//
//					ImGui::TreePop();
//				}
//				no++;
//			}
//			ImGui::TreePop();
//		}
//	}
//}