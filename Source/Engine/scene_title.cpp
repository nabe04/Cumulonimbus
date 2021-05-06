#include "scene_title.h"

#include "imgui.h"

#include "input_manager.h"
#include "scene_load.h"
#include "scene_game.h"
#include "scene_tutorial.h"

#include "transform.h"
#include "transform_component.h"
#include "mesh_object.h"
#include "geometric_primitive_component.h"
#include "fbx_model_component.h"

#include "sprite_object.h"
#include "anim_sprite.h"
#include "sky_box.h"

#include "test_mesh_particle.h"
#include "rotation_sample.h"

#include "locator.h"

#include "sound_resource.h"

void SceneTitle::InitializeScene()
{
	current_scene = SceneType::Title;
	scene_name = "Scene_Title";

	sound_resource->GetBGMSound(BGM::Title)->Play(true);

	for (int i = 0; i < static_cast<int>(SelectGameMode::End); ++i)
	{
		select_game_mode.reset(i);
	}
	select_game_mode.set(static_cast<int>(SelectGameMode::TraininigMode));	// 常にどれかのビットをセットしておく(初期はトレーニングモード)

//-- Light --//
	light->SetLightDir({ .0f,-.6f,1.f ,1.f });

	//-- View --//
	view->SetCameraPos(XMFLOAT3(.0f, 50.0, -100.0f), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));
	camera_work->SetPositon({ 0,10,-45 });

	//const char* coffee_filename = "coffee_256";
	//auto* coffee_tex = AddEntity();
	//coffee_tex->AddComponent<SpriteObject>();
	//coffee_tex->AddComponent<SpriteComponent>(GetFramework()->GetDevice(), coffee_filename, PivotType::RightTop,
	//	0, 0,
	//	256, 256);
	//coffee_tex->SetEntityName("Coffee");

	//const char* uv_chekcer_filename = "uv_checker";
	//auto* uv_chekcer = AddEntity();
	//uv_chekcer->AddComponent<SpriteObject>();
	//uv_chekcer->AddComponent<AnimSpriteComponent>(GetFramework()->GetDevice(), uv_chekcer_filename, PivotType::Center,
	//	0, 0,
	//	256, 256,
	//	8, 8);
	//uv_chekcer->GetComponent<TransformComponent>()->GetTransform()->SetPosition({ 500,500,0 });
	//uv_chekcer->SetEntityName("UV_Chacker");

	const char* sky_filename = "./Data/Assets/cubemap/skybox";
	auto* sky_box = AddEntity(UpdateOrder::Default, EntityTag::Sky_Map);
	sky_box->AddComponent<SkyBox>(GetFramework()->GetDevice(), sky_filename);
	sky_box->GetComponent<TransformComponent>()->GetTransform()->SetScale(3);
	sky_box->SetEntityName("Sky_Box");

	auto* stage = AddEntity(UpdateOrder::Default);
	stage->AddComponent<MeshObject>();
	stage->AddComponent<GeomPrimComponent>(GeomPrimType::Cube);
	stage->GetComponent<TransformComponent>()->GetTransform()->SetScale({ 100.0f,0.1f,100.0f });
	stage->SetEntityName("Stage");

	auto* sample01 = AddEntity(UpdateOrder::Default, EntityTag::Bunny);
	sample01->AddComponent<MeshObject>();
	sample01->GetComponent<MeshObject>()->SetShaderState(shader::MeshShaderTypes::Metal);
	sample01->AddComponent<FbxModelComponent>(Locator::GetResourceManager().FbxModelResouece("stanford_bunny"));
	sample01->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.01f);
	sample01->GetComponent<TransformComponent>()->GetTransform()->SetPosition({ 0,4,30 });
	sample01->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.01f);
	sample01->AddComponent<SampleRotation>();
	sample01->SetEntityName("Sample03");

	//const char* bunny_filename = "./Data/Assets/Bin/stanford_bunny.bin";
	//std::shared_ptr<FbxModelResource> bunny_resource = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), bunny_filename, " ");
	//auto* bunny = AddEntity(UpdateOrder::Default,EntityTag::Bunny);
	//bunny->AddComponent<MeshObject>();
	//bunny->AddComponent<FbxModelComponent>(Locator::GetResourceManager().FbxModelResouece("stanford_bunny"));
	//bunny->GetComponent<TransformComponent>()->GetTransform()->SetScale(0.01f);
	//bunny->GetComponent<TransformComponent>()->GetTransform()->SetPositionY(5);
	//bunny->GetComponent<MeshObject>()->SetRasterizeState(RasterizeState::Cull_Back);
	//bunny->SetEntityName("Bunny");

	auto* mesh_particle = AddEntity();
	mesh_particle->AddComponent<MeshObject>();
	mesh_particle->AddComponent<TestMeshParticle>();
}

void SceneTitle::UnInitializeScene()
{
	sound_resource->GetBGMSound(BGM::Title)->Stop();
}

void SceneTitle::UpdateScene(const float delta_time)
{
	if (Locator::GetInput().Keyboard().GetState(Keycode::Enter) == ButtonState::Press)
	{
		//next_scene = std::make_shared<SceneLoad>(new SceneTutorial);
	}

}

void SceneTitle::RenderImgui()
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