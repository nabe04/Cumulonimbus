#include "prefab.h"

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
	Prefab::Prefab(const mapping::rename_type::Entity& ent, ecs::Registry* registry)
	{
		//-- engine --//
		// transform
		RegistryComponent<component::TransformComponent>();
		// actor
		RegistryComponent<component::Actor3DComponent>();
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
		// sky box
		RegistryComponent<component::SkyBoxComponent>();
		//-- game --//
		// player
		RegistryComponent<component::PlayerComponent>();
		// enemy
		RegistryComponent<component::EnemyBaseComponent>();
		RegistryComponent<component::EnemySoldierComponent>();

		
	}

	template <class T>
	void Prefab::RegistryComponent()
	{
		const mapping::rename_type::ComponentName component_name = file_path_helper::GetTypeName<T>();

		if (components.contains(component_name))
			return;

		components.emplace(component_name, std::make_shared<ComponentAsset<T>>());
	}

} // cumulonimbus::asset
