#include "inspector.h"

#include <cassert>

#include "component_base.h"
#include "ecs.h"
#include "file_path_helper.h"
//-- components(engine) --//
// camera
#include "camera_component.h"
// collider
#include "capsule_collison_component.h"
#include "collider_message_receiver_component.h"
#include "collider_message_sender_component.h"
#include "raycast_component.h"
#include "sphere_collision_component.h"
// effekseer
#include "effekseer_component.h"
// light
#include "directiona_light_component.h"
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
// spawner
#include "effect_spawner_component.h"
#include "prefab_spawner_component.h"
// transition
#include "scene_transition_component.h"

//-- components(game) --//
// enemy
#include "enemy_boss_component.h"
#include "enemy_soldier_component.h"
// player
#include "player_component.h"
#include "player_avoid_effect_component.h"
// weapon
#include "energy_shot_component.h"
#include "player_sword_component.h"
#include "soldier_sword_component.h"
// spawner
#include "flame_spawner_component.h"
#include "magic_circle_spawner_component.h"

namespace cumulonimbus::editor
{
	Inspector::Inspector()
	{
		RegisterComponent<component::ModelComponent>(					"Model"					, mapping::component_tag::ComponentTag::Mesh);
		RegisterComponent<component::SpriteComponent>(					"Sprite"				, mapping::component_tag::ComponentTag::Sprite);
		RegisterComponent<component::BillboardComponent>(				"Billboard"				, mapping::component_tag::ComponentTag::Sprite);
		RegisterComponent<component::EffekseerComponent>(				"Effekseer"				, mapping::component_tag::ComponentTag::Sprite);
		RegisterComponent<component::PhysicMaterialComponent>(			"Physic Material"		, mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::RigidBodyComponent>(				"RigidBody"				, mapping::component_tag::ComponentTag::Physics);
		RegisterComponent<component::RayCastComponent>(					"RayCast Collider"		, mapping::component_tag::ComponentTag::Collider);
		RegisterComponent<component::CapsuleCollisionComponent>(		"Capsule Collider"		, mapping::component_tag::ComponentTag::Collider);
		RegisterComponent<component::SphereCollisionComponent>(			"Sphere Collider"		, mapping::component_tag::ComponentTag::Collider);
		RegisterComponent<component::ColliderMessageReceiverComponent>(	"Message Receiver"		, mapping::component_tag::ComponentTag::Collider);
		RegisterComponent<component::ColliderMessageSenderComponent>(	"Message Sender"		, mapping::component_tag::ComponentTag::Collider);
		RegisterComponent<component::DirectionalLightComponent>(		"Directional Light"		, mapping::component_tag::ComponentTag::Light);
		RegisterComponent<component::PointLightComponent>(				"Point Light"			, mapping::component_tag::ComponentTag::Light);
		RegisterComponent<component::SpotLightComponent>(				"Spot Light"			, mapping::component_tag::ComponentTag::Light);
		RegisterComponent<component::CameraComponent>(					"Camera"				, mapping::component_tag::ComponentTag::Camera);
		RegisterComponent<component::EffectSpawnerComponent>(			"Effect Spawner"		, mapping::component_tag::ComponentTag::Spawner);
		RegisterComponent<component::PrefabSpawnerComponent>(			"Prefab Spawner"		, mapping::component_tag::ComponentTag::Spawner);
		RegisterComponent<component::MagicCircleSpawnerComponent>(		"Magic Circle Spawner"	, mapping::component_tag::ComponentTag::Spawner);
		RegisterComponent<component::FlameSpownerComponent>(			"Flame Spawner"			, mapping::component_tag::ComponentTag::Spawner);
		RegisterComponent<component::SceneTransitionComponent>(			"Scene Transition"		, mapping::component_tag::ComponentTag::Transition);
		RegisterComponent<component::PlayerAvoidEffectComponent>(		"Player Avoid Effect"	, mapping::component_tag::ComponentTag::Game);
		RegisterComponent<component::EnemySoldierComponent>(			"E_Soldier"				, mapping::component_tag::ComponentTag::Game);
		RegisterComponent<component::EnemyBossComponent>(				"E_Boss"				, mapping::component_tag::ComponentTag::Game);
		RegisterComponent<component::EnergyShotComponent>(				"Energy Shot"			, mapping::component_tag::ComponentTag::Game);
		RegisterComponent<component::PlayerSwordComponent>(				"Player Sword"			, mapping::component_tag::ComponentTag::Game);
		RegisterComponent<component::SoldierSwordComponent>(			"Solider Sword"			, mapping::component_tag::ComponentTag::Game);
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

		ComponentMenu(registry, ent, "Mesh"		 , mapping::component_tag::ComponentTag::Mesh);
		ComponentMenu(registry, ent, "Sprite"	 , mapping::component_tag::ComponentTag::Sprite);
		ComponentMenu(registry, ent, "Physics"	 , mapping::component_tag::ComponentTag::Physics);
		ComponentMenu(registry, ent, "Collider"  , mapping::component_tag::ComponentTag::Collider);
		ComponentMenu(registry, ent, "Light"	 , mapping::component_tag::ComponentTag::Light);
		ComponentMenu(registry, ent, "Camera"	 , mapping::component_tag::ComponentTag::Camera);
		ComponentMenu(registry, ent, "Spawner"   , mapping::component_tag::ComponentTag::Spawner);
		ComponentMenu(registry, ent, "Transition", mapping::component_tag::ComponentTag::Transition);
		ComponentMenu(registry, ent, "Game"		 , mapping::component_tag::ComponentTag::Game);

		ImGui::EndPopup(); // "my_file_popup"
	}

	void Inspector::ComponentMenu(ecs::Registry* registry, const mapping::rename_type::Entity ent, const std::string& menu_name, const mapping::component_tag::ComponentTag tag)
	{
		for (bool is_used = false;
			 auto& [key, value] : component_map)
		{
			if (value.second.get()->GetComponentTag() == tag)
			{
				if (!ImGui::BeginMenu(menu_name.c_str()))
					continue;
				if(!is_used)
					ImGui::MenuItem(menu_name.c_str(), nullptr, false, false);
				if (ImGui::MenuItem(key.c_str()))
				{
					auto* comp = registry->GetComponentArrays().at(value.first)->AddComponent(ent);
					comp->SetRegistry(registry);
					comp->SetEntity(ent);
					comp->Initialize(registry, ent);
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

		component_map.emplace(comp_name, std::make_pair(file_path_helper::GetTypeName<T>(), std::make_unique<T>(tag)));
	}

} // cumulonimbus::editor