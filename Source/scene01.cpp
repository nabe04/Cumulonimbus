//#include "scene.h"
//
//#include <iostream>
//
////#include "ecs.h"
////#include "component_list.h"
////#include "string_helper.h"
////#include "transform_component.h"
////#include "geometric_primitive_component.h"
////#include "obj_model_component.h"
////#include "fbx_model_component.h"
//
//#include "imgui.h"
//
//void Scene01::InitializeScene()
//{
//	//auto* cubeComp = AddEntity();
//	//cubeComp->SetEntityName("Cube");
//	//cubeComp->AddComponent<ActorComponent>();
//	//cubeComp->AddComponent<GeomPrimComponent>(GetFramework()->GetDevice(), GeomPrimType::Cube);
//	//cubeComp->GetComponent<ActorComponent>()->SetShaderState(shader::MeshShaderTypes::Metal);
//
//	//auto* objBunny = AddEntity();
//	//objBunny->SetEntityName("Bunny");
//	//objBunny->AddComponent<ActorComponent>();
//	//objBunny->AddComponent<ObjModelComponent>(GetFramework()->GetDevice(), "./Data/Assets/OBJ/Stanford_bunny/stanford-bunny.obj");
//	//objBunny->GetComponent<ActorComponent>()->SetShaderState(shader::MeshShaderTypes::Metal);
//	//objBunny->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition(XMFLOAT3(-1, 0, 0));
//	//objBunny->GetComponent<TransformComponent>()->GetTransform()->SetScale(XMFLOAT3{10,10,10});
//
//	//auto* objInc = AddEntity();
//	//objInc->SetEntityName("Incredible");
//	//objInc->AddComponent<ActorComponent>();
//	//objInc->AddComponent<ObjModelComponent>(GetFramework()->GetDevice(), "./Data/Assets/OBJ/Mr.Inc/Mr.Incredible.obj");
//	//objInc->GetComponent<ActorComponent>()->SetShaderState(shader::MeshShaderTypes::Phong);
//	//objInc->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition(XMFLOAT3(-1, 1, 0));
//	//objInc->GetComponent<TransformComponent>()->GetTransform()->SetScale(XMFLOAT3{ 0.3f,0.3f,0.3f });
//
//	//auto* test2D = AddEntity();
//	//test2D->SetEntityName("Test 2D Texture");
//	//test2D->AddComponent<ActorComponent>(ActorType::Actor2D);
//	//test2D->AddComponent<Texture2DComponent>(GetFramework()->GetDevice(), "./Data/Textures/char.png", PivotType::LeftTop, 0U, 0U, 256U, 320U);
//	//test2D->GetComponent<Texture2DComponent>()->CreateAnim(0U, 0U, 32U, 64U, 8U, 5U);
//	//test2D->GetComponent<Texture2DComponent>()->SetAnimation(0, 0);
//	//test2D->GetComponent<Texture2DComponent>()->Resize(g_screen_width, g_screen_height);
//
//	//const char* fbxCubeFilename = "./Data/Assets/FBX/Cube/003_cube.fbx";
//	//fbxCube = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), fbxCubeFilename, "");
//	//auto* modelFbx = AddEntity();
//	//modelFbx->AddComponent<FbxModelComponent>(fbxCube);
//	//modelFbx->SetEntityName("Model FBX");
//
//	//const char* fbxCharacterFilename = "./Data/Assets/FBX/RPG-Character/RPG-Character.FBX";
//	//resCharacter = std::make_shared<FbxModelResource>(GetFramework()->GetDevice(), fbxCharacterFilename, "");
//	////resCharacter->AddAnimation("./Data/Assets/FBX/RPG-Character/Unarmed/RPG-Character@Unarmed-Idle.fbx");
//	////resCharacter->AddAnimation("./Data/Assets/FBX/RPG-Character/Unarmed/RPG-Character@Unarmed-Turn-Left-90.fbx");
//	////resCharacter->AddAnimation("./Data/Assets/FBX/RPG-Character/Unarmed/RPG-Character@Unarmed-Run-Forward.fbx");
//	//auto* fbxCharacter = AddEntity();
//	//fbxCharacter->AddComponent<ActorComponent>();
//	//fbxCharacter->AddComponent<FbxModelComponent>(resCharacter);
//	//fbxCharacter->SetEntityName("Character FBX");
//	//fbxCharacter->GetComponent<ActorComponent>()->SetShaderState(shader::MeshShaderTypes::Diffuse);
//	//fbxCharacter->GetComponent<TransformComponent>()->GetTransform()->SetScale(XMFLOAT3(0.01f, 0.01f, 0.01f));
//	//fbxCharacter->GetComponent<TransformComponent>()->GetTransform()->SetEyePosition(XMFLOAT3(2, 2, 0));
//	//fbxCharacter->GetComponent<FbxModelComponent>()->SwitchAnimation(3, true);
//
//	//-- Camera --//
//	//main_camera->SetViewInfo(XMFLOAT3(.0f, .0f, -34.0f), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));
//}
//
//void Scene01::UpdateScene(const float delta_time)
//{
//
//}
//
//void Scene01::UnInitializeScene()
//{
//
//}
