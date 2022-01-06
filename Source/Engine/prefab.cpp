#include "prefab.h"

#include <ranges>

#include "prefab.h"
#include "ecs.h"
#include "file_path_helper.h"
#include "file_path_helper.h"
#include "generic.h"
// components(engine)
#include "actor3d_component.h"
#include "billboard_component.h"
#include "camera_component.h"
#include "capsule_collison_component.h"
#include "damageable_component.h"
#include "directiona_light_component.h"
#include "hierarchy_component.h"
#include "light_component.h"
#include "model_component.h"
#include "physic_material_component.h"
#include "point_light_component.h"
#include "raycast_component.h"
#include "rigid_body_component.h"
#include "sphere_collision_component.h"
#include "spot_light_component.h"
#include "sprite_component.h"
#include "transform_component.h"
// components(game)
#include "enemy_base_component.h"
#include "enemy_soldier_component.h"
#include "enemy_boss_component.h"
#include "player_component.h"
#include "player_sword_component.h"
#include "soldier_sword_component.h"


namespace cumulonimbus::asset
{
	Prefab::EntityInfo::EntityInfo()
	{
		//-- engine --//
		// transform
		RegistryComponent<component::TransformComponent>();
		// actor
		RegistryComponent<component::Actor3DComponent>();
		RegistryComponent<component::HierarchyComponent>();
		// model
		RegistryComponent<component::ModelComponent>();
		//sprite
		RegistryComponent<component::SpriteComponent>();
		RegistryComponent<component::BillboardComponent>();
		// physics
		RegistryComponent<component::SphereCollisionComponent>();
		RegistryComponent<component::CollisionComponent>();
		RegistryComponent<component::RayCastComponent>();
		RegistryComponent<component::CapsuleCollisionComponent>();
		RegistryComponent<component::PhysicMaterialComponent>();
		RegistryComponent<component::RigidBodyComponent>();
		// lights
		RegistryComponent<component::LightComponent>();
		RegistryComponent<component::DirectionalLightComponent>();
		RegistryComponent<component::PointLightComponent>();
		RegistryComponent<component::SpotLightComponent>();
		// camera
		RegistryComponent<component::CameraComponent>();
		// damage
		RegistryComponent<component::DamageableComponent>();
		//-- game --//
		// player
		RegistryComponent<component::PlayerComponent>();
		// enemy
		RegistryComponent<component::EnemyBaseComponent>();
		RegistryComponent<component::EnemySoldierComponent>();
		RegistryComponent<component::EnemyBossComponent>();
		// weapon
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
