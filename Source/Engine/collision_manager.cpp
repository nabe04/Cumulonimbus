#include "collision_manager.h"

#include "arithmetic.h"
#include "ecs.h"
#include "model_data.h"
#include "fbx_model_resource.h"
#include "fbx_model_component.h"
#include "raycast_component.h"
#include "sphere_collision_component.h"
#include "capsule_collison_component.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::collision
{
	void CollisionManager::Update(ecs::Registry* registry)
	{
		//registry->GetEntities().size();
		// ���C�L���X�g
		auto& ray_cast_array = registry->GetArray<component::RayCastComponent>();

		for(auto& ray_comp : ray_cast_array.GetComponents())
		{
			mapping::rename_type::Entity ent = ray_comp.GetEntity();

			// ent_terrains��RayCastComponent�������f���Ń��C�L���X�g�̔�����s��
			for(const auto& ent_trrain : ent_terrains)
			{
				const auto& model = registry->GetComponent<component::FbxModelComponent>(ent_trrain);
				if(IntersectRayVsModel(ray_comp.GetRayStartPos(), ray_comp.GetRayEndPos(), model, ray_comp.GetHitResult()))
				{
					int a;
					a = 0;
				}
			}
		}

		// �����m�̓����蔻��
		auto& sphere_collisions = registry->GetArray<component::SphereCollisionComponent>();
		for(int s1 = 0; s1 < sphere_collisions.GetComponents().size(); ++s1)
		{
			for (int s2 = s1 + 1; s2 < sphere_collisions.GetComponents().size(); ++s2)
			{
				IntersectSphereVsSphere(sphere_collisions.GetComponents().at(s1), sphere_collisions.GetComponents().at(s2));
			}
		}

		// �J�v�Z�����m�̔���
		auto& capsule_collisions = registry->GetArray<component::CapsuleCollisionComponent>();
		for(int c1 = 0;c1 < capsule_collisions.GetComponents().size(); ++c1)
		{
			for(int c2 = c1 + 1; c2 < capsule_collisions.GetComponents().size(); ++c2)
			{
				IntersectCapsuleVsCapsule(capsule_collisions.GetComponents().at(c1), capsule_collisions.GetComponents().at(c2));
			}
		}

		
	}

	void CollisionManager::RegistryRayCastModel(mapping::rename_type::Entity ent)
	{
		ent_terrains.emplace_back(ent);
	}

	bool CollisionManager::IntersectRayVsModel(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const component::FbxModelComponent& model, HitResult& result)
	{
		DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
		DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
		DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
		DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

		// ���[���h��Ԃ̃��C�̒���
		DirectX::XMStoreFloat(&result.distance, WorldRayLength);

		bool hit = false;
		const FbxModelResource* resource = model.GetResource();
		for (const ModelData::Mesh& mesh : resource->GetModelData().GetMeshes())
		{
			const component::FbxModelComponent::Node node = model.GetNodes().at(mesh.node_index);

			// ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
			DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.world_transform);
			DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

			DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
			DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
			DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
			DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

			// ���C�̒���
			float neart;
			DirectX::XMStoreFloat(&neart, Length);

			// �O�p�`(��)�Ƃ̌�������
			const std::vector<ModelData::Vertex>& vertices = mesh.vertices;
			const std::vector<UINT> indices = mesh.indices;

			int materialIndex = -1;
			DirectX::XMVECTOR HitPosition;
			DirectX::XMVECTOR HitNormal;
			for (const ModelData::Subset& subset : mesh.subsets)
			{
				for (UINT i = 0; i < subset.index_count; i += 3)
				{
					UINT index = subset.start_index + i;
					// �O�p�`�̒��_�𒊏o
					const ModelData::Vertex& a = vertices.at(indices.at(index));
					const ModelData::Vertex& b = vertices.at(indices.at(index + 1));
					const ModelData::Vertex& c = vertices.at(indices.at(index + 2));

					DirectX::XMVECTOR A = DirectX::XMLoadFloat4(&a.position);
					DirectX::XMVECTOR B = DirectX::XMLoadFloat4(&b.position);
					DirectX::XMVECTOR C = DirectX::XMLoadFloat4(&c.position);

					// �O�p�`�̎O�Ӄx�N�g�����Z�o
					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

					//�O�p�`�̖@���x�N�g�����Z�o
					DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

					// ���ς̌��ʂ��v���X�Ȃ�Η�����
					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
					float dot;
					DirectX::XMStoreFloat(&dot, Dot);
					if (dot >= 0)continue;

					// ���C�Ƃ̕��ʂ̌�_���Z�o
					DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, Start);
					DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
					float t;
					DirectX::XMStoreFloat(&t, T);
					// ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������傫�����̓X�L�b�v
					if (t<0.0f || t>neart)continue;

					DirectX::XMVECTOR Position = Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

					// ��_���O�p�`�̓����ɂ��邩����
					// �P��
					DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, Position);
					DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
					DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal);
					float dot1;
					DirectX::XMStoreFloat(&dot1, Dot1);
					if (dot1 < 0.0f)continue;

					// �Q��
					DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, Position);
					DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
					DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal);
					float dot2;
					DirectX::XMStoreFloat(&dot2, Dot2);
					if (dot2 < 0.0f)continue;

					// �R��
					DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position);
					DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
					DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal);
					float dot3;
					DirectX::XMStoreFloat(&dot3, Dot3);
					if (dot3 < 0.0f)continue;

					// �ŋߋ������X�V
					neart = t;

					// ��_�Ɩ@�����X�V
					HitPosition = Position;
					HitNormal = Normal;
					materialIndex = subset.material_index;
				}
			}
			if (materialIndex >= 0)
			{
				// ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
				DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
				DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
				DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
				float distance;
				DirectX::XMStoreFloat(&distance, WorldCrossLength);

				// �q�b�g���ۑ�
				if (result.distance > distance)
				{
					DirectX::XMVECTOR WorldNormal = DirectX::XMVector3Transform(HitNormal, WorldTransform);
					result.distance = distance;
					result.materialIndex = materialIndex;
					DirectX::XMStoreFloat3(&result.position, WorldPosition);
					DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
					hit = true;
				}
			}
		}
		return hit;
	}

	bool CollisionManager::IntersectSphereVsSphere(
		const component::SphereCollisionComponent& sphere_1,
		const component::SphereCollisionComponent& sphere_2)
	{
		for(const auto& s1 : sphere_1.GetSpheres())
		{
			for(const auto& s2 : sphere_2.GetSpheres())
			{
				const DirectX::SimpleMath::Vector3 s1_translation = s1.second.world_transform_matrix.Translation();
				const DirectX::SimpleMath::Vector3 s2_translation = s2.second.world_transform_matrix.Translation();

				const DirectX::SimpleMath::Vector3 v = { s1_translation - s2_translation };
				const float l = v.Length();

				if (l <= s1.second.radius + s2.second.radius)
					return true;
			}
		}
		return false;
	}

	bool CollisionManager::IntersectCapsuleVsCapsule(
		component::CapsuleCollisionComponent& capsule_1,
		component::CapsuleCollisionComponent& capsule_2)
	{
		for(auto& c1 : capsule_1.GetCapsules())
		{
			for(auto& c2 : capsule_2.GetCapsules())
			{
				// ���ꂼ��̃J�v�Z���̐���(�n�_)����̑傫��
				float c1_t, c2_t;
				DirectX::SimpleMath::Vector3 c1_p, c2_p;

				// �����̍ŋߋ���
				float len = arithmetic::ClosestPtSegmentSegment(
								c1.second.start, c1.second.end,
								c2.second.start, c2.second.end,
								c1_t, c2_t,
								c1_p, c2_p);

				if(len <c1.second.radius + c2.second.radius)
				{
					c1.second.is_hit = c2.second.is_hit = true;
					return true;
				}

				c1.second.is_hit = c2.second.is_hit = false;
			}
		}
		return false;
	}


} // cumulonimbus::collision
