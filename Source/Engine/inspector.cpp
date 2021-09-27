#include "inspector.h"

#include <cassert>

#include "component_base.h"
#include "ecs.h"
#include "fbx_model_resource.h"
#include "file_path_helper.h"
// components
#include "raycast_component.h"
#include "capsule_collison_component.h"
#include "physic_material_component.h"
#include "rigid_body_component.h"
#include "sphere_collision_component.h"
#include "model_component.h"

namespace cumulonimbus::editor
{
	Inspector::Inspector()
	{
		RegisterComponent<component::ModelComponent>(			"Model Component"	 , mapping::component_tag::ComponentTag::Mesh);
		RegisterComponent<component::RayCastComponent>(			"RayCast Collider"	 , mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::CapsuleCollisionComponent>("Capsule Collider"	 , mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::PhysicMaterialComponent>(	"Physic Material"	 , mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::SphereCollisionComponent>(	"Sphere Collider"	 , mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::RigidBodyComponent>(		"RigidBody"			 , mapping::component_tag::ComponentTag::Physics);
	}

	void Inspector::Render(ecs::Registry* registry, const mapping::rename_type::Entity ent)
	{
		if (ImGui::Begin(ICON_FA_INFO_CIRCLE" Inspector"))
		{
			if (!registry->GetEntities().contains(ent))
			{
				ImGui::End();
				return;
			}

			// エンティティ名の変更
			std::string& name = registry->GetEntities().at(ent).second;
			const size_t arr_size = name.length();
			char buf[256];
			strcpy_s(buf, arr_size + 1, name.c_str());
			if (ImGui::InputText("##", buf, 64, ImGuiInputTextFlags_CharsNoBlank))
			{
				name = buf;
			}

			// エンティティの持つコンポーネントの詳細
			for (auto& [comp_name, comp_data] : registry->GetComponentArrays())
			{
				comp_data->RenderImGui(ent);
			}

			// 「Add Component」のボタン
			AddComponentButton(registry, ent);
		}
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

		ImGui::MenuItem("Component", nullptr, false, false);

		ComponentMenu(registry, ent, "Mesn", mapping::component_tag::ComponentTag::Mesh);
		ComponentMenu(registry, ent, "Physics", mapping::component_tag::ComponentTag::Physics);

		ImGui::EndPopup(); // "my_file_popup"
	}

	void Inspector::ComponentMenu(ecs::Registry* registry, const mapping::rename_type::Entity ent, const std::string& menu_name, const mapping::component_tag::ComponentTag tag)
	{
		bool is_used = false;

		for (auto& [key, value] : component_map)
		{
			if (value.second.get()->GetComponentTag() == tag)
			{
				if (!ImGui::BeginMenu(menu_name.c_str()))
					continue;
				if(!is_used)
					ImGui::MenuItem(menu_name.c_str(), nullptr, false, false);
				if (ImGui::MenuItem(key.c_str()))
				{
					auto* comp = registry->GetComponentArrays().at(value.first)->AddComponentFromInspector(ent);
					comp->SetRegistry(registry);
					comp->SetEntity(ent);
				}
				is_used = true;
				ImGui::EndMenu();
			}

		}
	}


	template <typename T>
	void Inspector::RegisterComponent(const std::string& comp_name, const mapping::component_tag::ComponentTag tag)
	{
		if(component_map.contains(comp_name))
			assert(!"The name already exists(Inspector::RegisterComponent)");

		component_map.insert(std::make_pair(comp_name, std::make_pair(file_path_helper::GetTypeName<T>(), std::make_unique<T>(tag))));
	}

} // cumulonimbus::editor