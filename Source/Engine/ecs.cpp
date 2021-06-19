#include "ecs.h"

#include <filesystem>
#include <fstream>

#include <imgui.h>

#include "transform_component.h"
#include "actor_component.h"
#include "child_actor.h"
#include "sprite.h"
#include "sprite_object.h"
#include "anim_sprite.h"
#include "camera_operation.h"
#include "mesh_object.h"
#include "collision_component.h"
#include "sphere_collision_component.h"
#include "fbx_model_component.h"
#include "geometric_primitive_component.h"
#include "obj_model_component.h"
#include "sky_box.h"
#include "scene.h"

namespace cumulonimbus::ecs
{
	Entity Registry::CreateEntity()
	{
		uint64_t entity = START_ID;

		// 後置インクリメントをしているのはSTART_IDを初めの要素にセットしたいから
		while (entities.contains(static_cast<Entity>(entity)))
		{
			entity++;
		}

		CreateEntity(static_cast<Entity>(entity));
		AddComponent<component::TransformComponent>(static_cast<Entity>(entity));
		return entities.at(static_cast<Entity>(entity)).first;
	}

	void Registry::CreateEntity(ecs::Entity ent)
	{
		std::string ent_name = "Entity";

		if (static_cast<uint64_t>(ent) == 0)
		{
			entities.emplace(ent, std::make_pair(ent, ent_name));
			return;
		}
		else
		{
			const int no = static_cast<int>(ent);
			ent_name += "(" + std::to_string(no) + ")";
			entities.emplace(ent, std::make_pair(ent, ent_name));
			return;
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
		RegistryComponent<component::ActorComponent>();
		RegistryComponent<component::ChildActorComponent>();
		RegistryComponent<component::TransformComponent>();
		RegistryComponent<component::SpriteComponent>();
		RegistryComponent<component::AnimSpriteComponent>();
		RegistryComponent<component::SpriteObjectComponent>();
		RegistryComponent<component::MeshObjectComponent>();
		RegistryComponent<component::CollisionComponent>();
		RegistryComponent<component::SphereCollisionComponent>();
		RegistryComponent<component::FbxModelComponent>();
		RegistryComponent<component::GeomPrimComponent>();
		RegistryComponent<component::ObjModelComponent>();
		RegistryComponent<component::SkyBoxComponent>();
	}

	void Registry::Save(const std::string& filename)
	{
		// ファイルを作成
			// ./Contents/「ファイル名」までの取得(拡張子はなし)
		const std::string save_file_path = file_path_helper::AttachSceneDirectory(filename);
		std::filesystem::create_directories(save_file_path);
		// 保存する先のファイルを指定
		const std::string save_file_path_and_name{ save_file_path + "/" + filename };

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

		for (auto&& [component_name, component_array] : component_arrays)
		{
			component_array->Save(filename);
		}
	}

	void Registry::Load(const std::string& filename)
	{
		// entitiesの削除
		if (!entities.empty())
		{
			DestroyAllEntities();
		}
		component_arrays.clear();


		{
			// ロードする先のファイルを指定
			// ./Contents/Scenes/「ファイル名」/「ファイル名」.bin
			std::ifstream ifs(file_path_helper::AttachSceneDirectory(filename) + "/" + filename + file_path_helper::GetBinExtension(), std::ios_base::binary);
			if (!ifs)
				assert(!"Not open file");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(
				CEREAL_NVP(entities)
			);
		}

		// component_arraysの型の再設定
		RegisterComponentName();
		// component_arrayのロード
		for (auto& it : component_arrays)
		{
			it.second->Load(filename);
		}
	}

}




