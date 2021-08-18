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

	const cum::mapping::rename_type::Entity ent_main_camera = registry->CreateEntity();
	//registry->AddComponent<cum::component::CameraComponent>(ent_main_camera, false);
	//registry->GetComponent<cum::component::CameraComponent>(ent_main_camera).SetViewInfo({ 25,100,-300 }, { .0f, .0f, .0f }, XMFLOAT3(.0f, 1.0f, .0f));
	//registry->GetComponent<cum::component::CameraComponent>(ent_main_camera).SetProjection(XM_PI / 8.0f, static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);

	const char* sky_filename = "./Data/Assets/cubemap/skybox";
	const cum::mapping::rename_type::Entity ent_sky_box = registry->CreateEntity();
	registry->AddComponent<cum::component::SkyBoxComponent>(ent_sky_box, cum::locator::Locator::GetDx11Device()->device.Get(), sky_filename);
	registry->GetComponent<cum::component::TransformComponent>(ent_sky_box).SetScale(3.f);

	//const char* ground_filename = "./Data/Assets/Bin/ground.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
	//std::shared_ptr<FbxModelResource> ground_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ground_filename, "./Data/Assets/FBX/ground/");
	const cum::mapping::rename_type::Entity ent_floor = registry->CreateEntity();
	registry->AddComponent<cum::component::MeshObjectComponent>(ent_floor);
	registry->AddComponent<cum::component::FbxModelComponent>(ent_floor, cum::locator::Locator::GetResourceManager()->FbxModelResouece("cube"));
	registry->GetComponent<cum::component::TransformComponent>(ent_floor).SetScale(DirectX::XMFLOAT3{ 500,500,0.1f });
	registry->GetComponent<cum::component::TransformComponent>(ent_floor).SetWorldRotation_X(90);
	registry->GetComponent<cum::component::MeshObjectComponent>(ent_floor).SetSamplerState(RenderingSampleState::Linear_Wrap);
	collision_manager->RegistryRayCastModel(ent_floor);

	//const cum::mapping::rename_type::Entity ent_bunny = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_bunny);
	//registry->GetComponent<cum::component::MeshObjectComponent>(ent_bunny).SetShaderState(shader::MeshShaderTypes::Metal);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_bunny, cum::locator::Locator::GetResourceManager()->FbxModelResouece("stanford_bunny"));
	//registry->GetComponent<cum::component::TransformComponent>(ent_bunny).SetScale(0.1f);

	// ‚±‚±‚©‚ç
	//const cum::mapping::rename_type::Entity ent_ganfaul = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_ganfaul);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_ganfaul, cum::locator::Locator::GetResourceManager()->FbxModelResouece("ganfaul"));
	//registry->GetComponent<cum::component::TransformComponent>(ent_ganfaul).SetScale(0.3f);
	////registry->GetComponent<cum::component::TransformComponent>(ent_player).SetPosition({ 0, -20, 0 });
	//registry->GetComponent<cum::component::TransformComponent>(ent_ganfaul).SetPosition({ 30, 10, 0 });
	//registry->GetComponent<cum::component::TransformComponent>(ent_ganfaul).SetWorldRotation_Y(180);
	//registry->GetComponent<cum::component::MeshObjectComponent>(ent_ganfaul).SetRasterizerState(RasterizeState::Cull_Front_CCW_True);
	//registry->GetComponent<cum::component::FbxModelComponent>(ent_ganfaul).SwitchAnimation(2, true);

	//const cum::mapping::rename_type::Entity ent_sphere = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_sphere);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_sphere, cum::locator::Locator::GetResourceManager()->FbxModelResouece("sphere"));
	//registry->GetComponent<cum::component::TransformComponent>(ent_sphere).SetPosition({ 50, 0, 0 });
	//registry->GetComponent<cum::component::TransformComponent>(ent_sphere).SetWorldRotation_Y(180);
	//registry->AddComponent<cum::component::CapsuleCollisionComponent>(ent_sphere, CollisionTag::Player);
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_sphere).AddCapsule();
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_sphere).SetAllRadius(10.0f);
	//registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_sphere).SetAllLength(20.0f);

	//const cum::mapping::rename_type::Entity ent_sphere2 = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_sphere2);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_sphere2, cum::locator::Locator::GetResourceManager()->FbxModelResouece("sphere"));
	//registry->GetComponent<cum::component::TransformComponent>(ent_sphere2).SetPosition({ 2, 0, 0 });
	//registry->GetComponent<cum::component::TransformComponent>(ent_sphere2).SetWorldRotation_Y(180);

	const cum::mapping::rename_type::Entity ent_stage_01 = registry->CreateEntity();
	registry->AddComponent<cum::component::MeshObjectComponent>(ent_stage_01);
	registry->AddComponent<cum::component::FbxModelComponent>(ent_stage_01, cum::locator::Locator::GetResourceManager()->FbxModelResouece("Stage_01"));
	registry->GetComponent<cum::component::FbxModelComponent>(ent_stage_01).SetAllShader(cumulonimbus::mapping::shader_assets::ShaderAsset3D::SampleShader);
	registry->GetComponent<cum::component::TransformComponent>(ent_stage_01).SetPosition({ 0,230,0 });

	//const cum::mapping::rename_type::Entity ent_ene_00 = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_ene_00);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_ene_00, cum::locator::Locator::GetResourceManager()->FbxModelResouece("Slime"));
	//registry->AddComponent<cum::component::RigidBodyComponent>(ent_ene_00);
	//registry->AddComponent<cum::component::EnemySlimeComponent>(ent_ene_00);
	//registry->AddComponent<cum::component::SphereCollisionComponent>(ent_ene_00, CollisionTag::Enemy);
	//registry->GetComponent<cum::component::SphereCollisionComponent>(ent_ene_00).AddSphere();
	//registry->GetComponent<cum::component::SphereCollisionComponent>(ent_ene_00).SetAllRadius(20);
	//registry->GetComponent<cum::component::SphereCollisionComponent>(ent_ene_00).SetAllOffset({0,30,0});
	//registry->GetComponent<cum::component::SphereCollisionComponent>(ent_ene_00).SetAllCollisionPreset(cumulonimbus::collision::CollisionPreset::BlockAll);
	////registry->AddComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00, CollisionTag::Enemy);
	////registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00).AddCapsule();
	////registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00).SetAllRadius(20.0f);
	////registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00).SetAllLength(40.0f);
	////registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_ene_00).SetAllCollisionPreset(cumulonimbus::collision::CollisionPreset::BlockAll);
	//registry->GetComponent<cum::component::TransformComponent>(ent_ene_00).SetScale(0.1f);
	//registry->GetComponent<cum::component::TransformComponent>(ent_ene_00).SetPosition({100,0,0});
	//registry->GetComponent<cum::component::TransformComponent>(ent_ene_00).ActiveQuaternion();

	//const cum::mapping::rename_type::Entity ent_ene_01 = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_ene_01);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_ene_01, cum::locator::Locator::GetResourceManager()->FbxModelResouece("ForestDemon"));
	//registry->GetComponent<cum::component::TransformComponent>(ent_ene_01).SetScale(0.1f);
	//registry->GetComponent<cum::component::TransformComponent>(ent_ene_01).SetPosition({100,0,0});
	//registry->GetComponent<cum::component::TransformComponent>(ent_ene_01).ActiveQuaternion();
	//registry->AddComponent<cum::component::RigidBodyComponent>(ent_ene_01);
	//registry->AddComponent<cum::component::EnemyForestDemonComponent>(ent_ene_01);

	const cum::mapping::rename_type::Entity ent_player = registry->CreateEntity();
	registry->AddComponent<cum::component::MeshObjectComponent>(ent_player);
	registry->AddComponent<cum::component::FbxModelComponent>(ent_player, cum::locator::Locator::GetResourceManager()->FbxModelResouece("Robo"));
	registry->AddComponent<cum::component::PlayerComponent>(ent_player);
	registry->AddComponent<cum::component::CapsuleCollisionComponent>(ent_player, CollisionTag::Player);
	registry->AddComponent<cum::component::CameraComponent>(ent_player, true);
	registry->AddComponent<cum::component::RigidBodyComponent>(ent_player);
	registry->GetComponent<cum::component::TransformComponent>(ent_player).SetScale(0.3f);
	registry->GetComponent<cum::component::TransformComponent>(ent_player).SetPosition({ 100, 10, -1600 });
	registry->GetComponent<cum::component::MeshObjectComponent>(ent_player).SetRasterizerState(RasterizeState::Cull_Front_CCW_True);
	registry->GetComponent<cum::component::FbxModelComponent>(ent_player).SwitchAnimation(4, true);
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).AddCapsule();
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).SetAllRadius(20.0f);
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).SetAllLength(40.0f);
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).SetAllCollisionPreset(cumulonimbus::collision::CollisionPreset::BlockAll);
	registry->GetComponent<cum::component::CameraComponent>(ent_player).AttachObject(ent_player);
	registry->GetComponent<cum::component::CameraComponent>(ent_player).InitializeObjectCameraParam(300);
	registry->GetComponent<cum::component::CameraComponent>(ent_player).SetIsDebugCamera(false);
	registry->GetComponent<cum::component::CameraComponent>(ent_player).SetProjection(XM_PI / 8.0f, static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenWidth()) / static_cast<float>(cumulonimbus::locator::Locator::GetDx11Device()->GetScreenHeight()), 0.1f, 2000.0f);
	registry->GetComponent<cum::component::TransformComponent>(ent_player).ActiveQuaternion();
	//registry->AddComponent<cum::component::SphereCollisionComponent>(ent_player, CollisionTag::Enemy);
	//registry->GetComponent<cum::component::SphereCollisionComponent>(ent_player).AddSphere();
	//registry->GetComponent<cum::component::SphereCollisionComponent>(ent_player).SetAllRadius(20);
	//registry->GetComponent<cum::component::SphereCollisionComponent>(ent_player).SetAllOffset({ 0,30,0 });
	//registry->GetComponent<cum::component::SphereCollisionComponent>(ent_player).SetAllCollisionPreset(cumulonimbus::collision::CollisionPreset::BlockAll);
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