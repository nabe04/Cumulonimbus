#include "scene_game.h"

#include <memory>

#include "fbx_model_component.h"
#include "input_manager.h"
#include "input_device.h"
#include "scene_title.h"
#include "sky_box.h"
#include "mesh_object.h"


using namespace shader;

void SceneGame::InitializeScene()
{
	current_scene = SceneType::Game;
	scene_name = "Scene_Game";

	//-- Light --//
	light->SetLightDir({ 0.0f,-.6f,1.f ,1.f });
	light->SetPosition({ -400, 500, 0 ,1 });

	//-- View --//
	view->SetCameraPos({ 25,100,-300 }, { .0f, .0f, .0f }, XMFLOAT3(.0f, 1.0f, .0f));

	//auto* test_entity = AddEntity();
	//test_entity->AddComponent<MeshObject>();
	//test_entity->GetComponent<TransformComponent>()->GetTransform()->SetPosition({ 10,10,10 });
	//SaveScene("Test5_28");

	const char* sky_filename = "./Data/Assets/cubemap/skybox";
	auto* sky_box = AddEntity(UpdateOrder::Default, EntityTag::Sky_Map);
	sky_box->AddComponent<SkyBox>(GetFramework()->GetDevice(), sky_filename);
	sky_box->GetComponent<TransformComponent>()->GetTransform()->SetScale(3);
	sky_box->SetEntityName("Sky_Box");

	const char* ganfaul_filename = "./Data/Assets/Bin/ganfaul.bin";
	std::shared_ptr<FbxModelResource> ganfaul_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ganfaul_filename, "./Data/Assets/FBX/ganfaul/character.fbm/");
	auto* player = AddEntity(UpdateOrder::Default, EntityTag::Player);
	player->AddComponent<MeshObject>();
	player->AddComponent<FbxModelComponent>(Locator::GetResourceManager()->FbxModelResouece("ganfaul"));
	player->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.3f);
	player->GetComponent<TransformComponent>()->GetTransform()->SetPosition({ 0,10, 0 });
	player->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_Y(180);
	player->GetComponent<MeshObject>()->SetRasterizeState(RasterizeState::Cull_Back_CCW_True);
	player->GetComponent<FbxModelComponent>()->SwitchAnimation(2, true); // 0,1 T_Pose 2 Walk 3 Run 4 Attack
	player->SetEntityName("Player");

	const char* ground_filename = "./Data/Assets/Bin/ground.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
	std::shared_ptr<FbxModelResource> ground_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ground_filename, "./Data/Assets/FBX/ground/");
	auto* ground = AddEntity();
	ground->AddComponent<MeshObject>();
	ground->AddComponent<FbxModelComponent>(ground_resource);
	ground->GetComponent<TransformComponent>()->GetTransform()->SetScale(DirectX::XMFLOAT3{ 1,1,0.1f });
	ground->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_X(90);
	ground->GetComponent<MeshObject>()->SetSamplerState(RenderingSampleState::Linear_Wrap);
	ground->SetEntityName("Ground");
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