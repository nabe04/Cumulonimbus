#include "capsule_collison_component.h"


#include <cassert>

#include "arithmetic.h"
#include "cereal_helper.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "model_component.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::CapsuleCollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::CollisionComponent, cumulonimbus::component::CapsuleCollisionComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::CapsuleCollisionComponent, 1)
CEREAL_CLASS_VERSION(cumulonimbus::collision::Capsule, 1)

namespace
{
	// ImGui描画時にCollisionPreset(enum class)から文字列一覧を取得する為に使用
	EnumStateMap<cumulonimbus::collision::CollisionPreset> collision_preset{};
	// ImGui描画時にCollisionTag(enum class)から文字列一覧を取得する為に使用
	EnumStateMap<cumulonimbus::collision::CollisionTag> collision_tag{};
}

namespace cumulonimbus::collision
{
	//template <class Archive>
	//void Capsule::serialize(Archive&& archive)
	//{
	//	archive(
	//		CEREAL_NVP(world_transform_matrix),
	//		CEREAL_NVP(offset),
	//		CEREAL_NVP(rotation),
	//		CEREAL_NVP(start),
	//		CEREAL_NVP(end),
	//		CEREAL_NVP(bone_name),
	//		CEREAL_NVP(length),
	//		CEREAL_NVP(radius),
	//		CEREAL_NVP(hit_result),
	//		CEREAL_NVP(collision_preset)
	//	);
	//}

	template <class Archive>
	void Capsule::load(Archive&& archive, uint32_t version)
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
			CEREAL_NVP(collision_preset),
			CEREAL_NVP(base_color),
			CEREAL_NVP(hit_color)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(collision_tag)
			);
		}
	}

	template <class Archive>
	void Capsule::save(Archive&& archive, uint32_t version) const
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
			CEREAL_NVP(collision_preset),
			CEREAL_NVP(base_color),
			CEREAL_NVP(hit_color),
			CEREAL_NVP(collision_tag)
		);
	}


} // collision

namespace cumulonimbus::component
{
	template <class Archive>
	void CapsuleCollisionComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<CollisionComponent>(this),
			CEREAL_NVP(capsules)
		);
	}

	template <class Archive>
	void CapsuleCollisionComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<CollisionComponent>(this),
			CEREAL_NVP(capsules)
		);
	}

	CapsuleCollisionComponent::CapsuleCollisionComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const collision::CollisionTag tag)
		:CollisionComponent{ registry,ent,tag }
	{

	}

	CapsuleCollisionComponent::CapsuleCollisionComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const CapsuleCollisionComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void CapsuleCollisionComponent::Start()
	{
		for(auto& capsule : capsules | std::views::values)
		{
			capsule.id = utility::GenerateUUID();
		}
	}

	void CapsuleCollisionComponent::CommonUpdate(float dt)
	{
		auto DefaultTransform = [&](collision::Capsule& capsule, const DirectX::SimpleMath::Matrix& local_mat)
		{
			// モデルが持つワールド変換行列
			DirectX::SimpleMath::Matrix world_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorldMatrix();
			world_transform._11 = world_transform._22 = world_transform._33 = world_transform._44 = 1.0f;
			capsule.world_transform_matrix = local_mat * world_transform;
		};

		const auto& transform_comp = GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		// 判定用(カプセル)データの更新
		for (auto& capsule : capsules | std::views::values)
		{
			// Scaling
			const DirectX::SimpleMath::Matrix s = DirectX::XMMatrixScaling(capsule.radius, capsule.length * 0.5f, capsule.radius);
			//const DirectX::SimpleMath::Matrix s = DirectX::SimpleMath::Matrix::Identity;
			// Rotation
			DirectX::SimpleMath::Vector3 radian = { arithmetic::NormalizeAngle(capsule.rotation.x), arithmetic::NormalizeAngle(capsule.rotation.y),arithmetic::NormalizeAngle(capsule.rotation.z) };
			radian.x = DirectX::XMConvertToRadians(radian.x);
			radian.y = DirectX::XMConvertToRadians(radian.y);
			radian.z = DirectX::XMConvertToRadians(radian.z);
			const DirectX::SimpleMath::Matrix r = DirectX::XMMatrixRotationRollPitchYawFromVector(radian);
			//const DirectX::SimpleMath::Matrix r = DirectX::SimpleMath::Matrix::Identity;
			// Parallel movement
			const DirectX::SimpleMath::Matrix t = DirectX::XMMatrixTranslation(capsule.offset.x, capsule.offset.y, capsule.offset.z);
			// Local matrix
			const DirectX::SimpleMath::Matrix model_local_matrix = s * r * t;

			if (capsule.bone_name.empty())
			{
				// モデルが持つワールド変換行列
				DefaultTransform(capsule, model_local_matrix);
			}
			else
			{
				if (auto* model_comp = GetRegistry()->TryGetComponent<component::ModelComponent>(GetEntity());
					model_comp)
				{
					if (model_comp->HasNodeFromName(capsule.bone_name.c_str()))
					{
						DirectX::SimpleMath::Matrix world_transform = model_comp->GetNodeMatrix(capsule.bone_name.c_str());
						DirectX::SimpleMath::Vector3 scale{}, translation{};
						DirectX::SimpleMath::Quaternion rotation{};
						if (world_transform.Decompose(scale, rotation, translation))
						{
							DirectX::SimpleMath::Matrix t = DirectX::SimpleMath::Matrix::CreateTranslation(translation);
							DirectX::SimpleMath::Matrix r = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
							world_transform = r * t;
						}

						capsule.world_transform_matrix = model_local_matrix * world_transform;
					}
					else
					{
						// モデルが持つワールド変換行列
						DefaultTransform(capsule, model_local_matrix);
					}
				}
				else
				{
					// モデルが持つワールド変換行列
					DefaultTransform(capsule, model_local_matrix);
				}
			}

			DirectX::SimpleMath::Vector3 up = capsule.world_transform_matrix.Up();
			up.Normalize();
			const DirectX::SimpleMath::Vector3 reference_point = capsule.world_transform_matrix.Translation();

			capsule.start   = reference_point + up * -(capsule.length * 0.5f);
			capsule.end		= reference_point + up * (capsule.length * 0.5f);
			//float radius = capsule.radius;
			//capsule.radius  = radius * transform_comp.GetScale().y;

			{// HitEventの更新
				if (capsule.hit_result.is_hit)
				{
					if (capsule.hit_result.is_old_hit)
					{// 他のCollisionに触れている間
						// ヒットイベントの更新
						capsule.hit_result.hit_event = collision::HitEvent::OnCollisionStay;
						// イベント処理の発行
						on_collision_stay_event.Invoke(GetEntity(), capsule.hit_result);
					}
					else
					{// 他のCollisionに触れたとき
						// ヒットイベントの更新
						capsule.hit_result.hit_event = collision::HitEvent::OnCollisionEnter;
						// イベント処理の発行
						on_collision_enter_event.Invoke(GetEntity(), capsule.hit_result);
					}
				}
				else
				{
					if (capsule.hit_result.is_old_hit)
					{// 他のCollisionに触れるのをやめたとき
						// ヒットイベントの更新
						capsule.hit_result.hit_event = collision::HitEvent::OnCollisionExit;
						// イベント処理の発行
						on_collision_exit_event.Invoke(GetEntity(), capsule.hit_result);
					}
					else
					{// 他のどのCollisionにも触れていない間
						// ヒットイベントの更新
						capsule.hit_result.hit_event = collision::HitEvent::None;
						// イベント処理の発行
						on_collision_none.Invoke(GetEntity(), capsule.hit_result);
					}
				}

				capsule.hit_result.is_old_hit = capsule.hit_result.is_hit;
			}
		}
	}

	void CapsuleCollisionComponent::PreGameUpdate(float dt)
	{
	}

	void CapsuleCollisionComponent::GameUpdate(float dt)
	{
	}


	void CapsuleCollisionComponent::PostGameUpdate(float dt)
	{
		//auto DefaultTransform = [&](collision::Capsule& capsule, const DirectX::SimpleMath::Matrix& local_mat)
		//{
		//	// モデルが持つワールド変換行列
		//	DirectX::SimpleMath::Matrix world_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorldMatrix();
		//	world_transform._11 = world_transform._22 = world_transform._33 = world_transform._44 = 1.0f;
		//	capsule.world_transform_matrix = local_mat * world_transform;
		//};

		//// 判定用(カプセル)データの更新
		//for(auto& capsule : capsules | std::views::values)
		//{
		//	// Scaling
		//	const DirectX::SimpleMath::Matrix s = DirectX::XMMatrixScaling(capsule.radius, capsule.length * 0.5f, capsule.radius);
		//	//const DirectX::SimpleMath::Matrix s = DirectX::SimpleMath::Matrix::Identity;
		//	// Rotation
		//	DirectX::SimpleMath::Vector3 radian = { arithmetic::NormalizeAngle(capsule.rotation.x), arithmetic::NormalizeAngle(capsule.rotation.y),arithmetic::NormalizeAngle(capsule.rotation.z) };
		//	radian.x = DirectX::XMConvertToRadians(radian.x);
		//	radian.y = DirectX::XMConvertToRadians(radian.y);
		//	radian.z = DirectX::XMConvertToRadians(radian.z);
		//	const DirectX::SimpleMath::Matrix r = DirectX::XMMatrixRotationRollPitchYawFromVector(radian);
		//	//const DirectX::SimpleMath::Matrix r = DirectX::SimpleMath::Matrix::Identity;
		//	// Parallel movement
		//	const DirectX::SimpleMath::Matrix t = DirectX::XMMatrixTranslation(capsule.offset.x, capsule.offset.y, capsule.offset.z);
		//	// Local matrix
		//	const DirectX::SimpleMath::Matrix model_local_matrix = s * r * t;

		//	if(capsule.bone_name.empty())
		//	{
		//		// モデルが持つワールド変換行列
		//		DefaultTransform(capsule, model_local_matrix);
		//	}
		//	else
		//	{
		//		if (auto* model_comp = GetRegistry()->TryGetComponent<component::ModelComponent>(GetEntity());
		//			model_comp)
		//		{
		//			if(model_comp->HasNodeFromName(capsule.bone_name.c_str()))
		//			{
		//				DirectX::SimpleMath::Matrix world_transform = model_comp->GetNodeMatrix(capsule.bone_name.c_str());
		//				capsule.world_transform_matrix = model_local_matrix * world_transform;
		//			}
		//			else
		//			{
		//				// モデルが持つワールド変換行列
		//				DefaultTransform(capsule, model_local_matrix);
		//			}
		//		}
		//		else
		//		{
		//			// モデルが持つワールド変換行列
		//			DefaultTransform(capsule, model_local_matrix);
		//		}
		//	}

		//	//if (capsule.bone_name.empty())
		//	//{
		//	//	// モデルが持つ回転 × 平行移動行列から行列を作成(モデルの拡縮は考慮しない)
		//	//	DirectX::SimpleMath::Matrix st_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetRotationMatrix() * GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetTranslationMatrix();
		//	//	//world_transform._11 = world_transform._22   = world_transform._33 = world_transform._44 = 1.0f;
		//	//	capsule.world_transform_matrix = model_local_matrix * st_transform;
		//	//}
		//	//else
		//	//{
		//	//	capsule.world_transform_matrix = model_local_matrix * GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).GetNodeMatrix(capsule.bone_name.c_str());
		//	//}

		//	DirectX::SimpleMath::Vector3 up = capsule.world_transform_matrix.Up();
		//	up.Normalize();
		//	const DirectX::SimpleMath::Vector3 reference_point = capsule.world_transform_matrix.Translation();
		//	capsule.start = reference_point + up * -(capsule.length * 0.5f);
		//	capsule.end   = reference_point + up *  (capsule.length * 0.5f);
		//}
	}

	void CapsuleCollisionComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<CapsuleCollisionComponent>(GetEntity(), "Capsule Collider"))
		{
			ImGui::Text("Add Capsule");
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PLUS))
			{
				AddCapsule();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_MINUS))
			{
				if (const auto itr = capsules.find(selected_collision_name);
					itr != capsules.end())
				{
					capsules.erase(itr);
					if (capsules.size() > 0)
					{
						selected_collision_name = capsules.begin()->first;
					}
					else
					{
						selected_collision_name = "";
					}
				}
			}

			std::vector<std::string> items{};
			items.reserve(capsules.size());
			for (const auto& capsule_name : capsules | std::views::keys)
			{
				items.emplace_back(capsule_name);
			}

			//-- ImGui::Comboでの選択中のコリジョン変更 --//
			if (helper::imgui::Combo("Capsule Data", selected_collision_name, items))
			{

			}

			//-- 選択されているカプセルの設定 --//
			if(capsules.contains(selected_collision_name))
			{
				collision::Capsule& capsule = capsules.at(selected_collision_name);
				// カプセルプリセット変更関数
				auto CollisionPresetCombo = [&]()
				{
					if (std::string current_name = nameof::nameof_enum(capsule.collision_preset).data();
						helper::imgui::Combo("Preset", current_name, collision_preset.GetStateNames()))
					{// コリジョンプリセットの変更
						capsule.collision_preset = collision_preset.GetStateMap().at(current_name);
					}
				};
				// カプセルタグ変更関数
				auto CollisionTagCombo = [&]()
				{
					if (std::string current_name = nameof::nameof_enum(capsule.collision_tag).data();
						helper::imgui::Combo("Tag", current_name, collision_tag.GetStateNames()))
					{// コリジョンタグの変更
						capsule.collision_tag = collision_tag.GetStateMap().at(current_name);
					}
				};

				ImGui::PushID("Capsule Collider");
				AttachSocket(capsule.bone_name);
				//ImGui::Text("Is Hit : %d", capsule.hit_result.is_hit);
				//ImGui::Text("Fetch Bone Name : %s", capsule.bone_name.c_str());
				//ImGui::Text("Segment Start.x : %f", capsule.start.x);
				//ImGui::Text("Segment Start.y : %f", capsule.start.y);
				//ImGui::Text("Segment Start.z : %f", capsule.start.z);
				//ImGui::Text("Segment End.x   : %f", capsule.end.x);
				//ImGui::Text("Segment End.y   : %f", capsule.end.y);
				//ImGui::Text("Segment End.z   : %f", capsule.end.z);
				ImGui::Checkbox("Is Visible", &capsule.is_visible);
				CollisionPresetCombo();
				CollisionTagCombo();
				ImGui::DragFloat3("Offset"	, &capsule.offset.x	 , 0.1f, -1000 , 1000);
				ImGui::DragFloat3("Rotation", &capsule.rotation.x, 0.5f, -180.f, 180.f);
				ImGui::DragFloat("Length"	, &capsule.length	 , 0.1f, 1.0f  , 1000.0f);
				ImGui::DragFloat("Radius"	, &capsule.radius	 , 0.1f, 1.0f  , 1000.0f);
				ImGui::ColorEdit4("Base Color", &capsule.base_color.x);
				ImGui::ColorEdit4("Hit Color" , &capsule.hit_color.x);
				ImGui::PopID();
			}
		}
	}

	void CapsuleCollisionComponent::Load(ecs::Registry* registry)
	{
		CollisionComponent::Load(registry);
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
					capsules.at(new_name).id = utility::GenerateUUID();
					return new_name;
				}
			}
		}
		else
		{
			if (capsules.contains(name))
				assert((!"The sphere name already exists(SphereCollisionComponent::AddSphere)"));

			capsules.emplace(name, capsule);
			capsules.at(name).id = utility::GenerateUUID();
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

	collision::Capsule* CapsuleCollisionComponent::TryGetCapsule(const mapping::rename_type::UUID& capsule_id)
	{
		for(auto& capsule : capsules | std::views::values)
		{
			if (capsule.id == capsule_id)
				return &capsule;
		}

		return nullptr;
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