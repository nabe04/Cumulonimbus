#include "ecs.h"

#include <filesystem>
#include <fstream>

#include "generic.h"
// components(engine)
#include "actor3d_component.h"
#include "actor_component.h"
#include "anim_sprite.h"
#include "camera_component.h"
#include "child_actor.h"
#include "fbx_model_component.h"
#include "material_component.h"
#include "mesh_object.h"
#include "obj_model_component.h"
#include "raycast_component.h"
#include "scene.h"
#include "sky_box.h"
#include "sprite.h"
#include "sprite_object.h"
#include "transform_component.h"
#include "model_component.h"
// components(game)
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

	void Registry::PreUpdate(float dt)
	{
		for(auto& component : component_arrays)
		{
			component.second->PreUpdate(dt);
		}
	}

	void Registry::Update(float dt)
	{
		for(auto& component : component_arrays)
		{
			component.second->Update(dt);
		}
	}

	void Registry::PostUpdate(float dt)
	{
		for (auto& component : component_arrays)
		{
			component.second->PostUpdate(dt);
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
		//// engine
		//RegistryComponent<component::ActorComponent>();
		//RegistryComponent<component::ChildActorComponent>();
		RegistryComponent<component::TransformComponent>();
		//RegistryComponent<component::SpriteComponent>();
		//RegistryComponent<component::AnimSpriteComponent>();
		//RegistryComponent<component::SpriteObjectComponent>();
		RegistryComponent<component::MeshObjectComponent>();
		RegistryComponent<component::FbxModelComponent>();
		RegistryComponent<component::ModelComponent>();
		//RegistryComponent<component::GeomPrimComponent>();
		//RegistryComponent<component::ObjModelComponent>();
		RegistryComponent<component::SkyBoxComponent>();
		RegistryComponent<component::CameraComponent>();
		//RegistryComponent<component::MaterialComponent>();
		// game
		//RegistryComponent<component::PlayerComponent>();
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
		component_arrays.clear();

		const std::string n = file_path + "/" +
			file_path_helper::GetEntitiesFilename() + "/" +
			file_path_helper::GetEntitiesFilename() + "/" +
			file_path_helper::GetBinExtension();

		{// エンティティの読み込み
			// ./file_path/「Entities」.bin
			std::ifstream ifs(
				file_path + "/" +
				file_path_helper::GetEntitiesFilename() + "/" +
				file_path_helper::GetEntitiesFilename() +
				file_path_helper::GetBinExtension()
				, std::ios_base::binary);
			if (!ifs)
				assert(!"Not open file");
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




