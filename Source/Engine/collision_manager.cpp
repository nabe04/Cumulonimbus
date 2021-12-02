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
		// System::Render�֐����Ŏg�p����֐��̓o�^
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

			// �S�Ẵ��f���̃t�@�C���p�X(�g���q���܂�)
			std::vector<std::filesystem::path> items_path{};
			items_path.reserve(model_asset_size);
			// �S�Ẵ��f���̃��f����(�g���q���܂܂Ȃ�)
			std::vector<std::string> items_name{};
			items_name.reserve(model_asset_size);
			for (const auto& asset_path : asset_manager.GetAssetSheetManager().GetSheet<asset::Model>().sheet | std::views::values)
			{
				// �A�Z�b�g�t�@�C���p�X�̒ǉ�
				items_path.emplace_back(asset_path);
				// �A�Z�b�g�t�@�C����(�g���q���܂܂Ȃ�)�̒ǉ�
				items_name.emplace_back(std::filesystem::path{ asset_path }.filename().replace_extension().string());
			}


			{// �����蔻��p���̃A�Z�b�g�I��
				std::string current_sphere_name{};
				std::string current_sphere_path{};
				if (asset_manager.GetLoader<asset::ModelLoader>()->HasModel(sphere_id))
				{
					current_sphere_name = asset_sheet_manager.GetAssetName<asset::Model>(sphere_id);
					current_sphere_path = asset_sheet_manager.GetAssetFilename<asset::Model>(sphere_id);
				}


				if (helper::imgui::Combo("Sphere Model", current_sphere_name, items_name))
				{// ImGui::Combo���ŃA�C�e���I����
					// �o�^����Ă��郂�f���A�Z�b�g�f�[�^����I�����ꂽ�A�Z�b�g��T��
					for (const auto& [asset_id, asset_path] : asset_sheet_manager.GetSheet<asset::Model>().sheet)
					{
						// �I������Ă���A�Z�b�g���Ɠo�^����Ă��閼���Ⴄ�ꍇ�������΂�
						if (current_sphere_name != asset_sheet_manager.GetAssetName<asset::Model>(asset_id))
							continue;
						// �ʂ̃A�Z�b�g���I�����ꂽ�ꍇ��sphere_id�ɑ������
						if (asset_id != sphere_id)
						{
							sphere_id = asset_id;
						}
					}
				}
			}

			{// �����蔻��p�J�v�Z���̃A�Z�b�g�I��
				std::string current_capsule_name{};
				std::string current_capsule_path{};
				if (asset_manager.GetLoader<asset::ModelLoader>()->HasModel(capsule_id))
				{
					current_capsule_name = asset_sheet_manager.GetAssetName<asset::Model>(capsule_id);
					current_capsule_path = asset_sheet_manager.GetAssetFilename<asset::Model>(capsule_id);
				}
				if (helper::imgui::Combo("Capsule Model", current_capsule_name, items_name))
				{// ImGui::Combo���ŃA�C�e���I����
					// �o�^����Ă��郂�f���A�Z�b�g�f�[�^����I�����ꂽ�A�Z�b�g��T��
					for (const auto& [asset_id, asset_path] : asset_sheet_manager.GetSheet<asset::Model>().sheet)
					{
						// �I������Ă���A�Z�b�g���Ɠo�^����Ă��閼���Ⴄ�ꍇ�������΂�
						if (current_capsule_name != asset_sheet_manager.GetAssetName<asset::Model>(asset_id))
							continue;
						// �ʂ̃A�Z�b�g���I�����ꂽ�ꍇ��capsule_id�ɑ������
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
		// ���ɓo�^����Ă���ꍇ�͏����𔲂���
		if (hit_entities.contains(ent))
			return;
		// �q�b�g�G���e�B�e�B�̒ǉ�
		hit_entities.emplace(ent);
	}

	void CollisionManager::HitData::UnRegisterHitEntity(const mapping::rename_type::Entity& ent)
	{
		// �o�^����Ă��Ȃ��ꍇ�����𔲂���
		if (!hit_entities.contains(ent))
			return;
		// �q�b�g�G���e�B�e�B�̍폜
		hit_entities.erase(ent);
	}

	CollisionManager::CollisionManager(system::System& system)
	{
		// System::Render�֐����Ŏg�p����֐��̓o�^
		system.RegisterRenderFunction(utility::GetHash<CollisionManager>(),
									  [&](ecs::Registry* registry) {RenderImGui(registry); });
	}

	void CollisionManager::Update(const float dt, ecs::Registry* registry)
	{
		//-- �����m�̔��� --//
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

		//-- �J�v�Z�����m�̔��� --//
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

		//-- ���ƃJ�v�Z���̔��� --//
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

		//-- �n�`�ƃ��f���Ƃ̔���(���C�L���X�g) --//
		for (auto& ray_cast_array = registry->GetArray<component::RayCastComponent>();
			 auto& ray_comp : ray_cast_array.GetComponents())
		{
			mapping::rename_type::Entity ent = ray_comp.GetEntity();

			// ent_terrains��RayCastComponent�������f���Ń��C�L���X�g�̔�����s��
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

		//-- ����V�[�����ł̔��菈�� --//
		for(const auto& scene : scene_manager.GetActiveScenes() | std::views::values)
		{
			auto* registry = scene->GetRegistry();

			//-- �����m�̔��� --//
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

			//-- �J�v�Z�����m�̔��� --//
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

			//-- ���ƃJ�v�Z���̔��� --//
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

			//-- �n�`�ƃ��f���Ƃ̔���(���C�L���X�g) --//
			for (auto& ray_cast_array = registry->GetArray<component::RayCastComponent>();
				auto & ray_comp : ray_cast_array.GetComponents())
			{
				mapping::rename_type::Entity ent = ray_comp.GetEntity();

				// ent_terrains��RayCastComponent�������f���Ń��C�L���X�g�̔�����s��
				for (const auto& ent_terrain : ent_terrains)
				{
					const auto& model = registry->GetComponent<component::ModelComponent>(ent_terrain);
					IntersectRayVsModel(dt, registry, model, ray_comp);
				}
			}
		}

		//-- �ʃV�[���ł̔��菈�� --//
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
					// �V�[��1�̃��W�X�g��(1�A�Q�͎��ʔԍ�)
					auto* registry_1 = scene_manager.GetActiveScenes().at(scenes_id.at(scene_1))->GetRegistry();
					// �V�[��2�̃��W�X�g��(1�A�Q�͎��ʔԍ�)
					auto* registry_2 = scene_manager.GetActiveScenes().at(scenes_id.at(scene_2))->GetRegistry();

					//-- �����m�̔��� --//
					auto& sphere_collisions_1 = registry_1->GetArray<component::SphereCollisionComponent>();
					auto& sphere_collisions_2 = registry_2->GetArray<component::SphereCollisionComponent>();
					const auto s1_size = sphere_collisions_1.Size();
					const auto s2_size = sphere_collisions_2.Size();
					if (s1_size >= 1 &&
						s2_size >= 1)
					{// ���ꂼ��̃V�[���ɋ��R���C�_�[��1�ȏ㑶�݂���ꍇ�ɔ��菈�����s��
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


					//-- �J�v�Z�����m�̔��� --//
					auto& capsule_collisions_1 = registry_1->GetArray<component::CapsuleCollisionComponent>();
					auto& capsule_collisions_2 = registry_2->GetArray<component::CapsuleCollisionComponent>();
					const auto c1_size = capsule_collisions_1.Size();
					const auto c2_size = capsule_collisions_2.Size();
					if (c1_size >= 1 &&
						c2_size >= 1)
					{// ���ꂼ��̃V�[���ɃJ�v�Z���R���C�_�[��1�ȏ㑶�݂���ꍇ�ɔ��菈�����s��
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

					//-- ���ƃJ�v�Z�����m�̔��� --//
					// �V�[��1(��)�A�V�[��2(�J�v�Z��)�̏ꍇ
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
					// �V�[��1(�J�v�Z��)�A�V�[��2(��)�̏ꍇ
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

		//-- ���肳�ꂽ�R���W�����̔����true�� --//
		for (const auto& scene : scene_manager.GetActiveScenes() | std::views::values)
		{
			auto* registry = scene->GetRegistry();

			for (const auto& [sphere_id, hit_data] : hit_ent_sphere)
			{// ���肳�ꂽ����񂪈�v�����ꍇ����(is_hit)��true��
				if (!registry->HasComponent<component::SphereCollisionComponent>(hit_data.self_entity))
					continue;
				auto* sphere = registry->GetComponent<component::SphereCollisionComponent>(hit_data.self_entity).TryGetSphere(sphere_id);
				if (!sphere)
					continue;

				// ���̃G���e�B�e�B�ʂ̔����True��
				for (auto& [hit_ent, hit_result] : sphere->hit_results)
				{
					// ���肳�ꂽ�������R���C�_�[�̃��X�g���ɑ��݂���ꍇ�̂ݔ����True�ɂ���
					if (hit_data.hit_entities.contains(hit_ent))
						hit_result.is_hit = true;
				}
			}

			for (const auto& [capsule_id, hit_data] : hit_ent_capsule)
			{// ���肳�ꂽ�J�v�Z����񂪈�v�����ꍇ����(is_hit)��true��
				if (!registry->HasComponent<component::CapsuleCollisionComponent>(hit_data.self_entity))
					continue;
				auto* capsule = registry->GetComponent<component::CapsuleCollisionComponent>(hit_data.self_entity).TryGetCapsule(capsule_id);
				if (!capsule)
					continue;
				// �J�v�Z���̃G���e�B�e�B�ʂ̔����True��
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
				{//	���C�L���X�g�Ώۂ��i�[����R���e�i�̒ǉ�
					ent_terrains.emplace_back();
				}
				ImGui::SameLine();
				if(ImGui::Button(ICON_FA_MINUS))
				{// ���C�L���X�g�Ώۂ��i�[����R���e�i�̍폜(��Ԍ��̃R���e�i)
					ent_terrains.pop_back();
				}
				for(int no = 0; auto& terrain : ent_terrains)
				{// ���C�L���X�g�Ώۂ̃G���e�B�e�B��I��
					std::string combo_name = "Terrain(" + std::to_string(no++) + ")";

					if (ImGui::BeginCombo(combo_name.c_str(), registry->GetName(terrain).c_str()))
					{
						for (const auto& [key, value] : registry->GetEntities())
						{
							const bool is_selected = (registry->GetName(terrain) == value.second);
							if (ImGui::Selectable(value.second.c_str(), is_selected))
							{
								// ���C�L���X�g�Ώۂ��d�����Ȃ��ꍇ�̂ݓo�^
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
		{// �����n�`��o�^���悤�Ƃ����ꍇ�����𔲂���
			if (terrain == ent)
				return;
		}

		ent_terrains.emplace_back(ent);
	}

	bool CollisionManager::HasTerrain(const mapping::rename_type::Entity ent)
	{
		for (const auto& terrain : ent_terrains)
		{// �����n�`��o�^���悤�Ƃ����ꍇ�����𔲂���
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

		// PhysicMaterialComponent��nullptr�̏ꍇ�͔����W����0�ɐݒ�
		if (physic_material_comp_1)
			restitution_1 = physic_material_comp_1->GetBounciness();
		if (physic_material_comp_2)
			restitution_2 = physic_material_comp_2->GetBounciness();

		/*
		 * CombineSetting�����Ƃɔ����W�����Z�o
		 * �ڐG���Ă��� 2 �̃R���C�_�[�ɈقȂ� combine ���[�h���ݒ肳��Ă���Ƃ��͓��ʂōł������D��x�����@�\���g�p�����B
		 * Average < Minimum < Multiply < Maximum
		 * ��)
		 *  ����}�e���A���� Average ���[�h�������A���� Maximum ���[�h�����Ƃ��A combine �@�\�� Maximum �Ŏg�p�����B
		 */

		// �ǂ����PhysicMaterialComponent�����݂���ꍇ
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
		// physic_material_comp_1�̂ݑ��݂���ꍇ
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
		// physic_material_comp_2�̂ݑ��݂���ꍇ
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

		// �ǂ����PhysicMaterialComponent��nullptr�̏ꍇ
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
		//// �����̃��W�X�g��
		//result_1.registry = registry_2;
		//result_2.registry = registry_1;
		//// �����̃R���W�����^�O
		//result_1.collision_tag = sphere_2.collision_tag;
		//result_2.collision_tag = sphere_1.collision_tag;
		//// �����̃R���W����ID
		//result_1.collision_id = sphere_2.id;
		//result_2.collision_id = sphere_1.id;
		//// �����̂̃G���e�B�e�B
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
		//// �����̃��W�X�g��
		//result_1.registry = registry_2;
		//result_2.registry = registry_1;
		//// �����̃R���W�����^�O
		//result_1.collision_tag = capsule_2.collision_tag;
		//result_2.collision_tag = capsule_1.collision_tag;
		//// �����̃R���W����ID
		//result_1.collision_id = capsule_2.id;
		//result_2.collision_id = capsule_1.id;
		//// �����̂̃G���e�B�e�B
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
		//// �����̃��W�X�g��
		//s_result.registry = registry_2;
		//c_result.registry = registry_1;
		//// �����̃R���W�����^�O
		//s_result.collision_tag = capsule.collision_tag;
		//c_result.collision_tag = sphere.collision_tag;
		//// �����̃R���W����ID
		//s_result.collision_id = capsule.id;
		//c_result.collision_id = sphere.id;
		//// �����̂̃G���e�B�e�B
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
		// ���� 1 : ���݂�RigidBodyComponent�������Ă��邱��
		// ���� 2 : ���݂���CollisionPreset��BlockAll�ł��邱��
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

		// �ՓˑO�̑��x
		const float pre_v1 = rigid_body_comp_1.GetVelocity().Dot(n);
		const float pre_v2 = rigid_body_comp_2.GetVelocity().Dot(n);
		if(pre_v1 - pre_v2 > 0)
		{
			// �����W���̎擾
			const float restitution = CalculateRestitution(registry->TryGetComponent<component::PhysicMaterialComponent>(ent_1),
														   registry->TryGetComponent<component::PhysicMaterialComponent>(ent_2));

			// �Փˌ�̑��x
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
		// ���� 1 : ���݂�RigidBodyComponent�������Ă��邱��
		// ���� 2 : ���݂���CollisionPreset��BlockAll�ł��邱��
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

		// �ՓˑO�̑��x
		const float pre_v1 = rigid_body_comp_1.GetVelocity().Dot(n);
		const float pre_v2 = rigid_body_comp_2.GetVelocity().Dot(n);
		if (pre_v1 - pre_v2 > 0)
		{
			// �����W���̎擾
			const float restitution = CalculateRestitution(registry_1->TryGetComponent<component::PhysicMaterialComponent>(ent_1),
														   registry_2->TryGetComponent<component::PhysicMaterialComponent>(ent_2));

			// �Փˌ�̑��x
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

		// ent_terrains��RayCastComponent�������f���Ń��C�L���X�g�̔�����s��
		for (const auto& ent_terrain : ent_terrains)
		{
			if (!registry->GetEntities().contains(ent_terrain))
				continue;
			const component::ModelComponent& model_comp = registry->GetComponent<component::ModelComponent>(ent_terrain);

			asset::Model& model = locator::Locator::GetAssetManager()->GetLoader<asset::ModelLoader>()->GetModel(model_comp.GetModelID());
			for (const asset::ModelData::Mesh& mesh : model.GetModelData().GetMeshes())
			{
				const component::ModelComponent::Node node = model_comp.GetNodes().at(mesh.node_index);

				// ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
				DirectX::XMMATRIX world_transform		  = DirectX::XMLoadFloat4x4(&node.world_transform);
				DirectX::XMMATRIX inverse_world_transform = DirectX::XMMatrixInverse(nullptr, world_transform);

				DirectX::XMVECTOR start  = DirectX::XMVector3TransformCoord(world_start, inverse_world_transform);
				DirectX::XMVECTOR end	 = DirectX::XMVector3TransformCoord(world_end, inverse_world_transform);
				DirectX::XMVECTOR vec	 = DirectX::XMVectorSubtract(end, start);
				DirectX::XMVECTOR dir	 = DirectX::XMVector3Normalize(vec);
				DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);

				// ���C�̒���
				float neart{};
				DirectX::XMStoreFloat(&neart, length);

				// �O�p�`(��)�Ƃ̌�������
				const std::vector<asset::ModelData::Vertex>& vertices = mesh.vertices;
				const std::vector<UINT> indices = mesh.indices;

				for (const asset::ModelData::Subset& subset : mesh.subsets)
				{
					for (UINT i = 0; i < subset.index_count; i += 3)
					{
						UINT index = subset.start_index + i;
						// �O�p�`�̒��_�𒊏o
						const asset::ModelData::Vertex& a = vertices.at(indices.at(index));
						const asset::ModelData::Vertex& b = vertices.at(indices.at(index + 1));
						const asset::ModelData::Vertex& c = vertices.at(indices.at(index + 2));

						DirectX::XMVECTOR A = DirectX::XMLoadFloat4(&a.position);
						DirectX::XMVECTOR B = DirectX::XMLoadFloat4(&b.position);
						DirectX::XMVECTOR C = DirectX::XMLoadFloat4(&c.position);

						// �O�p�`�̎O�Ӄx�N�g�����Z�o
						DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
						DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
						DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

						//�O�p�`�̖@���x�N�g�����Z�o
						DirectX::XMVECTOR normal = DirectX::XMVector3Cross(AB, BC);

						// ���ς̌��ʂ��v���X�Ȃ�Η�����
						DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(dir, normal);
						float dot;
						DirectX::XMStoreFloat(&dot, Dot);
						if (dot >= 0)continue;

						// ���C�Ƃ̕��ʂ̌�_���Z�o
						DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, start);
						DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(normal, V), Dot);
						float t;
						DirectX::XMStoreFloat(&t, T);
						// ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������傫�����̓X�L�b�v
						if (t<0.0f || t>neart)continue;

						DirectX::XMVECTOR position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, T), start);

						// ��_���O�p�`�̓����ɂ��邩����
						// �P��
						DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, position);
						DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
						DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, normal);
						float dot1;
						DirectX::XMStoreFloat(&dot1, Dot1);
						if (dot1 < 0.0f)continue;

						// �Q��
						DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, position);
						DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
						DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, normal);
						float dot2;
						DirectX::XMStoreFloat(&dot2, Dot2);
						if (dot2 < 0.0f)continue;

						// �R��
						DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, position);
						DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
						DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, normal);
						float dot3;
						DirectX::XMStoreFloat(&dot3, Dot3);
						if (dot3 < 0.0f)continue;

						// �ŋߋ������X�V
						neart = t;

						// ��_�Ɩ@�����X�V
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

		//	// ���[���h��Ԃ̃��C�̒���
		//	DirectX::XMStoreFloat(&ray.second.hit_result.distance, WorldRayLength);

		//	for (const ModelData::Mesh& mesh : resource->GetModelData().GetMeshes())
		//	{
		//		const component::FbxModelComponent::Node node = model.GetNodes().at(mesh.node_index);

		//		// ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
		//		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.world_transform);
		//		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		//		DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		//		DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		//		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
		//		DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
		//		DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

		//		// ���C�̒���
		//		float neart;
		//		DirectX::XMStoreFloat(&neart, Length);

		//		// �O�p�`(��)�Ƃ̌�������
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
		//				// �O�p�`�̒��_�𒊏o
		//				const ModelData::Vertex& a = vertices.at(indices.at(index));
		//				const ModelData::Vertex& b = vertices.at(indices.at(index + 1));
		//				const ModelData::Vertex& c = vertices.at(indices.at(index + 2));

		//				DirectX::XMVECTOR A = DirectX::XMLoadFloat4(&a.position);
		//				DirectX::XMVECTOR B = DirectX::XMLoadFloat4(&b.position);
		//				DirectX::XMVECTOR C = DirectX::XMLoadFloat4(&c.position);

		//				// �O�p�`�̎O�Ӄx�N�g�����Z�o
		//				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
		//				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
		//				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

		//				//�O�p�`�̖@���x�N�g�����Z�o
		//				DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);

		//				// ���ς̌��ʂ��v���X�Ȃ�Η�����
		//				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
		//				float dot;
		//				DirectX::XMStoreFloat(&dot, Dot);
		//				if (dot >= 0)continue;

		//				// ���C�Ƃ̕��ʂ̌�_���Z�o
		//				DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, Start);
		//				DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
		//				float t;
		//				DirectX::XMStoreFloat(&t, T);
		//				// ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������傫�����̓X�L�b�v
		//				if (t<0.0f || t>neart)continue;

		//				DirectX::XMVECTOR Position = Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

		//				// ��_���O�p�`�̓����ɂ��邩����
		//				// �P��
		//				DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, Position);
		//				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
		//				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal);
		//				float dot1;
		//				DirectX::XMStoreFloat(&dot1, Dot1);
		//				if (dot1 < 0.0f)continue;

		//				// �Q��
		//				DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, Position);
		//				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
		//				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal);
		//				float dot2;
		//				DirectX::XMStoreFloat(&dot2, Dot2);
		//				if (dot2 < 0.0f)continue;

		//				// �R��
		//				DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position);
		//				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
		//				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal);
		//				float dot3;
		//				DirectX::XMStoreFloat(&dot3, Dot3);
		//				if (dot3 < 0.0f)continue;

		//				// �ŋߋ������X�V
		//				neart = t;

		//				// ��_�Ɩ@�����X�V
		//				HitPosition		= Position;
		//				HitNormal		= Normal;
		//				materialIndex	= subset.material_index;
		//				break;
		//			}
		//		}
		//		if (materialIndex >= 0)
		//		{
		//			// ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
		//			DirectX::XMVECTOR WorldPosition		= DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
		//			DirectX::XMVECTOR WorldCrossVec		= DirectX::XMVectorSubtract(WorldPosition, WorldStart);
		//			DirectX::XMVECTOR WorldCrossLength	= DirectX::XMVector3Length(WorldCrossVec);
		//			float distance;
		//			DirectX::XMStoreFloat(&distance, WorldCrossLength);
		//			utility::MaterialDiscrimination material_discrimination{};

		//			// �q�b�g���ۑ�
		//			if (ray.second.hit_result.distance > distance)
		//			{
		//				DirectX::XMVECTOR WorldNormal			= DirectX::XMVector3Transform(HitNormal, WorldTransform);
		//				ray.second.hit_result.distance			= distance;
		//				ray.second.hit_result.material_index	= materialIndex;
		//				ray.second.hit_result.is_hit			= true;
		//				DirectX::XMStoreFloat3(&ray.second.hit_result.position, WorldPosition);
		//				DirectX::XMStoreFloat3(&ray.second.hit_result.normal, DirectX::XMVector3Normalize(WorldNormal));
		//				ray_cast_comp.SetTerrainAttribute(ray.first,material_discrimination.GetTerrainAttribute(model.GetResource()->GetModelData().GetMaterials().at(materialIndex).texture_name)); // �}�e���A���̃e�N�X�`��������}�e���A���̑�������
		//				hit = true;
		//			}

		//			{// ���o������
		//				// ���o���������s��Ȃ��ꍇ�͏����𔲂���(���̃��C�Ƃ̏����Ɉڂ�)
		//				if(!ray.second.is_block)
		//					break;

		//				//-- ���o�����@ --//
		//				/*
		//				 * Ray�̃u���b�N�ʒu����n�`�̈ʒu�܂ł̃x�N�g�����쐬��ray�x�N�g��
		//				 * �ƕ������t(�p�x��90�x�ȏ�)�Ȃ牟���o���������s��
		//				 */

		//				// Ray�̎n�_����n�`�̈ʒu�܂ł̃x�N�g��
		//				const DirectX::SimpleMath::Vector3 v1{ ray.second.hit_result.position - ray.second.block_pos };
		//				DirectX::SimpleMath::Vector3 v1_n = v1;
		//				v1_n.Normalize();
		//				// Ray�x�N�g��
		//				const DirectX::SimpleMath::Vector3 v2{ ray.second.ray_end - ray.second.ray_start };
		//				DirectX::SimpleMath::Vector3 v2_n = v2;
		//				v2_n.Normalize();
		//				float dot = v1.Dot(v2);
		//				// �x�N�g���Ԃ̊p�x��90�x�����Ȃ珈���𔲂���(���̃��C�Ƃ̏����Ɉڂ�)
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
					// �q�b�g���̍X�V
					UpdateHitResult(sphere_1.GetRegistry(),sphere_2.GetRegistry(),
									s1, s2,
									sphere_1.GetEntity(), sphere_2.GetEntity());

					// ���o������
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
					// �q�b�g���̍X�V
					UpdateHitResult(sphere_1.GetRegistry(),sphere_2.GetRegistry(),
									s1, s2,
									sphere_1.GetEntity(), sphere_2.GetEntity());

					// ���o������
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
				// ���ꂼ��̃J�v�Z���̐���(�n�_)����̑傫��
				float c1_t, c2_t;
				DirectX::SimpleMath::Vector3 c1_p, c2_p;

				// �����̍ŋߋ���
				const float len = arithmetic::ClosestPtSegmentSegment(
									c1.start, c1.end,
									c2.start, c2.end,
									c1_t, c2_t,
									c1_p, c2_p);

				if(len <c1.radius + c2.radius)
				{
					AddHitCapsule(c1.id, HitData{ capsule_1.GetEntity(),capsule_2.GetEntity() });
					AddHitCapsule(c2.id, HitData{ capsule_2.GetEntity(),capsule_1.GetEntity() });
					// �q�b�g���̍X�V
					UpdateHitResult(capsule_1.GetRegistry(),capsule_2.GetRegistry(),
									c1, c2,
									capsule_1.GetEntity(), capsule_2.GetEntity());
					hit = true;

					// ���o������
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
				// ���ꂼ��̃J�v�Z���̐���(�n�_)����̑傫��
				float c1_t, c2_t;
				DirectX::SimpleMath::Vector3 c1_p, c2_p;

				// �����̍ŋߋ���
				const float len = arithmetic::ClosestPtSegmentSegment(
					c1.start, c1.end,
					c2.start, c2.end,
					c1_t, c2_t,
					c1_p, c2_p);

				if (len < c1.radius + c2.radius)
				{
					AddHitCapsule(c1.id, HitData{ capsule_1.GetEntity(),capsule_2.GetEntity() });
					AddHitCapsule(c2.id, HitData{ capsule_2.GetEntity(),capsule_1.GetEntity() });
					// �q�b�g���̍X�V
					UpdateHitResult(capsule_1.GetRegistry(),capsule_2.GetRegistry(),
									c1, c2,
									capsule_1.GetEntity(), capsule_2.GetEntity());
					hit = true;

					// ���o������
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

		// �����G���e�B�e�B���m�Ȃ画�菈�����s��Ȃ�
		if (sphere.GetEntity() == capsule.GetEntity())
			return hit;

		for (auto& s : sphere.GetSpheres() | std::views::values)
		{
			for (auto& c : capsule.GetCapsules() | std::views::values)
			{
				// ���̒��S�ƃJ�v�Z���̐����̊Ԃ�(��������)�����̎Z�o
				const float dist = arithmetic::SqDistPointSegment(c.start, c.end, s.world_transform_matrix.Translation());
				if(const float radius = s.radius + c.radius;
				   dist <= radius * radius)
				{
					AddHitSphere(s.id, sphere.GetEntity(), capsule.GetEntity());
					AddHitCapsule(c.id, HitData{ capsule.GetEntity(),sphere.GetEntity() });
					// �q�b�g���̍X�V
					UpdateHitResult(sphere.GetRegistry(),capsule.GetRegistry(),
									s, c,
									sphere.GetEntity(), capsule.GetEntity());
					hit = true;
					float t = 0; //	�ŋߐړ_�܂ł̋���(�����̊���)
					DirectX::SimpleMath::Vector3 p{}; // �J�v�Z���̍ŋߐړ_
					arithmetic::ClosestPtPointSegment(c.start, c.end, s.world_transform_matrix.Translation(), p, t);
					// ���o������
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

		// �����G���e�B�e�B���m�Ȃ画�菈�����s��Ȃ�
		if (sphere.GetEntity() == capsule.GetEntity())
			return hit;

		for (auto& s : sphere.GetSpheres() | std::views::values)
		{
			for (auto& c : capsule.GetCapsules() | std::views::values)
			{
				// ���̒��S�ƃJ�v�Z���̐����̊Ԃ�(��������)�����̎Z�o
				const float dist = arithmetic::SqDistPointSegment(c.start, c.end, s.world_transform_matrix.Translation());
				// (��������)������(��������)���a�̑��a�����������ꍇ�͏Փ�
				if (const float radius = s.radius + c.radius;
					dist <= radius * radius)
				{
					AddHitSphere(s.id, sphere.GetEntity(), capsule.GetEntity());
					AddHitCapsule(c.id, HitData{ capsule.GetEntity(),sphere.GetEntity() });
					// �q�b�g���̍X�V
					UpdateHitResult(sphere.GetRegistry(),capsule.GetRegistry(),
									s, c,
									sphere.GetEntity(), capsule.GetEntity());
					hit = true;
					float t = 0; //	�ŋߐړ_�܂ł̋���(�����̊���)
					DirectX::SimpleMath::Vector3 p{}; // �J�v�Z���̍ŋߐړ_
					arithmetic::ClosestPtPointSegment(c.start, c.end, s.world_transform_matrix.Translation(), p, t);
					// ���o������
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
