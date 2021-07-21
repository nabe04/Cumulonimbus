#include "scene_game.h"

#include <memory>


#include "fbx_model_component.h"
#include "input_manager.h"
#include "component_list.h"
#include "input_device.h"
#include "scene_title.h"
#include "sky_box.h"
#include "mesh_object.h"
#include "anim_sprite.h"
#include "player_component.h"
#include "sphere_collision_component.h"
#include "capsule_collison_component.h"

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

	//-- View --//
	view->SetViewInfo({ 25,100,-300 }, { .0f, .0f, .0f }, XMFLOAT3(.0f, 1.0f, .0f));

	//auto* test_entity = AddEntity();
	//test_entity->SetEntityName("Test_Entity");
	//test_entity->AddComponent<MeshObject>();
	//test_entity->GetComponent<TransformComponent>()->SetPosition({ 10,10,10 });
	//test_entity->AddComponent<ActorComponent>(ActorType::Actor3D);

	//const char* uv_chekcer_filename = "uv_checker";
	//auto* uv_chekcer = AddEntity();
	//uv_chekcer->AddComponent<SpriteObject>();
	//uv_chekcer->AddComponent<AnimSpriteComponent>(GetFramework()->GetDevice(), uv_chekcer_filename, PivotType::Center,
	//	0, 0,
	//	256, 256,
	//	8, 8);
	//uv_chekcer->GetComponent<TransformComponent>()->SetPosition({ 500,500,0 });
	//uv_chekcer->SetEntityName("UV_Chacker");

	//const char* ganfaul_filename = "./Data/Assets/Bin/ganfaul.bin";
	//std::shared_ptr<FbxModelResource> ganfaul_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ganfaul_filename, "./Data/Assets/FBX/ganfaul/character.fbm/");
	//auto* player = AddEntity(UpdateOrder::Default, EntityTag::Player);
	//player->AddComponent<MeshObject>();
	//player->AddComponent<FbxModelComponent>(cum::locator::Locator::GetResourceManager()->FbxModelResouece("ganfaul"));
	//player->GetComponent<TransformComponent>()->SetScale(0.3f);
	//player->GetComponent<TransformComponent>()->SetPosition({ 0,10, 0 });
	//player->GetComponent<TransformComponent>()->SetWorldRotation_Y(180);
	//player->GetComponent<MeshObject>()->SetRasterizerState(RasterizeState::Cull_Back_CCW_True);
	//player->GetComponent<FbxModelComponent>()->SwitchAnimation(2, true); // 0,1 T_Pose 2 Walk 3 Run 4 Attack
	//player->SetEntityName("Player");

	//SaveScene("Test5_30");

	//const char* sky_filename = "./Data/Assets/cubemap/skybox";
	//auto* sky_box = AddEntity(UpdateOrder::Default, EntityTag::Sky_Map);
	//sky_box->AddComponent<SkyBox>(GetFramework()->GetDevice(), sky_filename);
	//sky_box->GetComponent<TransformComponent>()->SetScale(3);
	//sky_box->SetEntityName("Sky_Box");

	//const char* ganfaul_filename = "./Data/Assets/Bin/ganfaul.bin";
	//std::shared_ptr<FbxModelResource> ganfaul_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ganfaul_filename, "./Data/Assets/FBX/ganfaul/character.fbm/");
	//auto* player = AddEntity(UpdateOrder::Default, EntityTag::Player);
	//player->AddComponent<MeshObject>();
	//player->AddComponent<FbxModelComponent>(locator::cum::locator::Locator::GetResourceManager()->FbxModelResouece("ganfaul"));
	//player->GetComponent<TransformComponent>()->SetScale(0.3f);
	//player->GetComponent<TransformComponent>()->SetPosition({ 0,10, 0 });
	//player->GetComponent<TransformComponent>()->SetWorldRotation_Y(180);
	//player->GetComponent<MeshObject>()->SetRasterizerState(RasterizeState::Cull_Back_CCW_True);
	//player->GetComponent<FbxModelComponent>()->SwitchAnimation(2, true); // 0,1 T_Pose 2 Walk 3 Run 4 Attack
	//player->SetEntityName("Player");

	//const char* ground_filename = "./Data/Assets/Bin/ground.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
	//std::shared_ptr<FbxModelResource> ground_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ground_filename, "./Data/Assets/FBX/ground/");
	//auto* ground = AddEntity();
	//ground->AddComponent<MeshObject>();
	//ground->AddComponent<FbxModelComponent>(ground_resource);
	//ground->GetComponent<TransformComponent>()->SetScale(DirectX::XMFLOAT3{ 1,1,0.1f });
	//ground->GetComponent<TransformComponent>()->SetWorldRotation_X(90);
	//ground->GetComponent<MeshObject>()->SetSamplerState(RenderingSampleState::Linear_Wrap);
	//ground->SetEntityName("Ground");

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

	const cum::mapping::rename_type::Entity ent_sphere = registry->CreateEntity();
	registry->AddComponent<cum::component::MeshObjectComponent>(ent_sphere);
	registry->AddComponent<cum::component::FbxModelComponent>(ent_sphere, cum::locator::Locator::GetResourceManager()->FbxModelResouece("sphere"));
	registry->GetComponent<cum::component::TransformComponent>(ent_sphere).SetPosition({ 50, 0, 0 });
	registry->GetComponent<cum::component::TransformComponent>(ent_sphere).SetWorldRotation_Y(180);
	registry->AddComponent<cum::component::CapsuleCollisionComponent>(ent_sphere, CollisionTag::Player);
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_sphere).AddCapsule();
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_sphere).SetAllRadius(10.0f);
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_sphere).SetAllLength(20.0f);

	//const cum::mapping::rename_type::Entity ent_sphere2 = registry->CreateEntity();
	//registry->AddComponent<cum::component::MeshObjectComponent>(ent_sphere2);
	//registry->AddComponent<cum::component::FbxModelComponent>(ent_sphere2, cum::locator::Locator::GetResourceManager()->FbxModelResouece("sphere"));
	//registry->GetComponent<cum::component::TransformComponent>(ent_sphere2).SetPosition({ 2, 0, 0 });
	//registry->GetComponent<cum::component::TransformComponent>(ent_sphere2).SetWorldRotation_Y(180);

	const cum::mapping::rename_type::Entity ent_player = registry->CreateEntity();
	registry->AddComponent<cum::component::MeshObjectComponent>(ent_player);
	registry->AddComponent<cum::component::PlayerComponent>(ent_player);
	registry->AddComponent<cum::component::FbxModelComponent>(ent_player, cum::locator::Locator::GetResourceManager()->FbxModelResouece("Robo"));
	registry->AddComponent<cum::component::CapsuleCollisionComponent>(ent_player, CollisionTag::Player);
	registry->GetComponent<cum::component::TransformComponent>(ent_player).SetScale(0.3f);
	registry->GetComponent<cum::component::TransformComponent>(ent_player).SetPosition({ -30, 10, 0 });
	registry->GetComponent<cum::component::MeshObjectComponent>(ent_player).SetRasterizerState(RasterizeState::Cull_Front_CCW_True);
	registry->GetComponent<cum::component::FbxModelComponent>(ent_player).SwitchAnimation(4, true);
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).AddCapsule();
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).SetAllRadius(20.0f);
	registry->GetComponent<cum::component::CapsuleCollisionComponent>(ent_player).SetAllLength(40.0f);
	view->AttachObject(ent_player);
	view->InitializeObjectCameraParam();
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