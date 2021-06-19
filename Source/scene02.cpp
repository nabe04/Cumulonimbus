#include "scene.h"


void Scene02::InitializeScene()
{
	/*auto* fillTexture = AddEntity(UpdateOrder::FillTexture);
	fillTexture->SetEntityName("Fill Texture");
	fillTexture->AddComponent<FillTexture2DComponent>(GetFramework()->GetDevice(), XMFLOAT4(.0f, .0f, .0f, 1.0f));*/

	/*auto* test2D = AddEntity();
	test2D->SetEntityName("Test 2D Texture");
	test2D->AddComponent<Texture2DComponent>(GetFramework()->GetDevice(), "./Data/Textures/summer_wars.png", PivotType::LeftTop, 0U, 0U, 1639U, 1040U);
	test2D->GetComponent<Texture2DComponent>()->Resize(g_screen_width, g_screen_height);*/

	//auto* light = AddEntity(UpdateOrder::LightMap);
	//light->SetEntityName("Light Texture");
	//light->AddComponent<Texture2DComponent>(GetFramework()->GetDevice(), "./Data/Textures/light_map.png", PivotType::LeftTop, 0U, 0U, 256U, 256U);
	////light->GetComponent<Texture2DComponent>()->Resize(g_screen_width, g_screen_height);

	//auto* test2DChar = AddEntity();
	//test2DChar->SetEntityName("Test 2D Char");
	//test2DChar->AddComponent<Texture2DComponent>(GetFramework()->GetDevice(), "./Data/Textures/char.png", PivotType::LeftTop, 0U, 0U, 256U, 320U);
	//test2DChar->GetComponent<Texture2DComponent>()->CreateAnim(0U, 0U, 32U, 64U, 8U, 5U);
	//test2DChar->GetComponent<Texture2DComponent>()->SetAnimation(0, 0);


	//-- View --//
	view->SetCameraPos(XMFLOAT3(.0f, .0f, -5.0f), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));
}

void Scene02::UpdateScene(const float delta_time)
{

}

void Scene02::UnInitializeScene()
{

}
