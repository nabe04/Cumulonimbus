#include "prefab.h"

#include "ecs.h"
#include "file_path_helper.h"
#include "generic.h"
#include "file_path_helper.h"
// components(engine)
#include "actor3d_component.h"
#include "billboard_component.h"
#include "camera_component.h"
#include "capsule_collison_component.h"
#include "model_component.h"
#include "physic_material_component.h"
#include "raycast_component.h"
#include "rigid_body_component.h"
#include "sky_box.h"
#include "sphere_collision_component.h"
#include "sprite_component.h"
#include "transform_component.h"
// components(game)
#include "enemy_base_component.h"
#include "enemy_soldier_component.h"
#include "player_component.h"

namespace cumulonimbus::asset
{
	Prefab::Prefab()
	{
		//-- engine --//
		// transform
		RegistryComponent<component::TransformComponent>();
		// actor
		RegistryComponent<component::Actor3DComponent>();
		// model
		RegistryComponent<component::ModelComponent>();
		//// sprite
		//RegistryComponent<component::SpriteComponent>();
		//RegistryComponent<component::BillboardComponent>();
		//// physics
		//RegistryComponent<component::SphereCollisionComponent>();
		//RegistryComponent<component::CollisionComponent>();
		RegistryComponent<component::RayCastComponent>();
		RegistryComponent<component::CapsuleCollisionComponent>();
		RegistryComponent<component::PhysicMaterialComponent>();
		//RegistryComponent<component::RigidBodyComponent>();
		// camera
		RegistryComponent<component::CameraComponent>();
		//// sky box
		//RegistryComponent<component::SkyBoxComponent>();
		////-- game --//
		//// player
		//RegistryComponent<component::PlayerComponent>();
		//// enemy
		//RegistryComponent<component::EnemyBaseComponent>();
		//RegistryComponent<component::EnemySoldierComponent>();
	}

	void Prefab::CreatePrefab(
		ecs::Registry* registry, const mapping::rename_type::Entity& ent,
		const std::filesystem::path& path)
	{
		for(auto& [comp_name, comp_asset] : component_assets)
		{
			if(comp_asset->RegistryPrefab(registry, ent))
			{
				components_name.emplace(comp_name);
			}
		}
		// プレファブの保存(シリアライズ)
		Save(path);
	}

	void Prefab::AddComponent(ecs::Registry* registry, const mapping::rename_type::Entity& ent)
	{
		for (auto& [comp_name, comp_asset] : component_assets)
		{
			if(components_name.contains(comp_name))
			{
				component_assets.at(comp_name)->AddComponent(registry, ent);
			}
		}
	}

	void Prefab::Save(const std::filesystem::path& path)
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

		for(auto& comp : component_assets)
		{// コンポーネントのデータをファイルに保存(シリアライズ)
			comp.second->Save(path.parent_path().string());
		}
	}

	void Prefab::Load(const std::filesystem::path& path)
	{
		if (path.extension().compare(file_path_helper::GetPrefabExtension()) != 0)
			assert(!"The file extension is not 「.prefab」");

		// パラメータのリセット
		if (!component_assets.empty())
			component_assets.clear();
		if (!components_name.empty())
			components_name.clear();

		{// プレファブの読み込み
			std::ifstream ifs(path);
			if (!ifs)
				assert("!Not open file(Prefab::Load)");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(*this);
		}

		for (auto& comp : component_assets)
		{// コンポーネントのデータをファイルから読み込み(デシリアライズ)
			comp.second->Load(path.parent_path().string());
		}
	}

	template <class T>
	void Prefab::RegistryComponent()
	{
		const mapping::rename_type::ComponentName component_name = file_path_helper::GetTypeName<T>();

		if (component_assets.contains(component_name))
			return;

		component_assets.emplace(component_name, std::make_shared<ecs::ComponentAsset<T>>());
	}

} // cumulonimbus::asset
