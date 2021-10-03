#include "capsule_collison_component.h"


#include <cassert>

#include "arithmetic.h"
#include "cereal_helper.h"
#include "ecs.h"
#include "fbx_model_component.h"
#include "fbx_model_resource.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::CapsuleCollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::CollisionComponent, cumulonimbus::component::CapsuleCollisionComponent)

namespace cumulonimbus::collision
{
	template <class Archive>
	void Capsule::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(world_transform_matrix),
			CEREAL_NVP(offset),
			CEREAL_NVP(rotation),
			CEREAL_NVP(start),
			CEREAL_NVP(end),
			CEREAL_NVP(bone_name),
			CEREAL_NVP(length),
			CEREAL_NVP(radius),
			CEREAL_NVP(hit_result),
			CEREAL_NVP(collision_preset)
		);
	}

} // collision

namespace cumulonimbus::component
{
	template <class Archive>
	void CapsuleCollisionComponent::serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<CollisionComponent>(this),
			CEREAL_NVP(capsules)
		);
	}


	CapsuleCollisionComponent::CapsuleCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag)
		:CollisionComponent{ registry,ent,tag }
	{
	}

	void CapsuleCollisionComponent::PreGameUpdate(float dt)
	{
	}

	void CapsuleCollisionComponent::GameUpdate(float dt)
	{
	}

	void CapsuleCollisionComponent::PostGameUpdate(float dt)
	{
		// 判定用(カプセル)データの更新
		for(auto& capsule : capsules)
		{
			// Scaling
			const DirectX::SimpleMath::Matrix s = DirectX::XMMatrixScaling(capsule.second.radius, capsule.second.length * 0.5f, capsule.second.radius);
			//const DirectX::SimpleMath::Matrix s = DirectX::SimpleMath::Matrix::Identity;
			// Rotation
			DirectX::SimpleMath::Vector3 radian = { arithmetic::NormalizeAngle(capsule.second.rotation.x), arithmetic::NormalizeAngle(capsule.second.rotation.y),arithmetic::NormalizeAngle(capsule.second.rotation.z) };
			radian.x = DirectX::XMConvertToRadians(radian.x);
			radian.y = DirectX::XMConvertToRadians(radian.y);
			radian.z = DirectX::XMConvertToRadians(radian.z);
			const DirectX::SimpleMath::Matrix r = DirectX::XMMatrixRotationRollPitchYawFromVector(radian);
			//const DirectX::SimpleMath::Matrix r = DirectX::SimpleMath::Matrix::Identity;
			// Parallel movement
			const DirectX::SimpleMath::Matrix t = DirectX::XMMatrixTranslation(capsule.second.offset.x, capsule.second.offset.y, capsule.second.offset.z);
			// Local matrix
			const DirectX::SimpleMath::Matrix model_local_matrix = s * r * t;
			if (capsule.second.bone_name.empty())
			{
				// モデルが持つ回転 × 平行移動行列から行列を作成(モデルの拡縮は考慮しない)
				DirectX::SimpleMath::Matrix st_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetRotationMat() * GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetTranslationMat();
				//world_transform._11 = world_transform._22   = world_transform._33 = world_transform._44 = 1.0f;
				capsule.second.world_transform_matrix	    = model_local_matrix * st_transform;
			}
			else
			{
				capsule.second.world_transform_matrix = model_local_matrix * GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).GetNodeMatrix(capsule.second.bone_name.c_str());
			}

			DirectX::SimpleMath::Vector3 up = capsule.second.world_transform_matrix.Up();
			up.Normalize();
			const DirectX::SimpleMath::Vector3 reference_point = capsule.second.world_transform_matrix.Translation();
			capsule.second.start = reference_point + up * -(capsule.second.length * 0.5f);
			capsule.second.end   = reference_point + up *  (capsule.second.length * 0.5f);
		}
	}

	void CapsuleCollisionComponent::RenderImGui()
	{
		if(ImGui::TreeNode("CapsuleCollisionComponent"))
		{
			int id = 0;
			for(auto& capsule : capsules)
			{
				ImGui::PushID(id);
				if (ImGui::TreeNode(capsule.first.c_str()))
				{
					ImGui::Text("Is Hit : %d", capsule.second.hit_result.is_hit);
					ImGui::Text("Fetch Bone Name : %s", capsule.second.bone_name.c_str());
					ImGui::Text("Segment Start.x : %f", capsule.second.start.x);
					ImGui::Text("Segment Start.y : %f", capsule.second.start.y);
					ImGui::Text("Segment Start.z : %f", capsule.second.start.z);
					ImGui::Text("Segment End.x   : %f", capsule.second.end.x);
					ImGui::Text("Segment End.y   : %f", capsule.second.end.y);
					ImGui::Text("Segment End.z   : %f", capsule.second.end.z);
					ImGui::DragFloat3("Offset", (float*)&capsule.second.offset, 0.1f, -1000, 1000);
					ImGui::DragFloat3("Rotation", (float*)&capsule.second.rotation, 0.5f, -180.f, 180.f);
					ImGui::DragFloat("Length", &capsule.second.length, 0.1f, 1.0f, 1000.0f);
					ImGui::DragFloat("Radius", &capsule.second.radius, 0.1f, 1.0f, 1000.0f);
					++id;
					ImGui::TreePop();
				}

				ImGui::PopID();
			}

			ImGui::TreePop();
		}
	}

	void CapsuleCollisionComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}


	std::string CapsuleCollisionComponent::AddCapsule(const std::string& name, const collision::Capsule& capsule)
	{
		if (name == "")
		{// 名前の指定がない場合は「sphere(番号)」という名前にする
			size_t no = capsules.size();
			std::string new_name{};
			new_name = "capsule(" + std::to_string(no) + ")";
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

	void CapsuleCollisionComponent::SetLength(const std::string& capsule_name, float length)
	{
		if (!capsules.contains(capsule_name))
			assert(!"Name is not registered(CapsuleCollisionComponent::SetOffset)");
		capsules.at(capsule_name).length = length;
	}

	void CapsuleCollisionComponent::SetAllLength(float length)
	{
		for (auto& capsule : capsules)
		{
			capsule.second.length = length;
		}
	}

	void CapsuleCollisionComponent::SetCollisionPreset(const std::string& capsule_name, collision::CollisionPreset preset)
	{
		if (!capsules.contains(capsule_name))
			assert(!"Name is not registered(CapsuleCollisionComponent::SetCollisionPreset)");
		capsules.at(capsule_name).collision_preset = preset;
	}

	void CapsuleCollisionComponent::SetAllCollisionPreset(collision::CollisionPreset preset)
	{
		for(auto& capsule : capsules)
		{
			capsule.second.collision_preset = preset;
		}
	}


	std::unordered_map<std::string, collision::Capsule>& CapsuleCollisionComponent::GetCapsules()
	{
		return capsules;
	}

} // cumulonimbus::component