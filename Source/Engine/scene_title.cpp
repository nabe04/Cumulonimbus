#include "scene_title.h"

#include "imgui.h"

#include "input_manager.h"
#include "scene_load.h"


#include "transform.h"
#include "transform_component.h"
#include "mesh_object.h"
#include "fbx_model_component.h"

#include "sprite_object.h"
#include "anim_sprite.h"
#include "sky_box.h"

#include "locator.h"

#include "sound_resource.h"

#include "scene_game.h"

void SceneTitle::InitializeScene()
{
	current_scene = SceneType::Title;
	scene_name = "Scene_Title";

	for (int i = 0; i < static_cast<int>(SelectGameMode::End); ++i)
	{
		select_game_mode.reset(i);
	}
	select_game_mode.set(static_cast<int>(SelectGameMode::TraininigMode));	// 常にどれかのビットをセットしておく(初期はトレーニングモード)

	//-- Light --//
	light->SetLightDir({ 1.0f,-.6f,1.f ,1.f });
	light->SetPosition({ -200,100,-100,1 });

	//-- View --//
	view->SetCameraPos({ 0,60,-800 }, { .0f, .0f, .0f }, XMFLOAT3(.0f, 1.0f, .0f));

	//const char* uv_chekcer_filename = "uv_checker";
	//auto* uv_chekcer = AddEntity();
	//uv_chekcer->AddComponent<SpriteObject>();
	//uv_chekcer->AddComponent<AnimSpriteComponent>(GetFramework()->GetDevice(), uv_chekcer_filename, PivotType::Center,
	//	0, 0,
	//	256, 256,
	//	8, 8);
	//uv_chekcer->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition({ 500,500,0 });
	//uv_chekcer->SetEntityName("UV_Chacker");

	const char* sky_filename = "./Data/Assets/cubemap/skybox";
	auto* sky_box = AddEntity(UpdateOrder::Default, EntityTag::Sky_Map);
	sky_box->AddComponent<SkyBox>(GetFramework()->GetDevice(), sky_filename);
	sky_box->GetComponent<TransformComponent>()->GetTransform()->SetScale(3);
	sky_box->SetEntityName("Sky_Box");

	//auto* sample01 = AddEntity(UpdateOrder::Default, EntityTag::Bunny);
	//sample01->AddComponent<MeshObject>();
	//sample01->GetComponent<MeshObject>()->SetShaderState(shader::MeshShaderTypes::Metal);
	//sample01->AddComponent<FbxModelComponent>(Locator::GetResourceManager()->FbxModelResouece("stanford_bunny"));
	//sample01->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.01f);
	//sample01->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition({ 0,4,30 });
	//sample01->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.01f);
	//sample01->AddComponent<SampleRotation>();
	//sample01->SetEntityName("Bunny");

	const char* ganfaul_filename = "./Data/Assets/Bin/ganfaul.bin";
	std::shared_ptr<FbxModelResource> ganfaul_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ganfaul_filename, "./Data/Assets/FBX/ganfaul/character.fbm/");
	auto* player = AddEntity(UpdateOrder::Default, EntityTag::Player);
	player->AddComponent<MeshObject>();
	player->AddComponent<FbxModelComponent>(Locator::GetResourceManager()->FbxModelResouece("ganfaul"));
	player->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.3f);
	player->GetComponent<TransformComponent>()->GetTransform()->SetPosition({ -10,10,-500 });
	player->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_Y(180);
	player->GetComponent<MeshObject>()->SetRasterizeState(RasterizeState::Cull_Back_CCW_True);
	player->GetComponent<FbxModelComponent>()->SwitchAnimation(2, true); // 0,1 T_Pose 2 Walk 3 Run 4 Attack
	player->SetEntityName("Player");

	const char* ground_filename = "./Data/Assets/Bin/ground.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
	std::shared_ptr<FbxModelResource> ground_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), ground_filename, "./Data/Assets/FBX/ground/");
	auto* ground = AddEntity();
	ground->AddComponent<MeshObject>();
	ground->AddComponent<FbxModelComponent>(ground_resource);
	ground->GetComponent<TransformComponent>()->GetTransform()->SetScale(DirectX::XMFLOAT3{ 5,5,0.1f });
	ground->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_X(90);
	ground->GetComponent<MeshObject>()->SetSamplerState(RenderingSampleState::Linear_Wrap);
	ground->SetEntityName("Ground");

	//const char* house_1_filename = "./Data/Assets/Bin/houses/house_1.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
	//std::shared_ptr<FbxModelResource> house_1_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), house_1_filename, "./Data/Assets/FBX/houses/house1/");
	//auto* house_1 = AddEntity();
	//house_1->AddComponent<MeshObject>();
	//house_1->AddComponent<FbxModelComponent>(house_1_resource);
	//house_1->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition({ 300,0,-500 });
	//house_1->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.3f);
	//house_1->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_X(-90);
	////house_1->GetComponent<MeshObject>()->SetSamplerState(RenderingSampleState::Linear_Wrap);
	//house_1->SetEntityName("House_1");

	//const char* house_2_filename = "./Data/Assets/Bin/houses/house_2.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
	//std::shared_ptr<FbxModelResource> house_2_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), house_2_filename, "./Data/Assets/FBX/houses/house2/");
	//auto* house_2 = AddEntity();
	//house_2->AddComponent<MeshObject>();
	//house_2->AddComponent<FbxModelComponent>(house_2_resource);
	//house_2->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition({ 0,0,0 });
	//house_2->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.25f);
	//house_2->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_X(-90);
	////house_1->GetComponent<MeshObject>()->SetSamplerState(RenderingSampleState::Linear_Wrap);
	//house_2->SetEntityName("House_2");

	//auto* house_3 = AddEntity();
	//house_3->AddComponent<MeshObject>();
	//house_3->AddComponent<FbxModelComponent>(house_2_resource);
	//house_3->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition({ -400,0,-500 });
	//house_3->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.25f);
	//house_3->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_X(-90);
	////house_1->GetComponent<MeshObject>()->SetSamplerState(RenderingSampleState::Linear_Wrap);
	//house_3->SetEntityName("House_3");

	//const char* stone_1_filename = "./Data/Assets/Bin/stones/stone_1.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
	//std::shared_ptr<FbxModelResource> stone_1_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), stone_1_filename, "./Data/Assets/FBX/stones/stone_1/");
	//auto* stone_1 = AddEntity();
	//stone_1->AddComponent<MeshObject>();
	//stone_1->AddComponent<FbxModelComponent>(stone_1_resource);
	//stone_1->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition({ -300,0,500 });
	//stone_1->GetComponent<TransformComponent>()->GetTransform()->SetScale({1,3.7,1});
	//stone_1->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation({ -90,-127,0 });
	////house_1->GetComponent<MeshObject>()->SetSamplerState(RenderingSampleState::Linear_Wrap);
	//stone_1->SetEntityName("stone_1");

	//const char* stone_2_filename = "./Data/Assets/Bin/stones/stone_2.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
	//std::shared_ptr<FbxModelResource> stone_2_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), stone_2_filename, "./Data/Assets/FBX/stones/stone_2/");
	//auto* stone_2 = AddEntity();
	//stone_2->AddComponent<MeshObject>();
	//stone_2->AddComponent<FbxModelComponent>(stone_1_resource);
	//stone_2->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition({ 700,0,500 });
	//stone_2->GetComponent<TransformComponent>()->GetTransform()->SetScale({ 1,3.45f,0.8f });
	//stone_2->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation({ -90,-60,0 });
	////house_1->GetComponent<MeshObject>()->SetSamplerState(RenderingSampleState::Linear_Wrap);
	//stone_2->SetEntityName("stone_2");

	//auto* mesh_particle = AddEntity();
	//mesh_particle->AddComponent<MeshObject>();
	//mesh_particle->AddComponent<TestMeshParticle>();
}

void SceneTitle::UnInitializeScene()
{
	sound_resource->GetBGMSound(BGM::Title)->Stop();
}

void SceneTitle::UpdateScene(const float delta_time)
{
	if (Locator::GetInput()->Keyboard().GetState(Keycode::D1) == ButtonState::Press)
	{
		next_scene = std::make_shared<SceneGame>();
	}

}

void SceneTitle::RenderImGui()
{
	if (select_game_mode.test(static_cast<int>(SelectGameMode::TraininigMode)))
	{
		ImGui::Text("Select GameMode : Training Mode");
	}

	if (select_game_mode.test(static_cast<int>(SelectGameMode::MainGame)))
	{
		ImGui::Text("Select GameMode : Main Game");
	}
}