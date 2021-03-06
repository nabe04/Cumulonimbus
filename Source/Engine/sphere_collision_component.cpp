#include "sphere_collision_component.h"

#include <cassert>

#include "ecs.h"
#include "enum_state_map.h"
#include "cum_imgui_helper.h"
// components
#include "fbx_model_component.h"
#include "model_component.h"
#include "fbx_model_resource.h"
#include "transform_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::SphereCollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::CollisionComponent, cumulonimbus::component::SphereCollisionComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::SphereCollisionComponent, 0)
CEREAL_CLASS_VERSION(cumulonimbus::collision::Sphere, 1)

namespace
{
	// ImGui描画時にCollisionPreset(enum class)から文字列一覧を取得する為に使用
	EnumStateMap<cumulonimbus::collision::CollisionType> collision_preset{};
	// ImGui描画時にCollisionTag(enum class)から文字列一覧を取得する為に使用
	EnumStateMap<cumulonimbus::collision::CollisionTag> collision_tag{};
}

namespace cumulonimbus::collision
{
	template <class Archive>
	void Sphere::load(Archive&& archive, const uint32_t version)
	{
		if(version == 0)
		{
			archive(
				CEREAL_NVP(world_transform_matrix),
				CEREAL_NVP(offset),
				CEREAL_NVP(bone_name),
				CEREAL_NVP(radius),
				//CEREAL_NVP(hit_result),
				CEREAL_NVP(collision_preset),
				CEREAL_NVP(base_color),
				CEREAL_NVP(hit_color),
				CEREAL_NVP(collision_tag)
			);
		}

		if(version == 1)
		{
			archive(
				CEREAL_NVP(world_transform_matrix),
				CEREAL_NVP(offset),
				CEREAL_NVP(bone_name),
				CEREAL_NVP(radius),
				//CEREAL_NVP(hit_result),
				CEREAL_NVP(collision_preset),
				CEREAL_NVP(base_color),
				CEREAL_NVP(hit_color),
				CEREAL_NVP(collision_tag)
			);
		}

		if(version == 2)
		{
			archive(
				CEREAL_NVP(world_transform_matrix),
				CEREAL_NVP(offset),
				CEREAL_NVP(bone_name),
				CEREAL_NVP(radius),
				CEREAL_NVP(collision_preset),
				CEREAL_NVP(base_color),
				CEREAL_NVP(hit_color),
				CEREAL_NVP(collision_tag),
				CEREAL_NVP(is_enable),
				CEREAL_NVP(is_visible),
				CEREAL_NVP(hidden_in_game)
			);
		}
	}

	template <class Archive>
	void Sphere::save(Archive&& archive, const uint32_t version) const
	{
		if (version == 0)
		{
			archive(
				CEREAL_NVP(world_transform_matrix),
				CEREAL_NVP(offset),
				CEREAL_NVP(bone_name),
				CEREAL_NVP(radius),
				//CEREAL_NVP(hit_result),
				CEREAL_NVP(collision_preset),
				CEREAL_NVP(base_color),
				CEREAL_NVP(hit_color),
				CEREAL_NVP(collision_tag)
			);
		}

		if (version == 1)
		{
			archive(
				CEREAL_NVP(world_transform_matrix),
				CEREAL_NVP(offset),
				CEREAL_NVP(bone_name),
				CEREAL_NVP(radius),
				//CEREAL_NVP(hit_result),
				CEREAL_NVP(collision_preset),
				CEREAL_NVP(base_color),
				CEREAL_NVP(hit_color),
				CEREAL_NVP(collision_tag)
			);
		}

		if (version == 2)
		{
			archive(
				CEREAL_NVP(world_transform_matrix),
				CEREAL_NVP(offset),
				CEREAL_NVP(bone_name),
				CEREAL_NVP(radius),
				CEREAL_NVP(collision_preset),
				CEREAL_NVP(base_color),
				CEREAL_NVP(hit_color),
				CEREAL_NVP(collision_tag),
				CEREAL_NVP(is_enable),
				CEREAL_NVP(is_visible),
				CEREAL_NVP(hidden_in_game)
			);
		}
	}

	HitResult* Sphere::TryGetHitResult(const mapping::rename_type::Entity& ent)
	{
		if (hit_results.contains(ent))
			return &hit_results.at(ent);

		return nullptr;
	}

	void Sphere::RegisterHitEntity(const mapping::rename_type::Entity& ent, const HitResult& hit_result)
	{
		// 既に登録されている場合は処理を抜ける
		if (hit_results.contains(ent))
			return;
		hit_results.emplace(ent, hit_result);
	}

	void Sphere::UnRegisterHitEntity(const mapping::rename_type::Entity& ent)
	{
		// エンティティが存在しない場合処理を抜ける
		if (!hit_results.contains(ent))
			return;
		hit_results.erase(ent);
	}
} // cumulonimbus::collision

namespace cumulonimbus::component
{
	template <class Archive>
	void SphereCollisionComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<CollisionComponent>(this),
			CEREAL_NVP(spheres)
		);
	}

	template <class Archive>
	void SphereCollisionComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<CollisionComponent>(this),
			CEREAL_NVP(spheres)
		);
	}

	SphereCollisionComponent::SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, collision::CollisionTag tag)
		:CollisionComponent{ registry,ent ,tag }
	{
	}

	SphereCollisionComponent::SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SphereCollisionComponent& copy_comp)
		: CollisionComponent{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void SphereCollisionComponent::Start()
	{
		for(auto& sphere : spheres | std::views::values)
		{
			sphere.id = utility::GenerateUUID();
		}
	}

	void SphereCollisionComponent::CommonUpdate(float dt)
	{
		auto DefaultTransform = [&](collision::Sphere& sphere, const DirectX::SimpleMath::Matrix& local_mat)
		{
			// モデルが持つワールド変換行列
			DirectX::SimpleMath::Matrix world_transform = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetWorldMatrix();
			world_transform._11 = world_transform._22 = world_transform._33 = world_transform._44 = 1.0f;
			sphere.world_transform_matrix = local_mat * world_transform;
		};

		// 判定用(球)データの更新
		for (auto& sphere : spheres | std::views::values)
		{
			{// ワールド変換行列の作成１
				// Scaling
				const DirectX::SimpleMath::Matrix s = DirectX::XMMatrixScaling(sphere.radius, sphere.radius, sphere.radius);
				// Rotation
				const DirectX::SimpleMath::Matrix r = DirectX::XMMatrixIdentity();
				// Parallel movement
				DirectX::SimpleMath::Matrix t = DirectX::SimpleMath::Matrix::Identity;
				t.Translation(sphere.offset);
				// Local matrix
				const DirectX::SimpleMath::Matrix model_local_matrix = s * r * t;

				if (sphere.bone_name.empty())
				{
					// モデルが持つワールド変換行列
					DefaultTransform(sphere, model_local_matrix);
				}
				else
				{
					if (auto* model_comp = GetRegistry()->TryGetComponent<component::ModelComponent>(GetEntity());
						model_comp)
					{
						if (model_comp->HasNodeFromName(sphere.bone_name.c_str()))
						{
							DirectX::SimpleMath::Matrix world_transform = model_comp->GetNodeMatrix(sphere.bone_name.c_str());
							DirectX::SimpleMath::Vector3 scale{}, translation{};
							DirectX::SimpleMath::Quaternion rotation{};
							if (world_transform.Decompose(scale, rotation, translation))
							{
								DirectX::SimpleMath::Matrix t = DirectX::SimpleMath::Matrix::CreateTranslation(translation);
								DirectX::SimpleMath::Matrix r = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
								world_transform = r * t;
							}
							sphere.world_transform_matrix = model_local_matrix * world_transform;
						}
						else
						{
							DefaultTransform(sphere, model_local_matrix);
						}
					}
					else
					{
						DefaultTransform(sphere, model_local_matrix);
					}
				}
			}
		}
	}


	void SphereCollisionComponent::PreGameUpdate(float dt)
	{

	}

	void SphereCollisionComponent::GameUpdate(float dt)
	{
		for (auto& sphere : spheres | std::views::values)
		{
			// HitEventの更新
			const auto ent_name = GetRegistry()->GetName(GetEntity());

			std::vector<mapping::rename_type::Entity> delete_hit_entities{};
			for (auto& [hit_ent, hit_result] : sphere.hit_results)
			{
				if (hit_result.is_hit)
				{
					if (hit_result.is_old_hit)
					{// 他のコリジョンに触れている間
						// ヒットイベントの更新
						hit_result.hit_event = collision::HitEvent::OnCollisionStay;
						// イベント処理の発行
						on_collision_stay_event.Invoke(GetEntity(), hit_result);
					}
					else
					{// 他のCollisionに触れたとき
						// ヒットイベントの更新
						hit_result.hit_event = collision::HitEvent::OnCollisionEnter;
						// イベント処理の発行
						on_collision_enter_event.Invoke(GetEntity(), hit_result);
					}
				}
				else
				{
					if (hit_result.is_old_hit)
					{// 他のCollisionに触れるのをやめたとき
						// ヒットイベントの更新
						hit_result.hit_event = collision::HitEvent::OnCollisionExit;
						// イベントの発行
						on_collision_exit_event.Invoke(GetEntity(), hit_result);
					}
					else
					{// 他のどのCollisionにも触れていない間
						// ヒットイベントの更新
						hit_result.hit_event = collision::HitEvent::None;
						// イベント処理の発行
						on_collision_none.Invoke(GetEntity(), hit_result);
						delete_hit_entities.emplace_back(hit_ent);
					}
				}
				hit_result.is_old_hit = hit_result.is_hit;
			}

			for (const auto& delete_hit_ent : delete_hit_entities)
			{
				sphere.UnRegisterHitEntity(delete_hit_ent);
			}

			//if (sphere.hit_result.is_hit)
			//{
			//	if (sphere.hit_result.is_old_hit)
			//	{// 他のCollisionに触れている間
			//		// ヒットイベントの更新
			//		sphere.hit_result.hit_event = collision::HitEvent::OnCollisionStay;
			//		// イベント処理の発行
			//		on_collision_stay_event.Invoke(GetEntity(), sphere.hit_result);
			//	}
			//	else
			//	{// 他のCollisionに触れたとき
			//		// ヒットイベントの更新
			//		sphere.hit_result.hit_event = collision::HitEvent::OnCollisionEnter;
			//		// イベント処理の発行
			//		on_collision_enter_event.Invoke(GetEntity(), sphere.hit_result);
			//	}
			//}
			//else
			//{
			//	if (sphere.hit_result.is_old_hit)
			//	{// 他のCollisionに触れるのをやめたとき
			//		// ヒットイベントの更新
			//		sphere.hit_result.hit_event = collision::HitEvent::OnCollisionExit;
			//		// イベント処理の発行
			//		on_collision_exit_event.Invoke(GetEntity(), sphere.hit_result);
			//	}
			//	else
			//	{// 他のどのCollisionにも触れていない間
			//		// ヒットイベントの更新
			//		sphere.hit_result.hit_event = collision::HitEvent::None;
			//		// イベント処理の発行
			//		on_collision_none.Invoke(GetEntity(), sphere.hit_result);
			//	}
			//}

			//sphere.hit_result.is_old_hit = sphere.hit_result.is_hit;
		}

	}

	void SphereCollisionComponent::PostGameUpdate(float dt)
	{

	}

	void SphereCollisionComponent::RenderImGui()
	{
		if(GetRegistry()->CollapsingHeader<SphereCollisionComponent>(GetEntity(),"Sphere Collider"))
		{
			//-- 球の追加&削除処理 --//
			ImGui::Text("Add Sphere");
			ImGui::SameLine();
			if(ImGui::Button(ICON_FA_PLUS))
			{
				AddSphere();
			}
			ImGui::SameLine();
			if(ImGui::Button(ICON_FA_MINUS))
			{
				if (const auto itr = spheres.find(selected_collision_name);
					itr != spheres.end())
				{
					spheres.erase(itr);
					if(spheres.size() > 0)
					{
						selected_collision_name = spheres.begin()->first;
					}
					else
					{
						selected_collision_name = "";
					}
				}
			}

			std::vector<std::string> items{};
			items.reserve(spheres.size());
			for (const auto& sphere_name : spheres | std::views::keys)
			{
				items.emplace_back(sphere_name);
			}

			//-- ImGui::Comboでの選択中のコリジョン変更 --//
			if(helper::imgui::Combo("Sphere Data",selected_collision_name,items))
			{

			}

			//-- 選択されている球の設定 --//
			if (spheres.contains(selected_collision_name))
			{
				collision::Sphere& sphere = spheres.at(selected_collision_name);
				auto CollisionPresetCombo = [&]()
				{
					if(std::string current_name = nameof::nameof_enum(sphere.collision_preset).data();
					   helper::imgui::Combo("Preset",current_name,collision_preset.GetStateNames()))
					{// コリジョンプリセットの変更
						sphere.collision_preset = collision_preset.GetStateMap().at(current_name);
					}
				};

				// カプセルタグ変更関数
				auto CollisionTagCombo = [&]()
				{
					if (std::string current_name = nameof::nameof_enum(sphere.collision_tag).data();
						helper::imgui::Combo("Tag", current_name, collision_tag.GetStateNames()))
					{// コリジョンタグの変更
						sphere.collision_tag = collision_tag.GetStateMap().at(current_name);
					}
				};

				ImGui::PushID("Sphere Collider");
				AttachSocket(sphere.bone_name);
				IMGUI_LEFT_LABEL(ImGui::Checkbox, "Is Visible", &sphere.is_visible);
				IMGUI_LEFT_LABEL(ImGui::Checkbox, "Hidden In Game", &sphere.hidden_in_game);
				CollisionPresetCombo();
				CollisionTagCombo();
				IMGUI_LEFT_LABEL(ImGui::DragFloat3, "Position", &sphere.offset.x, 0.01f, -FLT_MAX, FLT_MAX);
				IMGUI_LEFT_LABEL(ImGui::DragFloat , "Radius"  , &sphere.radius  , 0.1f , 0.1f	 , FLT_MAX);
				IMGUI_LEFT_LABEL(ImGui::ColorEdit4, "Base Color", &sphere.base_color.x);
				IMGUI_LEFT_LABEL(ImGui::ColorEdit4, "Hit Color" , &sphere.hit_color.x);

				//ImGui::Checkbox("Is Visible", &sphere.is_visible);
				//CollisionPresetCombo();
				//ImGui::DragFloat3("Position", &sphere.offset.x	, 0.01f	, FLT_MIN , FLT_MAX);
				//ImGui::DragFloat("Radius"	, &sphere.radius	, 0.1f	, 0.1f	  , FLT_MAX);
				//ImGui::ColorEdit4("Base Color", &sphere.base_color.x);
				//ImGui::ColorEdit4("Hit Color" , &sphere.hit_color.x);
				//// テスト(すぐ消す)
				//ImGui::Separator();
				//ImGui::Text("Hit Ent Name");
				//for (const auto& hit_result : sphere.hit_results)
				//{
				//	ImGui::Text(GetRegistry()->GetName(hit_result.second.entity).c_str());
				//}
				ImGui::PopID();
			}
		}
	}

	void SphereCollisionComponent::Load(ecs::Registry* registry)
	{
		CollisionComponent::Load(registry);
		SetRegistry(registry);
	}

	std::string SphereCollisionComponent::AddSphere(const std::string& name, const collision::Sphere& sphere)
	{
		if(name == "")
		{// 名前の指定がない場合は「sphere(番号)」という名前にする
			size_t no = spheres.size();
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
					spheres.at(new_name).id = utility::GenerateUUID();
					selected_collision_name = new_name;
					return new_name;
				}
			}
		}
		else
		{
			if (spheres.contains(name))
				assert((!"The sphere name already exists(SphereCollisionComponent::AddSphere)"));

			spheres.emplace(name, sphere);
			spheres.at(name).id		= utility::GenerateUUID();
			selected_collision_name = name;
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

	collision::Sphere* SphereCollisionComponent::TryGetSphere(const mapping::rename_type::UUID& sphere_id)
	{
		for(auto& sphere : spheres | std::views::values)
		{
			if (sphere.id == sphere_id)
				return &sphere;
		}

		return nullptr;
	}

	void SphereCollisionComponent::SetAllCollisionEnable(const bool is_enable)
	{
		for(auto& sphere : spheres | std::views::values)
		{
			sphere.is_enable = is_enable;
		}
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


	void SphereCollisionComponent::SetCollisionPreset(const std::string& sphere_name, collision::CollisionType preset)
	{
		if (!spheres.contains(sphere_name))
			assert(!"Name is not registered(SphereCollisionComponent::SetCollisionPreset)");
		spheres.at(sphere_name).collision_preset = preset;
	}

	void SphereCollisionComponent::SetAllCollisionPreset(collision::CollisionType preset)
	{
		for (auto& sphere : spheres)
		{
			sphere.second.collision_preset = preset;
		}
	}

	std::unordered_map<std::string, collision::Sphere>& SphereCollisionComponent::GetSpheres()
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