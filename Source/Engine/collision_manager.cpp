#include "collision_manager.h"

#include "ecs.h"
#include "model_data.h"
#include "fbx_model_resource.h"
#include "fbx_model_component.h"
#include "raycast_component.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::collision
{
	void CollisionManager::Update(ecs::Registry* registry)
	{
		//registry->GetEntities().size();
		// レイキャスト
		auto& ray_cast_array = registry->GetArray<component::RayCastComponent>();

		for(auto& ray_comp : ray_cast_array.GetComponents())
		{
			mapping::rename_type::Entity ent = ray_comp.GetEntity();

			// ent_terrainsとRayCastComponentを持つモデルでレイキャストの判定を行う
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

		// ワールド空間のレイの長さ
		DirectX::XMStoreFloat(&result.distance, WorldRayLength);

		bool hit = false;
		const FbxModelResource* resource = model.GetResource();
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

				// ヒット情報保存
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

} // cumulonimbus::collision
