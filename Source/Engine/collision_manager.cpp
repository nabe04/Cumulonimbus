#include "collision_manager.h"

#include "arithmetic.h"
#include "asset_manager.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "locator.h"
#include "fbx_model_resource.h"
#include "model_data.h"
#include "model_loader.h"
#include "rename_type_mapping.h"
#include "system.h"
#include "scene_manager.h"
#include "scene_loader.h"
// Components
#include "transform_component.h"
#include "capsule_collison_component.h"
#include "model_component.h"
#include "model_component.h"
#include "physic_material_component.h"
#include "raycast_component.h"
#include "rigid_body_component.h"
#include "sphere_collision_component.h"

namespace cumulonimbus::collision
{
	CollisionPrimitiveAsset::CollisionPrimitiveAsset(system::System& system)
	{
		// System::Render関数内で使用する関数の登録
		system.RegisterRenderFunction(utility::GetHash<CollisionPrimitiveAsset>(),
									  [&](ecs::Registry* registry) {RenderImGui(registry); });
	}

	void CollisionPrimitiveAsset::RenderImGui(ecs::Registry* registry)
	{
		if (const ImGuiTreeNodeFlags tree_flg{ ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth };
			ImGui::CollapsingHeader("Collision Asset", tree_flg))
		{
			auto& asset_manager = *locator::Locator::GetAssetManager();
			auto& asset_sheet_manager = asset_manager.GetAssetSheetManager();

			const size_t model_asset_size = asset_manager.GetAssetSheetManager().GetSheet<asset::Model>().sheet.size();

			// 全てのモデルのファイルパス(拡張子を含む)
			std::vector<std::filesystem::path> items_path{};
			items_path.reserve(model_asset_size);
			// 全てのモデルのモデル名(拡張子を含まない)
			std::vector<std::string> items_name{};
			items_name.reserve(model_asset_size);
			for (const auto& asset_path : asset_manager.GetAssetSheetManager().GetSheet<asset::Model>().sheet | std::views::values)
			{
				// アセットファイルパスの追加
				items_path.emplace_back(asset_path);
				// アセットファイル名(拡張子を含まない)の追加
				items_name.emplace_back(std::filesystem::path{ asset_path }.filename().replace_extension().string());
			}


			{// 当たり判定用球のアセット選択
				std::string current_sphere_name{};
				std::string current_sphere_path{};
				if (asset_manager.GetLoader<asset::ModelLoader>()->HasModel(sphere_id))
				{
					current_sphere_name = asset_sheet_manager.GetAssetName<asset::Model>(sphere_id);
					current_sphere_path = asset_sheet_manager.GetAssetFilename<asset::Model>(sphere_id);
				}


				if (helper::imgui::Combo("Sphere Model", current_sphere_name, items_name))
				{// ImGui::Combo内でアイテム選択時
					// 登録されているモデルアセットデータから選択されたアセットを探す
					for (const auto& [asset_id, asset_path] : asset_sheet_manager.GetSheet<asset::Model>().sheet)
					{
						// 選択されているアセット名と登録されている名が違う場合処理を飛ばす
						if (current_sphere_name != asset_sheet_manager.GetAssetName<asset::Model>(asset_id))
							continue;
						// 別のアセットが選択された場合にsphere_idに代入する
						if (asset_id != sphere_id)
						{
							sphere_id = asset_id;
						}
					}
				}
			}

			{// 当たり判定用カプセルのアセット選択
				std::string current_capsule_name{};
				std::string current_capsule_path{};
				if (asset_manager.GetLoader<asset::ModelLoader>()->HasModel(capsule_id))
				{
					current_capsule_name = asset_sheet_manager.GetAssetName<asset::Model>(capsule_id);
					current_capsule_path = asset_sheet_manager.GetAssetFilename<asset::Model>(capsule_id);
				}
				if (helper::imgui::Combo("Capsule Model", current_capsule_name, items_name))
				{// ImGui::Combo内でアイテム選択時
					// 登録されているモデルアセットデータから選択されたアセットを探す
					for (const auto& [asset_id, asset_path] : asset_sheet_manager.GetSheet<asset::Model>().sheet)
					{
						// 選択されているアセット名と登録されている名が違う場合処理を飛ばす
						if (current_capsule_name != asset_sheet_manager.GetAssetName<asset::Model>(asset_id))
							continue;
						// 別のアセットが選択された場合にcapsule_idに代入する
						if (asset_id != capsule_id)
						{
							capsule_id = asset_id;
						}
					}
				}
			}
		}
	}

	void CollisionManager::HitData::RegisterHitEntity(const mapping::rename_type::Entity& ent)
	{
		// 既に登録されている場合は処理を抜ける
		if (hit_entities.contains(ent))
			return;
		// ヒットエンティティの追加
		hit_entities.emplace(ent);
	}

	void CollisionManager::HitData::UnRegisterHitEntity(const mapping::rename_type::Entity& ent)
	{
		// 登録されていない場合処理を抜ける
		if (!hit_entities.contains(ent))
			return;
		// ヒットエンティティの削除
		hit_entities.erase(ent);
	}

	CollisionManager::CollisionManager(system::System& system)
	{
		// System::Render関数内で使用する関数の登録
		system.RegisterRenderFunction(utility::GetHash<CollisionManager>(),
									  [&](ecs::Registry* registry) {RenderImGui(registry); });
	}

	void CollisionManager::Update(const float dt, ecs::Registry* registry)
	{
		//-- 球同士の判定 --//
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

		//-- カプセル同士の判定 --//
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

		//-- 球とカプセルの判定 --//
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

		//-- 地形とモデルとの判定(レイキャスト) --//
		for (auto& ray_cast_array = registry->GetArray<component::RayCastComponent>();
			 auto& ray_comp : ray_cast_array.GetComponents())
		{
			mapping::rename_type::Entity ent = ray_comp.GetEntity();

			// ent_terrainsとRayCastComponentを持つモデルでレイキャストの判定を行う
			for (const auto& ent_terrain : ent_terrains)
			{
				const auto& model = registry->GetComponent<component::ModelComponent>(ent_terrain);
				IntersectRayVsModel(dt, registry, model, ray_comp);
			}
		}
	}

	void CollisionManager::Update(const float dt, scene::SceneManager& scene_manager)
	{
		hit_ent_sphere.clear();
		hit_ent_capsule.clear();

		//-- 同一シーン内での判定処理 --//
		for(const auto& scene : scene_manager.GetActiveScenes() | std::views::values)
		{
			auto* registry = scene->GetRegistry();

			//-- 球同士の判定 --//
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

			//-- カプセル同士の判定 --//
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

			//-- 球とカプセルの判定 --//
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

			//-- 地形とモデルとの判定(レイキャスト) --//
			for (auto& ray_cast_array = registry->GetArray<component::RayCastComponent>();
				auto & ray_comp : ray_cast_array.GetComponents())
			{
				mapping::rename_type::Entity ent = ray_comp.GetEntity();

				// ent_terrainsとRayCastComponentを持つモデルでレイキャストの判定を行う
				for (const auto& ent_terrain : ent_terrains)
				{
					const auto& model = registry->GetComponent<component::ModelComponent>(ent_terrain);
					IntersectRayVsModel(dt, registry, model, ray_comp);
				}
			}
		}

		//-- 別シーンでの判定処理 --//
		std::vector<mapping::rename_type::UUID> scenes_id{};
		scenes_id.reserve(scene_manager.GetActiveScenes().size());
		for (const auto & scene_id : scene_manager.GetActiveScenes() | std::views::keys)
		{
			scenes_id.emplace_back(scene_id);
		}
		if (scenes_id.size() >= 1)
		{
			for (int scene_1 = 0; scene_1 < scenes_id.size(); ++scene_1)
			{
				for (int scene_2 = scene_1 + 1; scene_2 < scenes_id.size(); ++scene_2)
				{
					// シーン1のレジストリ(1、２は識別番号)
					auto* registry_1 = scene_manager.GetActiveScenes().at(scenes_id.at(scene_1))->GetRegistry();
					// シーン2のレジストリ(1、２は識別番号)
					auto* registry_2 = scene_manager.GetActiveScenes().at(scenes_id.at(scene_2))->GetRegistry();

					//-- 球同士の判定 --//
					auto& sphere_collisions_1 = registry_1->GetArray<component::SphereCollisionComponent>();
					auto& sphere_collisions_2 = registry_2->GetArray<component::SphereCollisionComponent>();
					const auto s1_size = sphere_collisions_1.Size();
					const auto s2_size = sphere_collisions_2.Size();
					if (s1_size >= 1 &&
						s2_size >= 1)
					{// それぞれのシーンに球コライダーが1つ以上存在する場合に判定処理を行う
						for (int s1 = 0; s1 < s1_size; ++s1)
						{
							for (int s2 = 0; s2 < s2_size; ++s2)
							{
								IntersectSphereVsSphere(
									dt,
									registry_1, registry_2,
									sphere_collisions_1.GetComponents().at(s1),
									sphere_collisions_2.GetComponents().at(s2));
							}
						}
					}


					//-- カプセル同士の判定 --//
					auto& capsule_collisions_1 = registry_1->GetArray<component::CapsuleCollisionComponent>();
					auto& capsule_collisions_2 = registry_2->GetArray<component::CapsuleCollisionComponent>();
					const auto c1_size = capsule_collisions_1.Size();
					const auto c2_size = capsule_collisions_2.Size();
					if (c1_size >= 1 &&
						c2_size >= 1)
					{// それぞれのシーンにカプセルコライダーが1つ以上存在する場合に判定処理を行う
						for (int c1 = 0; c1 < c1_size; ++c1)
						{
							for (int c2 = 0; c2 < c2_size; ++c2)
							{
								IntersectCapsuleVsCapsule(
									dt,
									registry_1, registry_2,
									capsule_collisions_1.GetComponents().at(c1),
									capsule_collisions_2.GetComponents().at(c2));
							}
						}
					}

					//-- 球とカプセル同士の判定 --//
					// シーン1(球)、シーン2(カプセル)の場合
					for (int s1 = 0; s1 < s1_size; ++s1)
					{
						for (int c2 = 0; c2 < c2_size; ++c2)
						{
							IntersectSphereVsCapsule(
								dt,
								registry_1, registry_2,
								sphere_collisions_1.GetComponents().at(s1),
								capsule_collisions_2.GetComponents().at(c2));
						}
					}
					// シーン1(カプセル)、シーン2(球)の場合
					for (int c1 = 0; c1 < c1_size; ++c1)
					{
						for (int s2 = 0; s2 < s2_size; ++s2)
						{
							IntersectSphereVsCapsule(
								dt,
								registry_2, registry_1,
								sphere_collisions_2.GetComponents().at(s2),
								capsule_collisions_1.GetComponents().at(c1));
						}
					}
				}
			}
		}

		//-- 判定されたコリジョンの判定をtrueに --//
		for (const auto& scene : scene_manager.GetActiveScenes() | std::views::values)
		{
			auto* registry = scene->GetRegistry();

			for (const auto& [sphere_id, hit_data] : hit_ent_sphere)
			{// 判定された球情報が一致した場合判定(is_hit)をtrueに
				if (!registry->HasComponent<component::SphereCollisionComponent>(hit_data.self_entity))
					continue;
				auto* sphere = registry->GetComponent<component::SphereCollisionComponent>(hit_data.self_entity).TryGetSphere(sphere_id);
				if (!sphere)
					continue;

				// 球のエンティティ別の判定をTrueに
				for (auto& [hit_ent, hit_result] : sphere->hit_results)
				{
					// 判定された球が球コライダーのリスト内に存在する場合のみ判定をTrueにする
					if (hit_data.hit_entities.contains(hit_ent))
						hit_result.is_hit = true;
				}
			}

			for (const auto& [capsule_id, hit_data] : hit_ent_capsule)
			{// 判定されたカプセル情報が一致した場合判定(is_hit)をtrueに
				if (!registry->HasComponent<component::CapsuleCollisionComponent>(hit_data.self_entity))
					continue;
				auto* capsule = registry->GetComponent<component::CapsuleCollisionComponent>(hit_data.self_entity).TryGetCapsule(capsule_id);
				if (!capsule)
					continue;
				// カプセルのエンティティ別の判定をTrueに
				for(auto& hit_result : capsule->hit_results | std::views::values)
				{
					if(hit_result.entity == hit_data.hit_entity)
						hit_result.is_hit = true;
				}
			}
		}
	}

	void CollisionManager::RenderImGui(ecs::Registry* registry)
	{
		if(ImGui::CollapsingHeader(ICON_FA_CUBES"Collision Manager", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if(ImGui::TreeNodeEx(ICON_FA_CHECK"Ray Casts",ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Add Terrain");
				ImGui::SameLine();
				if(ImGui::Button(ICON_FA_PLUS))
				{//	レイキャスト対象を格納するコンテナの追加
					ent_terrains.emplace_back();
				}
				ImGui::SameLine();
				if(ImGui::Button(ICON_FA_MINUS))
				{// レイキャスト対象を格納するコンテナの削除(一番後ろのコンテナ)
					ent_terrains.pop_back();
				}
				for(int no = 0; auto& terrain : ent_terrains)
				{// レイキャスト対象のエンティティを選択
					std::string combo_name = "Terrain(" + std::to_string(no++) + ")";

					if (ImGui::BeginCombo(combo_name.c_str(), registry->GetName(terrain).c_str()))
					{
						for (const auto& [key, value] : registry->GetEntities())
						{
							const bool is_selected = (registry->GetName(terrain) == value.second);
							if (ImGui::Selectable(value.second.c_str(), is_selected))
							{
								// レイキャスト対象が重複しない場合のみ登録
								if(!HasTerrain(key))
									terrain = key;
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}
				ImGui::TreePop();
			}
		}
	}

	void CollisionManager::RegistryRayCastModel(const mapping::rename_type::Entity ent)
	{
		for(const auto& terrain : ent_terrains)
		{// 同じ地形を登録しようとした場合処理を抜ける
			if (terrain == ent)
				return;
		}

		ent_terrains.emplace_back(ent);
	}

	bool CollisionManager::HasTerrain(const mapping::rename_type::Entity ent)
	{
		for (const auto& terrain : ent_terrains)
		{// 同じ地形を登録しようとした場合処理を抜ける
			if (terrain == ent)
				return true;
		}

		return false;
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

	void CollisionManager::AddHitSphere(
		const mapping::rename_type::UUID& sphere_id,
		const mapping::rename_type::Entity& self_ent,
		const mapping::rename_type::Entity& hit_ent)
	{
		if (hit_ent_sphere.contains(sphere_id))
		{
			hit_ent_sphere.at(sphere_id).RegisterHitEntity(hit_ent);
			return;
		}

		HitData hit_data{};
		hit_data.self_entity = self_ent;
		hit_data.RegisterHitEntity(hit_ent);
		hit_ent_sphere.emplace(sphere_id, hit_data);
	}

	void CollisionManager::AddHitCapsule(
		const mapping::rename_type::UUID& capsule_id,
		const HitData& hit_data)
	{
		if (hit_ent_capsule.contains(capsule_id))
		{
			hit_ent_capsule.at(capsule_id) = hit_data;
			return;
		}

		hit_ent_capsule.emplace(capsule_id, hit_data);
	}

	void CollisionManager::UpdateHitResult(
		ecs::Registry* registry_1, ecs::Registry* registry_2,
		Sphere&		   sphere_1  , Sphere&		  sphere_2,
		const mapping::rename_type::UUID& ent_1,
		const mapping::rename_type::UUID& ent_2)
	{
		//// 判定先のレジストリ
		//result_1.registry = registry_2;
		//result_2.registry = registry_1;
		//// 判定先のコリジョンタグ
		//result_1.collision_tag = sphere_2.collision_tag;
		//result_2.collision_tag = sphere_1.collision_tag;
		//// 判定先のコリジョンID
		//result_1.collision_id = sphere_2.id;
		//result_2.collision_id = sphere_1.id;
		//// 判定先ののエンティティ
		//result_1.entity = ent_2;
		//result_2.entity = ent_1;

		HitResult result_1_{};
		result_1_.registry		= registry_2;
		result_1_.collision_tag = sphere_2.collision_tag;
		result_1_.collision_id	= sphere_2.id;
		result_1_.entity		= ent_2;
		sphere_1.RegisterHitEntity(ent_2, result_1_);

		HitResult result_2_{};
		result_2_.registry		= registry_1;
		result_2_.collision_tag = sphere_1.collision_tag;
		result_2_.collision_id	= sphere_1.id;
		result_2_.entity		= ent_1;
		sphere_2.RegisterHitEntity(ent_1, result_2_);
	}

	void CollisionManager::UpdateHitResult(
		ecs::Registry* registry_1, ecs::Registry* registry_2,
		Capsule&	   capsule_1 , Capsule&       capsule_2,
		const mapping::rename_type::UUID& ent_1,
		const mapping::rename_type::UUID& ent_2)
	{
		//// 判定先のレジストリ
		//result_1.registry = registry_2;
		//result_2.registry = registry_1;
		//// 判定先のコリジョンタグ
		//result_1.collision_tag = capsule_2.collision_tag;
		//result_2.collision_tag = capsule_1.collision_tag;
		//// 判定先のコリジョンID
		//result_1.collision_id = capsule_2.id;
		//result_2.collision_id = capsule_1.id;
		//// 判定先ののエンティティ
		//result_1.entity = ent_2;
		//result_2.entity = ent_1;

		HitResult result_1_{};
		result_1_.registry		= registry_2;
		result_1_.collision_tag = capsule_2.collision_tag;
		result_1_.collision_id	= capsule_2.id;
		result_1_.entity		= ent_2;
		capsule_1.RegisterHitEntity(ent_2, result_1_);

		HitResult result_2_{};
		result_2_.registry		= registry_1;
		result_2_.collision_tag = capsule_1.collision_tag;
		result_2_.collision_id	= capsule_1.id;
		result_2_.entity		= ent_1;
		capsule_2.RegisterHitEntity(ent_1, result_2_);
	}

	void CollisionManager::UpdateHitResult(
		ecs::Registry* registry_1, ecs::Registry* registry_2,
		Sphere&		   sphere	 , Capsule&		  capsule,
		const mapping::rename_type::UUID& s_ent,
		const mapping::rename_type::UUID& c_ent)
	{
		//// 判定先のレジストリ
		//s_result.registry = registry_2;
		//c_result.registry = registry_1;
		//// 判定先のコリジョンタグ
		//s_result.collision_tag = capsule.collision_tag;
		//c_result.collision_tag = sphere.collision_tag;
		//// 判定先のコリジョンID
		//s_result.collision_id = capsule.id;
		//c_result.collision_id = sphere.id;
		//// 判定先ののエンティティ
		//s_result.entity = c_ent;
		//c_result.entity = s_ent;

		HitResult result_1_{};
		result_1_.registry		= registry_2;
		result_1_.collision_tag = capsule.collision_tag;
		result_1_.collision_id	= capsule.id;
		result_1_.entity		= c_ent;
		sphere.RegisterHitEntity(c_ent, result_1_);

		HitResult result_2_{};
		result_2_.registry		= registry_1;
		result_2_.collision_tag = sphere.collision_tag;
		result_2_.collision_id	= sphere.id;
		result_2_.entity		= s_ent;
		capsule.RegisterHitEntity(s_ent, result_2_);
	}

	void CollisionManager::Extrude(
		const float dt,
		ecs::Registry* registry,
		const mapping::rename_type::Entity& ent_1,
		const mapping::rename_type::Entity& ent_2,
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
			const float restitution = CalculateRestitution(registry->TryGetComponent<component::PhysicMaterialComponent>(ent_1),
														   registry->TryGetComponent<component::PhysicMaterialComponent>(ent_2));

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

	void CollisionManager::Extrude(
		const float dt,
		ecs::Registry* registry_1, ecs::Registry* registry_2,
		const mapping::rename_type::Entity& ent_1,
		const mapping::rename_type::Entity& ent_2,
		const DirectX::SimpleMath::Vector3& mass_point_1,
		const DirectX::SimpleMath::Vector3& mass_point_2,
		const CollisionPreset collision_preset_1,
		const CollisionPreset collision_preset_2,
		const float penetration)
	{
		// 条件 1 : お互いRigidBodyComponentを持っていること
		// 条件 2 : お互いのCollisionPresetがBlockAllであること
		if (!registry_1->TryGetComponent<component::RigidBodyComponent>(ent_1)) return;
		if (!registry_2->TryGetComponent<component::RigidBodyComponent>(ent_2)) return;
		if (collision_preset_1 != CollisionPreset::BlockAll) return;
		if (collision_preset_2 != CollisionPreset::BlockAll) return;

		auto& rigid_body_comp_1 = registry_1->GetComponent<component::RigidBodyComponent>(ent_1);
		auto& rigid_body_comp_2 = registry_2->GetComponent<component::RigidBodyComponent>(ent_2);

		DirectX::SimpleMath::Vector3 n{ mass_point_2 - mass_point_1 };
		n.Normalize();

		const float m1 = rigid_body_comp_1.GetMass();
		const float m2 = rigid_body_comp_2.GetMass();

		// 衝突前の速度
		const float pre_v1 = rigid_body_comp_1.GetVelocity().Dot(n);
		const float pre_v2 = rigid_body_comp_2.GetVelocity().Dot(n);
		if (pre_v1 - pre_v2 > 0)
		{
			// 反発係数の取得
			const float restitution = CalculateRestitution(registry_1->TryGetComponent<component::PhysicMaterialComponent>(ent_1),
														   registry_2->TryGetComponent<component::PhysicMaterialComponent>(ent_2));

			// 衝突後の速度
			const float post_v1 = (m1 * pre_v1 + m2 * pre_v2 + restitution * m2 * (pre_v2 - pre_v1)) / (m1 + m2);
			const float post_v2 = (m1 * pre_v1 + m2 * pre_v2 + restitution * m1 * (pre_v1 - pre_v2)) / (m1 + m2);

			rigid_body_comp_1.AddVelocity((post_v1 - pre_v1) * n);
			rigid_body_comp_1.Integrate(dt);
			rigid_body_comp_2.AddVelocity((post_v2 - pre_v2) * n);
			rigid_body_comp_2.Integrate(dt);
		}

		registry_1->GetComponent<component::TransformComponent>(ent_1).AdjustPosition(-(m2 / (m1 + m2) * penetration * n));
		registry_2->GetComponent<component::TransformComponent>(ent_2).AdjustPosition(m1 / (m1 + m2) * penetration * n);
	}

	bool CollisionManager::IntersectRayVsDragModel(
		ecs::Registry* registry,
		const DirectX::SimpleMath::Vector3& ray_start,
		const DirectX::SimpleMath::Vector3& ray_end,
		DirectX::SimpleMath::Vector3* hit_pos)
	{
		DirectX::XMVECTOR world_start = DirectX::XMLoadFloat3(&ray_start);
		DirectX::XMVECTOR world_end	  = DirectX::XMLoadFloat3(&ray_end);

		// ent_terrainsとRayCastComponentを持つモデルでレイキャストの判定を行う
		for (const auto& ent_terrain : ent_terrains)
		{
			if (!registry->GetEntities().contains(ent_terrain))
				continue;
			const component::ModelComponent& model_comp = registry->GetComponent<component::ModelComponent>(ent_terrain);

			asset::Model& model = locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_comp.GetModelID());
			for (const asset::ModelData::Mesh& mesh : model.GetModelData().GetMeshes())
			{
				const component::ModelComponent::Node node = model_comp.GetNodes().at(mesh.node_index);

				// レイをワールド空間からローカル空間へ変換
				DirectX::XMMATRIX world_transform		  = DirectX::XMLoadFloat4x4(&node.world_transform);
				DirectX::XMMATRIX inverse_world_transform = DirectX::XMMatrixInverse(nullptr, world_transform);

				DirectX::XMVECTOR start  = DirectX::XMVector3TransformCoord(world_start, inverse_world_transform);
				DirectX::XMVECTOR end	 = DirectX::XMVector3TransformCoord(world_end, inverse_world_transform);
				DirectX::XMVECTOR vec	 = DirectX::XMVectorSubtract(end, start);
				DirectX::XMVECTOR dir	 = DirectX::XMVector3Normalize(vec);
				DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);

				// レイの長さ
				float neart{};
				DirectX::XMStoreFloat(&neart, length);

				// 三角形(面)との交差判定
				const std::vector<asset::ModelData::Vertex>& vertices = mesh.vertices;
				const std::vector<UINT> indices = mesh.indices;

				for (const asset::ModelData::Subset& subset : mesh.subsets)
				{
					for (UINT i = 0; i < subset.index_count; i += 3)
					{
						UINT index = subset.start_index + i;
						// 三角形の頂点を抽出
						const asset::ModelData::Vertex& a = vertices.at(indices.at(index));
						const asset::ModelData::Vertex& b = vertices.at(indices.at(index + 1));
						const asset::ModelData::Vertex& c = vertices.at(indices.at(index + 2));

						DirectX::XMVECTOR A = DirectX::XMLoadFloat4(&a.position);
						DirectX::XMVECTOR B = DirectX::XMLoadFloat4(&b.position);
						DirectX::XMVECTOR C = DirectX::XMLoadFloat4(&c.position);

						// 三角形の三辺ベクトルを算出
						DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
						DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
						DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

						//三角形の法線ベクトルを算出
						DirectX::XMVECTOR normal = DirectX::XMVector3Cross(AB, BC);

						// 内積の結果がプラスならば裏向き
						DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(dir, normal);
						float dot;
						DirectX::XMStoreFloat(&dot, Dot);
						if (dot >= 0)continue;

						// レイとの平面の交点を算出
						DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, start);
						DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(normal, V), Dot);
						float t;
						DirectX::XMStoreFloat(&t, T);
						// 交点までの距離が今までに計算した最近距離より大きい時はスキップ
						if (t<0.0f || t>neart)continue;

						DirectX::XMVECTOR position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, T), start);

						// 交点が三角形の内側にあるか判定
						// １つめ
						DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, position);
						DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
						DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, normal);
						float dot1;
						DirectX::XMStoreFloat(&dot1, Dot1);
						if (dot1 < 0.0f)continue;

						// ２つめ
						DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, position);
						DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
						DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, normal);
						float dot2;
						DirectX::XMStoreFloat(&dot2, Dot2);
						if (dot2 < 0.0f)continue;

						// ３つめ
						DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, position);
						DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
						DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, normal);
						float dot3;
						DirectX::XMStoreFloat(&dot3, Dot3);
						if (dot3 < 0.0f)continue;

						// 最近距離を更新
						neart = t;

						// 交点と法線を更新
						DirectX::XMVECTOR HitPos = DirectX::XMVector3TransformCoord(position, world_transform);
						DirectX::XMStoreFloat3(hit_pos, HitPos);
						return true;
					}
				}
			}
		}

		return false;
	}

	bool CollisionManager::IntersectRayVsModel(
		const float dt,
		ecs::Registry* registry,
		const component::ModelComponent& model,
		component::RayCastComponent& ray_cast_comp)
	{
		bool hit = false;
		//const FbxModelResource* resource = model.GetResource();
		//for (auto& ray : ray_cast_comp.GetRays())
		//{
		//	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&ray.second.ray_start);
		//	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&ray.second.ray_end);
		//	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
		//	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
		//	ray.second.hit_result.is_hit = false;
		//	ray.second.is_block_hit		 = false;
		//	ray.second.terrain_attribute = utility::TerrainAttribute::NotAttribute;

		//	// ワールド空間のレイの長さ
		//	DirectX::XMStoreFloat(&ray.second.hit_result.distance, WorldRayLength);

		//	for (const ModelData::Mesh& mesh : resource->GetModelData().GetMeshes())
		//	{
		//		const component::FbxModelComponent::Node node = model.GetNodes().at(mesh.node_index);

		//		// レイをワールド空間からローカル空間へ変換
		//		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.world_transform);
		//		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		//		DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		//		DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		//		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
		//		DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
		//		DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

		//		// レイの長さ
		//		float neart;
		//		DirectX::XMStoreFloat(&neart, Length);

		//		// 三角形(面)との交差判定
		//		const std::vector<ModelData::Vertex>& vertices = mesh.vertices;
		//		const std::vector<UINT> indices = mesh.indices;

		//		int materialIndex = -1;
		//		DirectX::XMVECTOR HitPosition;
		//		DirectX::XMVECTOR HitNormal;
		//		for (const ModelData::Subset& subset : mesh.subsets)
		//		{
		//			for (UINT i = 0; i < subset.index_count; i += 3)
		//			{
		//				UINT index = subset.start_index + i;
		//				// 三角形の頂点を抽出
		//				const ModelData::Vertex& a = vertices.at(indices.at(index));
		//				const ModelData::Vertex& b = vertices.at(indices.at(index + 1));
		//				const ModelData::Vertex& c = vertices.at(indices.at(index + 2));

		//				DirectX::XMVECTOR A = DirectX::XMLoadFloat4(&a.position);
		//				DirectX::XMVECTOR B = DirectX::XMLoadFloat4(&b.position);
		//				DirectX::XMVECTOR C = DirectX::XMLoadFloat4(&c.position);

		//				// 三角形の三辺ベクトルを算出
		//				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
		//				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
		//				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

		//				//三角形の法線ベクトルを算出
		//				DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

		//				// 内積の結果がプラスならば裏向き
		//				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
		//				float dot;
		//				DirectX::XMStoreFloat(&dot, Dot);
		//				if (dot >= 0)continue;

		//				// レイとの平面の交点を算出
		//				DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, Start);
		//				DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
		//				float t;
		//				DirectX::XMStoreFloat(&t, T);
		//				// 交点までの距離が今までに計算した最近距離より大きい時はスキップ
		//				if (t<0.0f || t>neart)continue;

		//				DirectX::XMVECTOR Position = Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

		//				// 交点が三角形の内側にあるか判定
		//				// １つめ
		//				DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, Position);
		//				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
		//				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal);
		//				float dot1;
		//				DirectX::XMStoreFloat(&dot1, Dot1);
		//				if (dot1 < 0.0f)continue;

		//				// ２つめ
		//				DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, Position);
		//				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
		//				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal);
		//				float dot2;
		//				DirectX::XMStoreFloat(&dot2, Dot2);
		//				if (dot2 < 0.0f)continue;

		//				// ３つめ
		//				DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position);
		//				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
		//				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal);
		//				float dot3;
		//				DirectX::XMStoreFloat(&dot3, Dot3);
		//				if (dot3 < 0.0f)continue;

		//				// 最近距離を更新
		//				neart = t;

		//				// 交点と法線を更新
		//				HitPosition		= Position;
		//				HitNormal		= Normal;
		//				materialIndex	= subset.material_index;
		//				break;
		//			}
		//		}
		//		if (materialIndex >= 0)
		//		{
		//			// ローカル空間からワールド空間へ変換
		//			DirectX::XMVECTOR WorldPosition		= DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
		//			DirectX::XMVECTOR WorldCrossVec		= DirectX::XMVectorSubtract(WorldPosition, WorldStart);
		//			DirectX::XMVECTOR WorldCrossLength	= DirectX::XMVector3Length(WorldCrossVec);
		//			float distance;
		//			DirectX::XMStoreFloat(&distance, WorldCrossLength);
		//			utility::MaterialDiscrimination material_discrimination{};

		//			// ヒット情報保存
		//			if (ray.second.hit_result.distance > distance)
		//			{
		//				DirectX::XMVECTOR WorldNormal			= DirectX::XMVector3Transform(HitNormal, WorldTransform);
		//				ray.second.hit_result.distance			= distance;
		//				ray.second.hit_result.material_index	= materialIndex;
		//				ray.second.hit_result.is_hit			= true;
		//				DirectX::XMStoreFloat3(&ray.second.hit_result.position, WorldPosition);
		//				DirectX::XMStoreFloat3(&ray.second.hit_result.normal, DirectX::XMVector3Normalize(WorldNormal));
		//				ray_cast_comp.SetTerrainAttribute(ray.first,material_discrimination.GetTerrainAttribute(model.GetResource()->GetModelData().GetMaterials().at(materialIndex).texture_name)); // マテリアルのテクスチャ名からマテリアルの属性を取
		//				hit = true;
		//			}

		//			{// 押出し処理
		//				// 押出し処理を行わない場合は処理を抜ける(次のレイとの処理に移る)
		//				if(!ray.second.is_block)
		//					break;

		//				//-- 押出し方法 --//
		//				/*
		//				 * Rayのブロック位置から地形の位置までのベクトルを作成しrayベクトル
		//				 * と方向が逆(角度が90度以上)なら押し出し処理を行う
		//				 */

		//				// Rayの始点から地形の位置までのベクトル
		//				const DirectX::SimpleMath::Vector3 v1{ ray.second.hit_result.position - ray.second.block_pos };
		//				DirectX::SimpleMath::Vector3 v1_n = v1;
		//				v1_n.Normalize();
		//				// Rayベクトル
		//				const DirectX::SimpleMath::Vector3 v2{ ray.second.ray_end - ray.second.ray_start };
		//				DirectX::SimpleMath::Vector3 v2_n = v2;
		//				v2_n.Normalize();
		//				float dot = v1.Dot(v2);
		//				// ベクトル間の角度が90度未満なら処理を抜ける(次のレイとの処理に移る)
		//				if (v1_n.Dot(v2_n) > 0)
		//					break;

		//				auto& transform_comp = registry->GetComponent<component::TransformComponent>(ray_cast_comp.GetEntity());
		//				transform_comp.AdjustPosition(v1);
		//				ray.second.is_block_hit = true;
		//				break;
		//			}
		//		}
		//	}
		//}
		return hit;
	}

	bool CollisionManager::IntersectSphereVsSphere(
		const float dt,
		ecs::Registry* registry,
		component::SphereCollisionComponent& sphere_1,
		component::SphereCollisionComponent& sphere_2)
	{
		bool hit = false;
		for(auto& s1 : sphere_1.GetSpheres() | std::views::values)
		{
			for(auto& s2 : sphere_2.GetSpheres() | std::views::values)
			{
				const DirectX::SimpleMath::Vector3 s1_translation = s1.world_transform_matrix.Translation();
				const DirectX::SimpleMath::Vector3 s2_translation = s2.world_transform_matrix.Translation();

				const DirectX::SimpleMath::Vector3 v = { s1_translation - s2_translation };
				const float len = v.Length();

				if (len <= s1.radius + s2.radius)
				{
					hit = true;
					AddHitSphere(s1.id, sphere_1.GetEntity(), sphere_2.GetEntity());
					AddHitSphere(s2.id, sphere_2.GetEntity(), sphere_1.GetEntity());
					// ヒット情報の更新
					UpdateHitResult(sphere_1.GetRegistry(),sphere_2.GetRegistry(),
									s1, s2,
									sphere_1.GetEntity(), sphere_2.GetEntity());

					// 押出し処理
					Extrude(
						dt,
						registry,
						sphere_1.GetEntity(), sphere_2.GetEntity(),
						s1_translation, s2_translation,
						s1.collision_preset, s2.collision_preset,
						(s1.radius + s2.radius) - len);
				}
				else
				{
					if (auto* hit_result_1 = s1.TryGetHitResult(sphere_2.GetEntity());
						hit_result_1)
					{
						hit_result_1->is_hit = false;
					}
					if (auto* hit_result_2 = s2.TryGetHitResult(sphere_1.GetEntity());
						hit_result_2)
					{
						hit_result_2->is_hit = false;
					}
				}

			}
		}
		return hit;
	}

	bool CollisionManager::IntersectSphereVsSphere(
		const float dt,
		ecs::Registry* registry_1, ecs::Registry* registry_2,
		component::SphereCollisionComponent& sphere_1,
		component::SphereCollisionComponent& sphere_2)
	{
		bool hit = false;
		for(auto& s1 : sphere_1.GetSpheres() | std::views::values)
		{
			for(auto& s2 : sphere_2.GetSpheres() | std::views::values)
			{
				const DirectX::SimpleMath::Vector3 s1_translation = s1.world_transform_matrix.Translation();
				const DirectX::SimpleMath::Vector3 s2_translation = s2.world_transform_matrix.Translation();

				const DirectX::SimpleMath::Vector3 v = { s1_translation - s2_translation };

				if (const float len = v.Length();
					len <= s1.radius + s2.radius)
				{
					hit = true;
					AddHitSphere(s1.id, sphere_1.GetEntity(), sphere_2.GetEntity());
					AddHitSphere(s2.id, sphere_2.GetEntity(), sphere_1.GetEntity());
					// ヒット情報の更新
					UpdateHitResult(sphere_1.GetRegistry(),sphere_2.GetRegistry(),
									s1, s2,
									sphere_1.GetEntity(), sphere_2.GetEntity());

					// 押出し処理
					Extrude(
						dt,
						registry_1,registry_2,
						sphere_1.GetEntity(), sphere_2.GetEntity(),
						s1_translation, s2_translation,
						s1.collision_preset, s2.collision_preset,
						(s1.radius + s2.radius) - len);
				}
				else
				{

					if (auto* hit_result_1 = s1.TryGetHitResult(sphere_2.GetEntity());
						hit_result_1)
					{
						hit_result_1->is_hit = false;
					}
					if (auto* hit_result_2 = s2.TryGetHitResult(sphere_1.GetEntity());
						hit_result_2)
					{
						hit_result_2->is_hit = false;
					}
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

		for(auto& c1 : capsule_1.GetCapsules() | std::views::values)
		{
			for(auto& c2 : capsule_2.GetCapsules() | std::views::values)
			{
				// それぞれのカプセルの線分(始点)からの大きさ
				float c1_t, c2_t;
				DirectX::SimpleMath::Vector3 c1_p, c2_p;

				// 線分の最近距離
				const float len = arithmetic::ClosestPtSegmentSegment(
									c1.start, c1.end,
									c2.start, c2.end,
									c1_t, c2_t,
									c1_p, c2_p);

				if(len <c1.radius + c2.radius)
				{
					AddHitCapsule(c1.id, HitData{ capsule_1.GetEntity(),capsule_2.GetEntity() });
					AddHitCapsule(c2.id, HitData{ capsule_2.GetEntity(),capsule_1.GetEntity() });
					// ヒット情報の更新
					UpdateHitResult(capsule_1.GetRegistry(),capsule_2.GetRegistry(),
									c1, c2,
									capsule_1.GetEntity(), capsule_2.GetEntity());
					hit = true;

					// 押出し処理
					Extrude(
						dt,
						registry,
						capsule_1.GetEntity(), capsule_2.GetEntity(),
						c1_p, c2_p,
						c1.collision_preset,
						c2.collision_preset,
						(c1.radius + c2.radius) - len);
				}
				else
				{
					if (auto* hit_result_1 = c1.TryGetHitResult(capsule_2.GetEntity());
						hit_result_1)
					{
						hit_result_1->is_hit = false;
					}
					if (auto* hit_result_2 = c2.TryGetHitResult(capsule_1.GetEntity());
						hit_result_2)
					{
						hit_result_2->is_hit = false;
					}
				}
			}
		}
		return hit;
	}

	bool CollisionManager::IntersectCapsuleVsCapsule(
		const float dt,
		ecs::Registry* registry_1, ecs::Registry* registry_2,
		component::CapsuleCollisionComponent& capsule_1,
		component::CapsuleCollisionComponent& capsule_2)
	{
		bool hit = false;

		for (auto& c1 : capsule_1.GetCapsules() | std::views::values)
		{
			for (auto& c2 : capsule_2.GetCapsules() | std::views::values)
			{
				// それぞれのカプセルの線分(始点)からの大きさ
				float c1_t, c2_t;
				DirectX::SimpleMath::Vector3 c1_p, c2_p;

				// 線分の最近距離
				const float len = arithmetic::ClosestPtSegmentSegment(
					c1.start, c1.end,
					c2.start, c2.end,
					c1_t, c2_t,
					c1_p, c2_p);

				if (len < c1.radius + c2.radius)
				{
					AddHitCapsule(c1.id, HitData{ capsule_1.GetEntity(),capsule_2.GetEntity() });
					AddHitCapsule(c2.id, HitData{ capsule_2.GetEntity(),capsule_1.GetEntity() });
					// ヒット情報の更新
					UpdateHitResult(capsule_1.GetRegistry(),capsule_2.GetRegistry(),
									c1, c2,
									capsule_1.GetEntity(), capsule_2.GetEntity());
					hit = true;

					// 押出し処理
					Extrude(
						dt,
						registry_1, registry_2,
						capsule_1.GetEntity(), capsule_2.GetEntity(),
						c1_p, c2_p,
						c1.collision_preset,
						c2.collision_preset,
						(c1.radius + c2.radius) - len);
				}
				else
				{
					if (auto* hit_result_1 = c1.TryGetHitResult(capsule_2.GetEntity());
						hit_result_1)
					{
						hit_result_1->is_hit = false;
					}
					if (auto* hit_result_2 = c2.TryGetHitResult(capsule_1.GetEntity());
						hit_result_2)
					{
						hit_result_2->is_hit = false;
					}
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

		for (auto& s : sphere.GetSpheres() | std::views::values)
		{
			for (auto& c : capsule.GetCapsules() | std::views::values)
			{
				// 球の中心とカプセルの線分の間の(平方した)距離の算出
				const float dist = arithmetic::SqDistPointSegment(c.start, c.end, s.world_transform_matrix.Translation());
				if(const float radius = s.radius + c.radius;
				   dist <= radius * radius)
				{
					AddHitSphere(s.id, sphere.GetEntity(), capsule.GetEntity());
					AddHitCapsule(c.id, HitData{ capsule.GetEntity(),sphere.GetEntity() });
					// ヒット情報の更新
					UpdateHitResult(sphere.GetRegistry(),capsule.GetRegistry(),
									s, c,
									sphere.GetEntity(), capsule.GetEntity());
					hit = true;
					float t = 0; //	最近接点までの距離(線分の割合)
					DirectX::SimpleMath::Vector3 p{}; // カプセルの最近接点
					arithmetic::ClosestPtPointSegment(c.start, c.end, s.world_transform_matrix.Translation(), p, t);
					// 押出し処理
					Extrude(
						dt,
						registry,
						sphere.GetEntity(), capsule.GetEntity(),
						s.world_transform_matrix.Translation(), p,
						s.collision_preset, c.collision_preset,
						(s.radius + c.radius) - std::sqrtf(dist));
				}
				else
				{
					if (auto* hit_result_1 = s.TryGetHitResult(capsule.GetEntity());
						hit_result_1)
					{
						hit_result_1->is_hit = false;
					}
					if (auto* hit_result_2 = c.TryGetHitResult(sphere.GetEntity());
						hit_result_2)
					{
						hit_result_2->is_hit = false;
					}
				}
			}
		}
		return hit;
	}

	bool CollisionManager::IntersectSphereVsCapsule(
		const float dt,
		ecs::Registry* registry_1, ecs::Registry* registry_2,
		component::SphereCollisionComponent& sphere,
		component::CapsuleCollisionComponent& capsule)
	{
		bool hit = false;

		// 同じエンティティ同士なら判定処理を行わない
		if (sphere.GetEntity() == capsule.GetEntity())
			return hit;

		for (auto& s : sphere.GetSpheres() | std::views::values)
		{
			for (auto& c : capsule.GetCapsules() | std::views::values)
			{
				// 球の中心とカプセルの線分の間の(平方した)距離の算出
				const float dist = arithmetic::SqDistPointSegment(c.start, c.end, s.world_transform_matrix.Translation());
				// (平方した)距離が(平方した)半径の総和よりも小さい場合は衝突
				if (const float radius = s.radius + c.radius;
					dist <= radius * radius)
				{
					AddHitSphere(s.id, sphere.GetEntity(), capsule.GetEntity());
					AddHitCapsule(c.id, HitData{ capsule.GetEntity(),sphere.GetEntity() });
					// ヒット情報の更新
					UpdateHitResult(sphere.GetRegistry(),capsule.GetRegistry(),
									s, c,
									sphere.GetEntity(), capsule.GetEntity());
					hit = true;
					float t = 0; //	最近接点までの距離(線分の割合)
					DirectX::SimpleMath::Vector3 p{}; // カプセルの最近接点
					arithmetic::ClosestPtPointSegment(c.start, c.end, s.world_transform_matrix.Translation(), p, t);
					// 押出し処理
					Extrude(
						dt,
						registry_1, registry_2,
						sphere.GetEntity(), capsule.GetEntity(),
						s.world_transform_matrix.Translation(), p,
						s.collision_preset, c.collision_preset,
						(s.radius + c.radius) - std::sqrtf(dist));
				}
				else
				{
					if (auto* hit_result_1 = s.TryGetHitResult(capsule.GetEntity());
						hit_result_1)
					{
						hit_result_1->is_hit = false;
					}
					if (auto* hit_result_2 = c.TryGetHitResult(sphere.GetEntity());
						hit_result_2)
					{
						hit_result_2->is_hit = false;
					}
				}
			}
		}
		return hit;
	}


} // cumulonimbus::collision
