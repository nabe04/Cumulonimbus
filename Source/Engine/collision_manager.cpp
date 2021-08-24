#include "collision_manager.h"

#include "arithmetic.h"
#include "ecs.h"
#include "fbx_model_resource.h"
#include "model_data.h"
#include "rename_type_mapping.h"
// Components
#include "transform_component.h"
#include "capsule_collison_component.h"
#include "fbx_model_component.h"
#include "physic_material_component.h"
#include "raycast_component.h"
#include "rigid_body_component.h"
#include "sphere_collision_component.h"

namespace cumulonimbus::collision
{
	void CollisionManager::Update(const float dt, ecs::Registry* registry)
	{
		// 球同士の判定
		auto& sphere_collisions = registry->GetArray<component::SphereCollisionComponent>();
		for (int s1 = 0; s1 < sphere_collisions.GetComponents().size(); ++s1)
		{
			for (int s2 = s1 + 1; s2 < sphere_collisions.GetComponents().size(); ++s2)
			{
				IntersectSphereVsSphere(
					dt,
					registry,
					sphere_collisions.GetComponents().at(s1),
					sphere_collisions.GetComponents().at(s2));
			}
		}

		// カプセル同士の判定
		auto& capsule_collisions = registry->GetArray<component::CapsuleCollisionComponent>();
		for (int c1 = 0; c1 < capsule_collisions.GetComponents().size(); ++c1)
		{
			for (int c2 = c1 + 1; c2 < capsule_collisions.GetComponents().size(); ++c2)
			{
				IntersectCapsuleVsCapsule(
					dt,
					registry,
					capsule_collisions.GetComponents().at(c1),
					capsule_collisions.GetComponents().at(c2));
			}
		}

		// 球とカプセルの判定
		for (int s = 0; s < sphere_collisions.GetComponents().size(); ++s)
		{
			for (int c = 0; c < capsule_collisions.GetComponents().size(); ++c)
			{
				IntersectSphereVsCapsule(
					dt,
					registry,
					sphere_collisions.GetComponents().at(s),
					capsule_collisions.GetComponents().at(c));
			}
		}

		// 地形とモデルとの判定(レイキャスト)
		auto& ray_cast_array = registry->GetArray<component::RayCastComponent>();

		for (auto& ray_comp : ray_cast_array.GetComponents())
		{
			mapping::rename_type::Entity ent = ray_comp.GetEntity();

			// ent_terrainsとRayCastComponentを持つモデルでレイキャストの判定を行う
			for (const auto& ent_trrain : ent_terrains)
			{
				const auto& model = registry->GetComponent<component::FbxModelComponent>(ent_trrain);
				if (IntersectRayVsModel(dt, model,ray_comp))
				{
					ray_comp.GetHitResult().is_hit = true;
				}
				else
				{
					ray_comp.GetHitResult().is_hit = false;
				}
			}
		}
	}

	void CollisionManager::RegistryRayCastModel(mapping::rename_type::Entity ent)
	{
		ent_terrains.emplace_back(ent);
	}

	float CollisionManager::CalculateRestitution(
		const component::PhysicMaterialComponent* physic_material_comp_1,
		const component::PhysicMaterialComponent* physic_material_comp_2)
	{
		float restitution_1 = 0;
		float restitution_2 = 0;

		// PhysicMaterialComponentがnullptrの場合は反発係数を0に設定
		if (physic_material_comp_1)
			restitution_1 = physic_material_comp_1->GetBounciness();
		if (physic_material_comp_2)
			restitution_2 = physic_material_comp_2->GetBounciness();

		/*
		 * CombineSettingをもとに反発係数を算出
		 * 接触している 2 つのコライダーに異なる combine モードが設定されているときは特別で最も高い優先度をもつ機能が使用される。
		 * Average < Minimum < Multiply < Maximum
		 * 例)
		 *  あるマテリアルが Average モードを持ち、他が Maximum モードを持つとき、 combine 機能は Maximum で使用される。
		 */

		// どちらもPhysicMaterialComponentが存在する場合
		if(physic_material_comp_1 && physic_material_comp_2)
		{
			const component::PhysicMaterialComponent::CombineSetting combine_setting_1 = physic_material_comp_1->GetBounceCombine();
			const component::PhysicMaterialComponent::CombineSetting combine_setting_2 = physic_material_comp_2->GetBounceCombine();

			if((combine_setting_1 == component::PhysicMaterialComponent::CombineSetting::Maximum) ||
			   (combine_setting_2 == component::PhysicMaterialComponent::CombineSetting::Maximum))
			{
				return restitution_1 > restitution_2 ? restitution_1 : restitution_2;
			}
			if((combine_setting_1 == component::PhysicMaterialComponent::CombineSetting::Multiply) ||
			   (combine_setting_2 == component::PhysicMaterialComponent::CombineSetting::Multiply))
			{
				return restitution_1 * restitution_2;
			}
			if((combine_setting_1 == component::PhysicMaterialComponent::CombineSetting::Minimum) ||
			   (combine_setting_2 == component::PhysicMaterialComponent::CombineSetting::Minimum))
			{
				return restitution_1 < restitution_2 ? restitution_1 : restitution_2;
			}
			if((combine_setting_1 == component::PhysicMaterialComponent::CombineSetting::Average) ||
			   (combine_setting_2 == component::PhysicMaterialComponent::CombineSetting::Average))
			{
				return (restitution_1 + restitution_2) * 0.5f;
			}
		}
		// physic_material_comp_1のみ存在する場合
		if(physic_material_comp_1)
		{
			const component::PhysicMaterialComponent::CombineSetting combine_setting_1 = physic_material_comp_1->GetBounceCombine();

			if(combine_setting_1 == component::PhysicMaterialComponent::CombineSetting::Maximum)
			{
				return restitution_1 > restitution_2 ? restitution_1 : restitution_2;
			}
			if((combine_setting_1 == component::PhysicMaterialComponent::CombineSetting::Multiply))
			{
				return restitution_1 * restitution_2;
			}
			if((combine_setting_1 == component::PhysicMaterialComponent::CombineSetting::Minimum))
			{
				return restitution_1 < restitution_2 ? restitution_1 : restitution_2;
			}
			if((combine_setting_1 == component::PhysicMaterialComponent::CombineSetting::Average))
			{
				return (restitution_1 + restitution_2) * 0.5f;
			}
		}
		// physic_material_comp_2のみ存在する場合
		if(physic_material_comp_2)
		{
			const component::PhysicMaterialComponent::CombineSetting combine_setting_2 = physic_material_comp_2->GetBounceCombine();

			if (combine_setting_2 == component::PhysicMaterialComponent::CombineSetting::Maximum)
			{
				return restitution_1 > restitution_2 ? restitution_1 : restitution_2;
			}
			if ((combine_setting_2 == component::PhysicMaterialComponent::CombineSetting::Multiply))
			{
				return restitution_1 * restitution_2;
			}
			if ((combine_setting_2 == component::PhysicMaterialComponent::CombineSetting::Minimum))
			{
				return restitution_1 < restitution_2 ? restitution_1 : restitution_2;
			}
			if ((combine_setting_2 == component::PhysicMaterialComponent::CombineSetting::Average))
			{
				return (restitution_1 + restitution_2) * 0.5f;
			}
		}

		// どちらのPhysicMaterialComponentもnullptrの場合
		return 0;
	}

	void CollisionManager::Extrude(
		const float dt,
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent_1,
		const mapping::rename_type::Entity ent_2,
		const DirectX::SimpleMath::Vector3& mass_point_1,
		const DirectX::SimpleMath::Vector3& mass_point_2,
		const CollisionPreset collision_preset_1,
		const CollisionPreset collision_preset_2,
		const float penetration)
	{
		// 条件 1 : お互いRigidBodyComponentを持っていること
		// 条件 2 : お互いのCollisionPresetがBlockAllであること
		if (!registry->TryGetComponent<component::RigidBodyComponent>(ent_1)) return;
		if (!registry->TryGetComponent<component::RigidBodyComponent>(ent_2)) return;
		if (collision_preset_1 != CollisionPreset::BlockAll) return;
		if (collision_preset_2 != CollisionPreset::BlockAll) return;

		auto& rigid_body_comp_1 = registry->GetComponent<component::RigidBodyComponent>(ent_1);
		auto& rigid_body_comp_2 = registry->GetComponent<component::RigidBodyComponent>(ent_2);

		DirectX::SimpleMath::Vector3 n{ mass_point_2 - mass_point_1 };
		n.Normalize();

		const float m1 = rigid_body_comp_1.GetMass();
		const float m2 = rigid_body_comp_2.GetMass();

		// 衝突前の速度
		const float pre_v1 = rigid_body_comp_1.GetVelocity().Dot(n);
		const float pre_v2 = rigid_body_comp_2.GetVelocity().Dot(n);
		if(pre_v1 - pre_v2 > 0)
		{
			// 反発係数の取得
			const float restitution = CalculateRestitution(registry->TryGetComponent<component::PhysicMaterialComponent>(ent_1), registry->TryGetComponent<component::PhysicMaterialComponent>(ent_2));

			// 衝突後の速度
			const float post_v1 = (m1 * pre_v1 + m2 * pre_v2 + restitution * m2 * (pre_v2 - pre_v1)) / (m1 + m2);
			const float post_v2 = (m1 * pre_v1 + m2 * pre_v2 + restitution * m1 * (pre_v1 - pre_v2)) / (m1 + m2);

			rigid_body_comp_1.AddVelocity((post_v1 - pre_v1) * n);
			rigid_body_comp_1.Integrate(dt);
			rigid_body_comp_2.AddVelocity((post_v2 - pre_v2) * n);
			rigid_body_comp_2.Integrate(dt);
		}

		registry->GetComponent<component::TransformComponent>(ent_1).AdjustPosition(-(m2 / (m1 + m2) * penetration * n));
		registry->GetComponent<component::TransformComponent>(ent_2).AdjustPosition(m1 / (m1 + m2) * penetration * n);
	}


	bool CollisionManager::IntersectRayVsModel(
		const float dt,
		const component::FbxModelComponent& model,
		component::RayCastComponent& ray_cast_comp)
	{
		bool hit = false;
		const FbxModelResource* resource = model.GetResource();
		for (auto& ray : ray_cast_comp.GetRays())
		{
			DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&ray.second.ray_start);
			DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&ray.second.ray_end);
			DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
			DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
			ray.second.hit_result.is_hit = false;

			// ワールド空間のレイの長さ
			DirectX::XMStoreFloat(&ray.second.hit_result.distance, WorldRayLength);

			for (const ModelData::Mesh& mesh : resource->GetModelData().GetMeshes())
			{
				const component::FbxModelComponent::Node node = model.GetNodes().at(mesh.node_index);

				// レイをワールド空間からローカル空間へ変換
				DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.world_transform);
				DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

				DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
				DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
				DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
				DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
				DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

				// レイの長さ
				float neart;
				DirectX::XMStoreFloat(&neart, Length);

				// 三角形(面)との交差判定
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
						// 三角形の頂点を抽出
						const ModelData::Vertex& a = vertices.at(indices.at(index));
						const ModelData::Vertex& b = vertices.at(indices.at(index + 1));
						const ModelData::Vertex& c = vertices.at(indices.at(index + 2));

						DirectX::XMVECTOR A = DirectX::XMLoadFloat4(&a.position);
						DirectX::XMVECTOR B = DirectX::XMLoadFloat4(&b.position);
						DirectX::XMVECTOR C = DirectX::XMLoadFloat4(&c.position);

						// 三角形の三辺ベクトルを算出
						DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
						DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
						DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

						//三角形の法線ベクトルを算出
						DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

						// 内積の結果がプラスならば裏向き
						DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
						float dot;
						DirectX::XMStoreFloat(&dot, Dot);
						if (dot >= 0)continue;

						// レイとの平面の交点を算出
						DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, Start);
						DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
						float t;
						DirectX::XMStoreFloat(&t, T);
						// 交点までの距離が今までに計算した最近距離より大きい時はスキップ
						if (t<0.0f || t>neart)continue;

						DirectX::XMVECTOR Position = Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

						// 交点が三角形の内側にあるか判定
						// １つめ
						DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, Position);
						DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
						DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal);
						float dot1;
						DirectX::XMStoreFloat(&dot1, Dot1);
						if (dot1 < 0.0f)continue;

						// ２つめ
						DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, Position);
						DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
						DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal);
						float dot2;
						DirectX::XMStoreFloat(&dot2, Dot2);
						if (dot2 < 0.0f)continue;

						// ３つめ
						DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position);
						DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
						DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal);
						float dot3;
						DirectX::XMStoreFloat(&dot3, Dot3);
						if (dot3 < 0.0f)continue;

						// 最近距離を更新
						neart = t;

						// 交点と法線を更新
						HitPosition = Position;
						HitNormal = Normal;
						materialIndex = subset.material_index;
					}
				}
				if (materialIndex >= 0)
				{
					// ローカル空間からワールド空間へ変換
					DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
					DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
					DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
					float distance;
					DirectX::XMStoreFloat(&distance, WorldCrossLength);
					utility::MaterialDiscrimination material_discrimination{};

					// ヒット情報保存
					if (ray.second.hit_result.distance > distance)
					{
						DirectX::XMVECTOR WorldNormal = DirectX::XMVector3Transform(HitNormal, WorldTransform);
						ray.second.hit_result.distance = distance;
						ray.second.hit_result.material_index = materialIndex;
						ray.second.hit_result.is_hit = true;
						DirectX::XMStoreFloat3(&ray.second.hit_result.position, WorldPosition);
						DirectX::XMStoreFloat3(&ray.second.hit_result.normal, DirectX::XMVector3Normalize(WorldNormal));
						ray_cast_comp.SetTerrainAttribute(ray.first,material_discrimination.GetTerrainAttribute(model.GetResource()->GetModelData().GetMaterials().at(materialIndex).texture_name)); // マテリアルのテクスチャ名からマテリアルの属性を取
						hit = true;
						break;
					}
				}
			}
		}
		return hit;
	}

	bool CollisionManager::IntersectSphereVsSphere(
		const float dt,
		ecs::Registry* registry,
		component::SphereCollisionComponent& sphere_1,
		component::SphereCollisionComponent& sphere_2)
	{
		bool hit = false;
		for(auto& s1 : sphere_1.GetSpheres())
		{
			for(auto& s2 : sphere_2.GetSpheres())
			{
				const DirectX::SimpleMath::Vector3 s1_translation = s1.second.world_transform_matrix.Translation();
				const DirectX::SimpleMath::Vector3 s2_translation = s2.second.world_transform_matrix.Translation();

				const DirectX::SimpleMath::Vector3 v = { s1_translation - s2_translation };
				const float len = v.Length();

				if (len <= s1.second.radius + s2.second.radius)
				{
					hit = true;
					s1.second.hit_result.is_hit = true;
					s2.second.hit_result.is_hit = true;
					// 押出し処理
					Extrude(
						dt,
						registry,
						sphere_1.GetEntity(), sphere_2.GetEntity(),
						s1_translation, s2_translation,
						s1.second.collision_preset, s2.second.collision_preset,
						(s1.second.radius + s2.second.radius) - len);
				}
				else
				{
					s1.second.hit_result.is_hit = false;
					s2.second.hit_result.is_hit = false;
				}

			}
		}
		return hit;
	}

	bool CollisionManager::IntersectCapsuleVsCapsule(
		const float dt,
		ecs::Registry* registry,
		component::CapsuleCollisionComponent& capsule_1,
		component::CapsuleCollisionComponent& capsule_2)
	{
		bool hit = false;

		for(auto& c1 : capsule_1.GetCapsules())
		{
			for(auto& c2 : capsule_2.GetCapsules())
			{
				// それぞれのカプセルの線分(始点)からの大きさ
				float c1_t, c2_t;
				DirectX::SimpleMath::Vector3 c1_p, c2_p;

				// 線分の最近距離
				const float len = arithmetic::ClosestPtSegmentSegment(
									c1.second.start, c1.second.end,
									c2.second.start, c2.second.end,
									c1_t, c2_t,
									c1_p, c2_p);

				if(len <c1.second.radius + c2.second.radius)
				{
					c1.second.hit_result.is_hit = true;
					c2.second.hit_result.is_hit = true;
					hit = true;
					// 押出し処理
					Extrude(
						dt,
						registry,
						capsule_1.GetEntity(), capsule_2.GetEntity(),
						c1_p, c2_p,
						c1.second.collision_preset,
						c2.second.collision_preset,
						(c1.second.radius + c2.second.radius) - len);
				}
				else
				{
					c1.second.hit_result.is_hit = c2.second.hit_result.is_hit = false;
				}
			}
		}
		return hit;
	}

	bool CollisionManager::IntersectSphereVsCapsule(
		const float dt,
		ecs::Registry* registry,
		component::SphereCollisionComponent&  sphere,
		component::CapsuleCollisionComponent& capsule)
	{
		bool hit = false;

		// 同じエンティティ同士なら判定処理を行わない
		if (sphere.GetEntity() == capsule.GetEntity())
			return hit;

		for (auto& s : sphere.GetSpheres())
		{
			for (auto& c : capsule.GetCapsules())
			{
				// 球の中心とカプセルの線分の間の(平方した)距離の算出
				const float dist = arithmetic::SqDistPointSegment(c.second.start, c.second.end, s.second.world_transform_matrix.Translation());
				// (平方した)距離が(平方した)半径の総和よりも小さい場合は衝突
				const float radius = s.second.radius + c.second.radius;
				if(dist <= radius * radius)
				{
					c.second.hit_result.is_hit = true;
					s.second.hit_result.is_hit = true;
					hit = true;
					float t = 0; //	最近接点までの距離(線分の割合)
					DirectX::SimpleMath::Vector3 p{}; // カプセルの最近接点
					arithmetic::ClosestPtPointSegment(c.second.start, c.second.end, s.second.world_transform_matrix.Translation(), p, t);
					// 押出し処理
					Extrude(
						dt,
						registry,
						sphere.GetEntity(), capsule.GetEntity(),
						s.second.world_transform_matrix.Translation(), p,
						s.second.collision_preset, c.second.collision_preset,
						(s.second.radius + c.second.radius) - std::sqrtf(dist));
				}
			}
		}

		return hit;
	}



} // cumulonimbus::collision
