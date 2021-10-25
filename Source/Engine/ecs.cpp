#include "ecs.h"

#include <filesystem>
#include <fstream>

#include "generic.h"
#include "scene.h"
// components(engine)
#include "actor3d_component.h"
#include "billboard_component.h"
#include "camera_component.h"
#include "capsule_collison_component.h"
#include "hierarchy_component.h"
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

namespace cumulonimbus::ecs
{
	using namespace mapping::rename_type;

	template <typename T>
	template <typename Archive>
	void ComponentArray<T>::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(components),
			CEREAL_NVP(entity_id)
		);
	}

	Entity Registry::CreateEntity()
	{
		//uint64_t entity = START_ID;

		//// 後置インクリメントをしているのはSTART_IDを初めの要素にセットしたいから
		//while (entities.contains(static_cast<Entity>(entity)))
		//{
		//	entity++;
		//}

		//CreateEntity(static_cast<Entity>(entity));
		//AddComponent<component::TransformComponent>(static_cast<Entity>(entity));
		//return entities.at(static_cast<Entity>(entity)).first;

		auto id = utility::GenerateUUID();
		// 後置インクリメントをしているのはSTART_IDを初めの要素にセットしたいから
		while (entities.contains(id))
		{
			id = utility::GenerateUUID();
		}

		CreateEntity(id);
		AddComponent<component::TransformComponent>(id);
		AddComponent<component::HierarchyComponent>(id);
		return entities.at(id).first;
	}

	void Registry::CreateEntity(ecs::Entity ent)
	{
		//std::string ent_name = "Entity";

		//if (static_cast<uint64_t>(ent) == 0)
		//{
		//	entities.emplace(ent, std::make_pair(ent, ent_name));
		//}
		//else
		//{
		//	const int no = static_cast<int>(ent);
		//	ent_name += "(" + std::to_string(no) + ")";
		//	entities.emplace(ent, std::make_pair(ent, ent_name));
		//}

		std::string ent_name = "Entity";

		int no = 0;
		while(true)
		{
			if(HasName(ent_name))
			{
				ent_name = "Entity";
				ent_name += "(" + std::to_string(++no) + ")";
			}
			else
			{
				entities.emplace(ent, std::make_pair(ent, ent_name));
				break;
			}
		}
	}

	void Registry::PreCommonUpdate(const float dt)
	{
		const auto& num_ent = entities.size();
		for (auto& component : component_arrays)
		{
			component.second->PreCommonUpdate(dt);
		}
	}

	void Registry::CommonUpdate(const float dt)
	{
		for (auto& component : component_arrays)
		{
			component.second->CommonUpdate(dt);
		}
	}

	void Registry::PostCommonUpdate(const float dt)
	{
		for (auto& component : component_arrays)
		{
			component.second->PostCommonUpdate(dt);
		}
	}

	void Registry::PreSceneUpdate(const float dt)
	{
		for (auto& component : component_arrays)
		{
			component.second->PreSceneUpdate(dt);
		}
	}

	void Registry::SceneUpdate(const float dt)
	{
		for (auto& component : component_arrays)
		{
			component.second->SceneUpdate(dt);
		}
	}

	void Registry::PostSceneUpdate(const float dt)
	{
		for (auto& component : component_arrays)
		{
			component.second->PostSceneUpdate(dt);
		}
	}

	void Registry::PreGameUpdate(const float dt)
	{
		for(auto& component : component_arrays)
		{
			component.second->PreGameUpdate(dt);
		}
	}

	void Registry::GameUpdate(const float dt)
	{
		for(auto& component : component_arrays)
		{
			component.second->GameUpdate(dt);
		}
	}

	void Registry::PostGameUpdate(const float dt)
	{
		for (auto& component : component_arrays)
		{
			component.second->PostGameUpdate(dt);
		}
	}

	void Registry::RenderImGui()
	{
		for (auto&& [ent_id, ent_name] : entities)
		{
			if (ImGui::CollapsingHeader(ent_name.second.c_str()))
			{
				for (auto&& [comp_name, comp_data] : component_arrays)
				{
					comp_data->RenderImGui(ent_id);
				}
			}
		}
	}

	/*
	 * brief     : component_arraysのキー値を予め登録
	 * ※caution : デシリアライズの際の型の判別に使用
	 */
	void Registry::RegisterComponentName()
	{
		//-- engine --//
		// transform
		RegistryComponent<component::TransformComponent>();
		// actor
		RegistryComponent<component::Actor3DComponent>();
		RegistryComponent<component::HierarchyComponent>();
		// model
		RegistryComponent<component::ModelComponent>();
		// sprite
		RegistryComponent<component::SpriteComponent>();
		RegistryComponent<component::BillboardComponent>();
		// physics
		RegistryComponent<component::SphereCollisionComponent>();
		RegistryComponent<component::CollisionComponent>();
		RegistryComponent<component::RayCastComponent>();
		RegistryComponent<component::CapsuleCollisionComponent>();
		RegistryComponent<component::PhysicMaterialComponent>();
		RegistryComponent<component::RigidBodyComponent>();
		// camera
		RegistryComponent<component::CameraComponent>();
		//-- game --//
		// player
		RegistryComponent<component::PlayerComponent>();
		// enemy
		RegistryComponent<component::EnemyBaseComponent>();
		RegistryComponent<component::EnemySoldierComponent>();
	}

	void Registry::Save(const std::string& filename, const std::string& scene_name)
	{
		// ./file_path/scene_name
		const std::string scene_dir = filename;
		// 「scene_name」フォルダの作成
		std::filesystem::create_directories(scene_dir);

		{
			// ./file_path/"Entities"
			const std::string dir_path = scene_dir + "/" + file_path_helper::GetEntitiesFilename();
			// 「Entities」フォルダの作成
			std::filesystem::create_directories(dir_path);
			// 保存する先のファイルを指定(./file_path/"Entities"/"Entities")
			const std::string save_file_path_and_name{
				dir_path + "/" +
				file_path_helper::GetEntitiesFilename()};

			{
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetJsonExtension());
				cereal::JSONOutputArchive output_archive(ofs);
				output_archive(*this);
			}

			{
				std::ofstream ofs(save_file_path_and_name + file_path_helper::GetBinExtension(), std::ios_base::binary);
				cereal::BinaryOutputArchive output_archive(ofs);
				output_archive(*this);
			}
		}

		{
			// ./file_path/"Components"
			const std::string dir_path = scene_dir + "/" + file_path_helper::GetComponentsFilename();
			// 「Components」フォルダの作成
			std::filesystem::create_directories(dir_path);
			for (auto&& [component_name, component_array] : component_arrays)
			{
				component_array->Save(dir_path);
			}
		}
	}

	void Registry::Load(const std::string& file_path)
	{
		// entitiesの削除
		if (!entities.empty())
		{
			DestroyAllEntities();
		}

		{// エンティティの読み込み
			// パス -> ./file_path/「Entities」.bin
			std::ifstream ifs(
				file_path + "/" +
				file_path_helper::GetEntitiesFilename() + "/" +
				file_path_helper::GetEntitiesFilename() +
				file_path_helper::GetBinExtension()
				, std::ios_base::binary);
			if (!ifs)
				assert(!"Not open file(Registry::Load)");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(*this);
		}

		// component_arraysの型の再設定
		RegisterComponentName();
		// component_arrayのロード
		for (auto& it : component_arrays)
		{
			it.second->Load(file_path, this);
		}
	}

	template <typename Archive>
	void Registry::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(entities)
		);
	}


} // cumulonimbus::ecs




