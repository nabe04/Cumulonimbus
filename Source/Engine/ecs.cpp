#include "ecs.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

#include "component_list.h"
#include "transform_component.h"
#include "scene.h"

//------------------------------------< Entity Class >-----------------------------
#pragma region
Entity::Entity(Scene* scene,const UpdateOrder update_order, const EntityTag entity_tag)
	: scene{ scene }
	, update_order{ update_order }
	, entity_tag{ entity_tag }
{
	AddComponent<TransformComponent>();
}

void Entity::NewFrame(const float delta_time)
{
	if (ApplyPaused())
		return;

	for (auto& comp : components)
	{
		comp->NewFrame(delta_time);
	}
}

void Entity::Update(const float delta_time)
{
	if (ApplyPaused())
		return;

	for (auto& comp : components)
	{
		comp->Update(delta_time);
	}
}

void Entity::RenderImgui()
{
	if (!ImGui::CollapsingHeader(entity_name.c_str()))
		return;

	for (auto& comp : components)
	{
		comp->RenderImGui();
	}
}

bool Entity::ApplyPaused()
{
	if (!is_apply_paused)
		return false;

	return scene->IsPaused();
}

void Entity::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + "/" + this->entity_name;

	std::filesystem::create_directories(file_path_and_name);

	{// Entity
		const std::string file_path_and_name_and_exe = file_path_and_name + ".json";
		std::ofstream ofs(file_path_and_name_and_exe);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(
			cereal::make_nvp("Entity Name"	  ,entity_name),
			cereal::make_nvp("Components Name",component_names),
			cereal::make_nvp("Update Order"	  ,update_order),
			cereal::make_nvp("Entity Tag"	  ,entity_tag)
		);
	}

	{// Component
		if (components.empty())
			return;

		const std::string component_file_path = file_path_and_name + "/Component/";
		std::filesystem::create_directories(component_file_path);

		for(const auto& comp : components)
		{
			comp->Save(component_file_path);
		}
	}
}

void Entity::Load(std::string file_path)
{
	for(const std::string comp_name : component_names)
	{
		Component* comp = component_list::Create(comp_name);
		if (!comp)
			continue;	// Component

		const std::string input_filename = file_path + "/" + entity_name + "/Component/" + comp_name + ".json";
		comp->Load(this, input_filename);

		components.emplace_back(comp);
	}
}
#pragma endregion Entity

//------------------------------------< Component Class >--------------------------
#pragma region


#pragma endregion  Component




