#include "prefab.h"

#include <ranges>

#include "prefab.h"
#include "ecs.h"
#include "file_path_helper.h"
#include "file_path_helper.h"
#include "generic.h"
//-- components(engine) --//
// actor
#include "actor3d_component.h"
#include "hierarchy_component.h"
// camera
#include "camera_component.h"
// collider
#include "capsule_collison_component.h"
#include "collider_message_receiver_component.h"
#include "collider_message_sender_component.h"
#include "collision_component.h"
#include "raycast_component.h"
#include "sphere_collision_component.h"
// damage
#include "damageable_component.h"
// effekseer
#include "effekseer_component.h"
// light
#include "directiona_light_component.h"
#include "light_component.h"
#include "point_light_component.h"
#include "spot_light_component.h"
// model
#include "model_component.h"
// physics
#include "physic_material_component.h"
#include "rigid_body_component.h"
// sprite
#include "billboard_component.h"
#include "sprite_component.h"
// transform
#include "transform_component.h"

//-- components(game) --//
// enemy
#include "enemy_base_component.h"
#include "enemy_boss_component.h"
#include "enemy_soldier_component.h"
// player
#include "player_component.h"
// weapon
#include "energy_shot_component.h"
#include "player_sword_component.h"
#include "soldier_sword_component.h"



namespace cumulonimbus::asset
{
	Prefab::EntityInfo::EntityInfo()
	{
		//-- engine --//
		// actor
		RegistryComponent<component::Actor3DComponent>();
		RegistryComponent<component::HierarchyComponent>();
		// camera
		RegistryComponent<component::CameraComponent>();
		// collider
		RegistryComponent<component::CapsuleCollisionComponent>();
		RegistryComponent<component::ColliderMessageReceiverComponent>();
		RegistryComponent<component::ColliderMessageSenderComponent>();
		RegistryComponent<component::CollisionComponent>();
		RegistryComponent<component::RayCastComponent>();
		RegistryComponent<component::SphereCollisionComponent>();
		// damage
		RegistryComponent<component::DamageableComponent>();
		// effekseer
		RegistryComponent<component::EffekseerComponent>();
		// lights
		RegistryComponent<component::DirectionalLightComponent>();
		RegistryComponent<component::LightComponent>();
		RegistryComponent<component::PointLightComponent>();
		RegistryComponent<component::SpotLightComponent>();
		// model
		RegistryComponent<component::ModelComponent>();
		// physics
		RegistryComponent<component::PhysicMaterialComponent>();
		RegistryComponent<component::RigidBodyComponent>();
		// sprite
		RegistryComponent<component::BillboardComponent>();
		RegistryComponent<component::SpriteComponent>();
		// transform
		RegistryComponent<component::TransformComponent>();

		//-- game --//
		// enemy
		RegistryComponent<component::EnemyBaseComponent>();
		RegistryComponent<component::EnemyBossComponent>();
		RegistryComponent<component::EnemySoldierComponent>();
		// player
		RegistryComponent<component::PlayerComponent>();
		// weapon
		RegistryComponent<component::EnergyShotComponent>();
		RegistryComponent<component::PlayerSwordComponent>();
		RegistryComponent<component::SoldierSwordComponent>();
	}

	template <class T>
	void Prefab::EntityInfo::RegistryComponent()
	{
		const mapping::rename_type::ComponentName component_name = file_path_helper::GetTypeName<T>();

		if (component_assets.contains(component_name))
			return;

		component_assets.emplace(component_name, std::make_shared<ecs::ComponentAsset<T>>());
	}

	Prefab::Prefab()
	{

	}

	void Prefab::CreatePrefab(
		ecs::Registry* const registry,
		const std::vector<mapping::rename_type::Entity>& entities,
		const std::filesystem::path& path)
	{
		for(const auto& ent : entities)
		{
			entity_assets.emplace(ent, EntityInfo{});
			for (auto& [comp_name, comp_asset] : entity_assets.at(ent).component_assets)
			{
				entity_assets.at(ent).entity_name = registry->GetName(ent);
				if (comp_asset->RegistryComponentData(registry, ent))
					entity_assets.at(ent).components_name.emplace(comp_name);
			}
		}
		// プレファブの保存(シリアライズ)
		Save(registry, path);
	}

	mapping::rename_type::Entity Prefab::Instantiate(ecs::Registry* const registry)
	{
		mapping::rename_type::Entity return_ent{};

		// シーンにドロップしてオブジェクトを作成した際に以前の
		// エンティティと作成後のエンティティを繋げるためのコネクター
		std::map<mapping::rename_type::Entity, mapping::rename_type::Entity> connector{};
		for (const auto& ent : entity_assets | std::views::keys)
		{
			connector.emplace(ent, registry->CreateEntity());
		}

		for (auto& [ent, comp_asset] : entity_assets)
		{
			for (auto& [comp_name, comp] : comp_asset.component_assets)
			{
				if (!comp_asset.components_name.contains(comp_name))
					continue;

				comp->AddComponent(registry, connector.at(ent));
				comp->OnDeserialize(registry, connector.at(ent), connector);
			}

			if(registry->HasComponent<component::HierarchyComponent>(connector.at(ent)))
			{
				auto& hierarchy_comp = registry->GetComponent<component::HierarchyComponent>(connector.at(ent));
				if(const auto& old_parent_ent = hierarchy_comp.GetParentEntity();
					old_parent_ent.empty())
				{// 一番上の親階層を返す(SceneView上に出した時の位置取得に使用)
					return_ent = connector.at(ent);
				}
			}
		}

		return return_ent;
	}

	void Prefab::Save(ecs::Registry* const registry, const std::filesystem::path& path)
	{
		if (path.extension().compare(file_path_helper::GetPrefabExtension()) != 0)
			assert(!"The file extension is not 「.prefab」");
		std::filesystem::create_directory(path.parent_path());
		{// 「.prefab」拡張子のファイルの保存
			std::ofstream ofs(path, std::ios_base::binary);
			if (!ofs)
				assert(!"Not open file(Prefab::Save)");
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
		for (auto& [ent, comp_asset] : entity_assets)
		{
			for (auto& [comp_name, comp] : comp_asset.component_assets)
			{
				comp->Save(path.parent_path().string() + "/" + registry->GetName(ent));
			}
		}
	}

	void Prefab::Load(const std::filesystem::path& path)
	{
		if (path.extension().compare(file_path_helper::GetPrefabExtension()) != 0)
			assert(!"The file extension is not 「.prefab」");
			//return;

		// パラメータのリセット
		if (!entity_assets.empty())
			entity_assets.clear();


		{// プレファブの読み込み
			std::ifstream ifs(path);
			if (!ifs)
				assert(!"Not open file(Prefab::Load)");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(*this);
		}

		const std::string load_path = std::filesystem::path{ path }.parent_path().string();
		for (auto& [ent, comp_asset] : entity_assets)
		{
			for (auto& [comp_name, comp] : comp_asset.component_assets)
			{
				comp->Load(load_path + "/" + comp_asset.entity_name);
			}
		}
	}
} // cumulonimbus::asset
