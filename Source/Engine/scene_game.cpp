#include "scene_game.h"

#include <memory>

#include "anim_sprite.h"
#include "camera_component.h"
#include "capsule_collison_component.h"
#include "rigid_body_component.h"
#include "fbx_model_component.h"
#include "input_device.h"
#include "input_manager.h"
#include "mesh_object.h"
#include "player_component.h"
#include "enemy_slime_component.h"]
#include "enemy_forest_demon_component.h"
#include "scene_title.h"
#include "sky_box.h"
#include "sphere_collision_component.h"

#include "transform_component.h"

using namespace shader;

void SceneGame::InitializeScene()
{
	namespace cum = cumulonimbus;

	current_scene = SceneType::Game;
	scene_name = "Scene_Game";

	registry->SetScene(this);

	//-- Light --//
	light->SetLightDir({ 0.0f,-.6f,1.f });
	light->SetPosition({ -400, 500, 0 });

	//registry->Load("./Data/Scenes/Sample_Scene2");

	const cum::mapping::rename_type::Entity ent_main_camera = registry->CreateEntity();
	registry->AddComponent<cum::component::CameraComponent>(ent_main_camera, true);
	registry->GetComponent<cum::component::CameraComponent>(ent_main_camera).SetViewInfo({ 25,100,-300 }, { .0f, .0f, .0f }, XMFLOAT3(.0f, 1.0f, .0f));
	registry->GetComponent<cum::component::CameraComponent>(ent_main_camera).SetProjection(XM_PI / 8.0f, static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);

	const char* sky_filename = "./Data/Assets/cubemap/skybox";
	const cum::mapping::rename_type::Entity ent_sky_box = registry->CreateEntity();
	registry->AddComponent<cum::component::SkyBoxComponent>(ent_sky_box, cum::locator::Locator::GetDx11Device()->device.Get(), sky_filename);
	registry->GetComponent<cum::component::TransformComponent>(ent_sky_box).SetScale(3.f);

	//SaveScene("./Data/Assets/Scene.json");
	//LoadScene("./Data/Assets/Scene.json");

	////registry->CreateEntity();
	////registry->CreateEntity();

	//const cum::mapping::rename_type::Entity ent_stage_01 = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_stage_01);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_stage_01, cum::locator::Locator::GetResourceManager()->FbxModelResouece("Stage_01_Collision"));
	//registry->GetComponent<cum::component::FbxModelComponent>(ent_stage_01).SetAllShader(cumulonimbus::mapping::shader_assets::ShaderAsset3D::SampleShader);
	//registry->GetComponent<cum::component::TransformComponent>(ent_stage_01).SetPosition({ 0,220,0 });
	//collision_manager->RegistryRayCastModel(ent_stage_01);

	//const cum::mapping::rename_type::Entity ent_ene_00 = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_ene_00);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_ene_00, cum::locator::Locator::GetResourceManager()->FbxModelResouece("Soldier"));
	//registry->GetComponent<cum::component::TransformComponent>(ent_ene_00).SetScale(0.3f);
	//registry->GetComponent<cum::component::TransformComponent>(ent_ene_00).SetPosition({ 100, 300, -1800 });
	//registry->AddComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00, CollisionTag::Enemy);
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00).AddCapsule();
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00).SetAllRadius(20.0f);
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00).SetAllLength(40.0f);
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00).SetAllCollisionPreset(cumulonimbus::collision::CollisionPreset::BlockAll);
	//registry->AddComponent<cum::component::RigidBodyComponent>(ent_ene_00);
	//registry->GetComponent<cum::component::RigidBodyComponent>(ent_ene_00).GravityStop(true);

	//const cum::mapping::rename_type::Entity ent_player = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_player);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_player, cum::locator::Locator::GetResourceManager()->FbxModelResouece("Robot"));
	//registry->AddComponent<cum::component::PlayerComponent>(ent_player);
	//registry->AddComponent<cum::component::CapsuleCollisionComponent>(ent_player, CollisionTag::Player);
	//registry->AddComponent<cum::component::CameraComponent>(ent_player, true);
	//registry->AddComponent<cum::component::RigidBodyComponent>(ent_player);
	//registry->GetComponent<cum::component::TransformComponent>(ent_player).SetScale(0.3f);
	//registry->GetComponent<cum::component::TransformComponent>(ent_player).SetPosition({ 100, 200, -1600 });
	//registry->GetComponent<cum::component::MeshObjectComponent>(ent_player).SetRasterizerState(RasterizeState::Cull_Front_CCW_True);
	//registry->GetComponent<cum::component::FbxModelComponent>(ent_player).SwitchAnimation(4, true);
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).AddCapsule();
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).SetAllRadius(20.0f);
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).SetAllLength(40.0f);
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).SetAllCollisionPreset(cumulonimbus::collision::CollisionPreset::BlockAll);
	//registry->GetComponent<cum::component::CameraComponent>(ent_player).AttachObject(ent_player);
	//registry->GetComponent<cum::component::CameraComponent>(ent_player).InitializeObjectCameraParam(300);
	//registry->GetComponent<cum::component::CameraComponent>(ent_player).SetIsDebugCamera(false);
	//registry->GetComponent<cum::component::CameraComponent>(ent_player).SetProjection(XM_PI / 8.0f, static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);
	//registry->GetComponent<cum::component::TransformComponent>(ent_player).ActiveQuaternion();

	//registry->Save("./Data/Scenes", "Sample_Scene2");
}

void SceneGame::UpdateScene(const float delta_time)
{
	//if (PadInput::GetInstance()->IsInput(0,InputType::GamePad::Start,PadInput::PadState::Press)
	//	|| KeyInput::GetInstance().KeyTrg(static_cast<int>(InputType::Key::KEY_BACK)))
	//{
	//	is_paused = !is_paused;

	//	GetSoundResourceManager()->GetSystemSound(System_SE::MenuCursor)->Play(false);
	//}

	//const SimpleMath::Vector3 angle{ -43.902f,-29.268f,1.00f };

	//SimpleMath::Vector3 vec = { view->GetCameraFront().x,view->GetCameraFront().y,view->GetCameraFront().z };
	//SimpleMath::Matrix mat;
	//mat = mat.CreateFromYawPitchRoll(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z));

	//vec = XMVector3TransformCoord(vec, mat);

	//light->SetLightDir({ vec.x ,vec.y,vec.z,0 });
}

void SceneGame::UnInitializeScene()
{
	sound_resource->GetBGMSound(BGM::Game)->Stop();
}