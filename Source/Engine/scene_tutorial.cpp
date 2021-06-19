#include "scene_tutorial.h"

#include <memory>

//#include "component_list.h"
//#include "mesh_object.h"
//#include "collision_component.h"
//#include "ecs.h"
//#include "fbx_model_component.h"
//#include "geometric_primitive_component.h"
//#include "input_manager.h"
//#include "input_device.h"
//#include "obj_model_component.h"
//#include "sphere_collision_component.h"
//#include "sprite.h"
//#include "scene_title.h"
//#include "scene_load.h"
//#include "texture.h"
//#include "transform_component.h"
//#include "camera_operation.h"
//#include "sky_box.h"
//#include "locator.h"

#include "input_system.h"

void SceneTutorial::InitializeScene()
{
	current_scene = SceneType::Tutorial;
	scene_name = "Scene_Tutorial";

	//const char* sky_filename = "./Data/Assets/cubemap/skybox";
	//auto* sky_box = AddEntity(UpdateOrder::Default, EntityTag::Sky_Map);
	//sky_box->AddComponent<SkyBox>(GetFramework()->GetDevice(), sky_filename);
	//sky_box->GetComponent<TransformComponent>()->SetScale(3);
	//sky_box->SetEntityName("Sky_Box");

	//auto* stage = AddEntity(UpdateOrder::Default);
	//stage->AddComponent<MeshObject>();
	//stage->AddComponent<GeomPrimComponent>(GeomPrimType::Cube);
	//stage->GetComponent<TransformComponent>()->SetScale({ 100.0f,0.1f,100.0f });
	//stage->SetEntityName("Stage");

	////const char* bunny_filename = "./Data/Assets/Bin/stanford_bunny.bin";
	////std::shared_ptr<FbxModelResource> bunny_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), bunny_filename, " ");
	//auto* bunny = AddEntity();
	//bunny->AddComponent<MeshObject>();
	//bunny->AddComponent<FbxModelComponent>(Locator::GetResourceManager()->FbxModelResouece("stanford_bunny"));
	//bunny->GetComponent<TransformComponent>()->SetScale(0.01f);
	//bunny->GetComponent<TransformComponent>()->SetPositionY(5);
	//bunny->GetComponent<MeshObject>()->SetRasterizerState(RasterizeState::Cull_Back);
	//bunny->SetEntityName("Bunny");

	////const char* ganfaul_filename = "./Data/Assets/Bin/ganfaul.bin";
	////std::shared_ptr<FbxModelResource> ganfaul_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ganfaul_filename, "./Data/Assets/FBX/ganfaul/character.fbm/");
	//auto* player = AddEntity(UpdateOrder::Default, EntityTag::Player);
	//player->AddComponent<MeshObject>();
	//player->AddComponent<FbxModelComponent>(Locator::GetResourceManager()->FbxModelResouece("ganfaul"));
	//player->GetComponent<TransformComponent>()->SetScale(0.05f);
	//player->GetComponent<TransformComponent>()->SetPosition({-10,3,0});
	//player->GetComponent<MeshObject>()->SetRasterizerState(RasterizeState::Cull_Back_CCW_True);
	//player->GetComponent<FbxModelComponent>()->SwitchAnimation(2,true); // 0,1 T_Pose 2 Walk 3 Run 4 Attack
	//player->SetEntityName("Player");

	//auto* sphere = AddEntity(UpdateOrder::Default);
	//sphere->AddComponent<MeshObject>();
	//sphere->AddComponent<GeomPrimComponent>(GeomPrimType::Sphere);
	//sphere->GetComponent<TransformComponent>()->SetPosition({ -5.0f,4.0f,0.0f });
	//sphere->GetComponent<TransformComponent>()->SetScale(3);
	//sphere->SetEntityName("Sphere");

	//-- Light --//
	light->SetLightDir({ .0f,-.6f,1.f ,1.f });

	//-- View --//
	view->SetCameraPos(XMFLOAT3(.0f, 30.0, -100.0f), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));

	//-- View --//
	//view->SetCameraPos(XMFLOAT3(.0f, 1000.0f, -1000.0f), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));
}

void SceneTutorial::UpdateScene(const float delta_time)
{

	//if (PadInput::GetInstance()->IsInput(0, InputType::GamePad::Start, PadInput::PadState::Press)
	//	|| KeyInput::GetInstance().KeyTrg(static_cast<int>(InputType::Key::KEY_BACK)))
	//{
	//	is_paused = !is_paused;

	//	GetSoundResourceManager()->GetSystemSound(System_SE::MenuCursor)->Play(false);
	//}

	//ImGui::Text("%f", InputSystem::Instance()->GamePad().LeftThumbStick(0).x);
	//ImGui::Text("%f", InputSystem::Instance()->GamePad().LeftThumbStick(0).y);

	//if (InputSystem::Instance()->GamePad().GetState(GamePadButton::LeftShoulder) == ButtonState::Press)
	//{
	//	ImGui::Text("Press");
	//}
	//if (InputSystem::Instance()->GamePad().GetState(GamePadButton::LeftShoulder) == ButtonState::Held)
	//{
	//	ImGui::Text("Held");
	//}
	//if (InputSystem::Instance()->GamePad().GetState(GamePadButton::LeftShoulder) == ButtonState::Release)
	//{
	//	ImGui::Text("Release");
	//}
	//if (InputSystem::Instance()->GamePad().GetState(GamePadButton::LeftShoulder) == ButtonState::None)
	//{
	//	ImGui::Text("None");
	//}

	//GetOtherEntity(EntityTag::Player)->GetComponent<FbxModelComponent>()->UpdateAnimState(delta_time);

	//const SimpleMath::Vector3 angle{ -43.902f,-29.268f,1.00f };

	//SimpleMath::Vector3 vec = { view->GetCameraFront().x,view->GetCameraFront().y,view->GetCameraFront().z };
	//SimpleMath::Matrix mat;
	//mat = mat.CreateFromYawPitchRoll(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z));

	//vec = XMVector3TransformCoord(vec, mat);

	//light->SetLightDir({ vec.x ,vec.y,vec.z,0 });

	//ImGui::SliderFloat("x", &v.x, -180, 180);
	//ImGui::SliderFloat("y", &v.y, -180, 180);
	//ImGui::SliderFloat("z", &v.z, -180, 180);

	//light->SetLightDir({ view->GetCameraFront().x,view->GetCameraFront().y,view->GetCameraFront().z,0 });
}

void SceneTutorial::UnInitializeScene()
{

}