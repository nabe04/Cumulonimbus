#include "capsule_collison_component.h"


#include <cassert>

#include "ecs.h"
#include "transform_component.h"
#include "fbx_model_resource.h"
#include "fbx_model_component.h"

namespace cumulonimbus::component
{
	CapsuleCollisionComponent::CapsuleCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag)
		:CollisionComponent{ registry,ent,tag }
	{
	}

	void CapsuleCollisionComponent::NewFrame(float dt)
	{
	}

	void CapsuleCollisionComponent::Update(float dt)
	{
	}

	void CapsuleCollisionComponent::PostUpdate(float dt)
	{
		// 判定用(カプセル)データの更新
		for(auto& capsule : capsules)
		{
			// Scaling
			//const DirectX::SimpleMath::Matrix s = DirectX::XMMatrixScaling(capsule.second.radius, capsule.second.length * 0.5f, capsule.second.radius);
			const DirectX::SimpleMath::Matrix s = DirectX::SimpleMath::Matrix::Identity;
			// Rotation
			//const DirectX::SimpleMath::Matrix r = DirectX::XMMatrixRotationRollPitchYawFromVector(capsule.second.rotation);
			const DirectX::SimpleMath::Matrix r = DirectX::SimpleMath::Matrix::Identity;
			// Parallel movement
			const DirectX::SimpleMath::Matrix t = DirectX::XMMatrixTranslation(capsule.second.offset.x, capsule.second.offset.y, capsule.second.offset.z);
			// Local matrix
			const DirectX::SimpleMath::Matrix model_local_matrix = s * r * t;
			if (capsule.second.bone_name.empty())
			{
				// モデルが持つワールド変換行列
				DirectX::SimpleMath::Matrix world_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorld4x4();
				world_transform._11 = world_transform._22 = world_transform._33 = world_transform._44 = 1.0f;
				capsule.second.world_transform_matrix = model_local_matrix * world_transform;
			}
			else
			{
				capsule.second.world_transform_matrix = model_local_matrix * GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).GetNodeMatrix(capsule.second.bone_name.c_str());
			}

			capsule.second.start = capsule.second.world_transform_matrix.Up() * -(capsule.second.world_transform_matrix.Translation() * 0.5f);
			capsule.second.end   = capsule.second.world_transform_matrix.Up() *  (capsule.second.world_transform_matrix.Translation() * 0.5f);
		}
	}

	void CapsuleCollisionComponent::RenderImGui()
	{
	}

	void CapsuleCollisionComponent::Load(const std::string& file_path_and_name)
	{
	}

	void CapsuleCollisionComponent::Save(const std::string& file_path)
	{
	}

	std::string CapsuleCollisionComponent::AddCapsule(const std::string& name, const collision::Capsule& capsule)
	{
		if (name == "")
		{// 名前の指定がない場合は「sphere(番号)」という名前にする
			int no = capsules.size();
			std::string new_name{};
			new_name = "CollisionTag tag(" + std::to_string(no) + ")";
			while (true)
			{
				if (capsules.contains(new_name))
				{// 名前の重複があったので番号を+1する
					++no;
				}
				else
				{
					capsules.emplace(new_name, capsule);
					return new_name;
				}
			}
		}
		else
		{
			if (capsules.contains(name))
				assert((!"The sphere name already exists(SphereCollisionComponent::AddSphere)"));

			capsules.emplace(name, capsule);
			return name;
		}
	}

	std::string CapsuleCollisionComponent::AddAndRegisterBoneName(
		const std::string& bone_name,
		const std::string& capsule_name,
		const collision::Capsule& capsule)
	{
		std::string name = AddCapsule(capsule_name, capsule);
		capsules.at(name).bone_name = bone_name;
		return name;
	}

	void CapsuleCollisionComponent::SetOffset(
		const std::string& capsule_name,
		const DirectX::SimpleMath::Vector3& offset)
	{
		if (!capsules.contains(capsule_name))
			assert(!"Name is not registered(CapsuleCollisionComponent::SetOffset)");
		capsules.at(capsule_name).offset = offset;
	}

	void CapsuleCollisionComponent::SetAllOffset(const DirectX::SimpleMath::Vector3& offset)
	{
		for (auto& capsule : capsules)
		{
			capsule.second.offset = offset;
		}
	}

	void CapsuleCollisionComponent::SetRadius(const std::string& capsule_name, float radius)
	{
		if (!capsules.contains(capsule_name))
			assert(!"Name is not registered(CapsuleCollisionComponent::SetOffset)");
		capsules.at(capsule_name).radius = radius;
	}

	void CapsuleCollisionComponent::SetAllRadius(float radius)
	{
		for (auto& capsule : capsules)
		{
			capsule.second.radius = radius;
		}
	}

	const std::unordered_map<std::string, collision::Capsule>& CapsuleCollisionComponent::GetCapsules() const
	{
		return capsules;
	}

} // cumulonimbus::component