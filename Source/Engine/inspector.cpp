#include "inspector.h"

#include <cassert>

#include "component_base.h"
#include "ecs.h"
#include "fbx_model_resource.h"
#include "file_path_helper.h"
// components
#include "fbx_model_component.h"
#include "raycast_component.h"
#include "capsule_collison_component.h"
#include "physic_material_component.h"
#include "rigid_body_component.h"
#include "sphere_collision_component.h"

namespace cumulonimbus::editor
{
	Inspector::Inspector()
	{
		RegisterComponent<component::FbxModelComponent>(		"Model Component"	, mapping::component_tag::ComponentTag::Mesh);
		RegisterComponent<component::RayCastComponent>(			"RayCast Collider"	, mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::CapsuleCollisionComponent>("Capsule Collider"	, mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::PhysicMaterialComponent>(	"Physic Material"	, mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::SphereCollisionComponent>(	"Sphere Collider"	, mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::RigidBodyComponent>(		"RigidBody"			, mapping::component_tag::ComponentTag::Physics);
	}


	void Inspector::Render(ecs::Registry* registry, const mapping::rename_type::Entity ent)
	{
		ImGui::Begin("Inspector");

		// エンティティ名の変更
		std::string& name = registry->GetEntities().at(ent).second;
		const int arr_size = name.length();
		char buf[256];
		strcpy_s(buf, arr_size + 1, name.c_str());
		if(ImGui::InputText("##", buf,64, ImGuiInputTextFlags_CharsNoBlank))
		{
			name = buf;
		}

		// エンティティの持つコンポーネントの詳細
		for(auto& [comp_name,comp_data] : registry->GetComponentArrays())
		{
			comp_data->RenderImGui(ent);
		}

		//
		AddComponentButton(registry, ent);

		ImGui::End();
	}

	void Inspector::AddComponentButton(ecs::Registry* registry, mapping::rename_type::Entity ent)
	{
		const std::string add_comp = "Add Component";
		const float font_size = ImGui::GetFontSize() * add_comp.size() / 2;
		ImGui::Spacing();
		ImGui::SameLine(
			ImGui::GetWindowSize().x / 2 -
			font_size + (font_size / 2)
		);
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("my_file_popup");
		if (!ImGui::BeginPopup("my_file_popup"))
			return;

		bool is_used_mesh    = false;
		bool is_used_physics = false;

		ImGui::MenuItem("Component", nullptr, false, false);
		//for(auto& [key,value] : registry->GetComponentArrays())
		//{
		//	if (value.get()->GetComponentTag() == component::ComponentTag::Mesh)
		//	{
		//		if (!ImGui::BeginMenu("Mesh"))
		//			continue;
		//		if (!is_used_mesh)
		//			ImGui::MenuItem("Mesh", nullptr, false, false);
		//		if (ImGui::MenuItem(key.c_str()))
		//		{

		//		}
		//		is_used_mesh = true;
		//		ImGui::EndMenu();
		//	}
		//}

		for(auto& [key,value] : component_map)
		{
			if(value.second.get()->GetComponentTag() == mapping::component_tag::ComponentTag::Mesh)
			{
				if (!ImGui::BeginMenu("Mesh"))
					continue;
				if(!is_used_mesh)
					ImGui::MenuItem("Mesh", nullptr, false, false);
				if(ImGui::MenuItem(key.c_str()))
				{

				}
				is_used_mesh = true;
				ImGui::EndMenu();
			}

			if(value.second.get()->GetComponentTag() == mapping::component_tag::ComponentTag::Physics)
			{
				if (!ImGui::BeginMenu("Physics"))
					continue;

				if(!is_used_physics)
					ImGui::MenuItem("Physics", nullptr, false, false);
				if (ImGui::MenuItem(key.c_str()))
				{
					//registry->AddComponent<component::PhysicMaterialComponent>(ent);
					auto* comp = registry->GetComponentArrays().at(value.first)->AddComponentFromInspector(ent);
					comp->SetRegistry(registry);
					comp->SetEntity(ent);
				}
				is_used_physics = true;
				ImGui::EndMenu();
			}
		}

		ImGui::EndPopup(); // "my_file_popup"
	}

	template <typename T>
	void Inspector::RegisterComponent(const std::string& comp_name, const mapping::component_tag::ComponentTag tag)
	{
		if(component_map.contains(comp_name))
			assert(!"The name already exists(Inspector::RegisterComponent)");

		component_map.insert(std::make_pair(comp_name, std::make_pair(file_path_helper::GetTypeName<T>(), std::make_unique<T>(tag))));
	}

} // cumulonimbus::editor