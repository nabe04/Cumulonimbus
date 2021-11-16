//#include "resource_manager.h"
//
//#include "locator.h"
//#include "fbx_model_resource.h"
//
//void ResourceManager::Initialize(ID3D11Device* device)
//{
//	{// FBXÉÇÉfÉãì«çûÇ›
//
//
//		//// Bunny
//		//const char* bunny = "./Data/Assets/Bin/stanford_bunny.bin";
//		//std::shared_ptr<FbxModelResource> bunny_resource = std::make_shared<FbxModelResource>(device, bunny, " ");
//		//AddFBXData(bunny_resource);
//		////const char* box = "./Data/Assets/Bin/box.bin";
//		////std::shared_ptr<FbxModelResource> box_resource = std::make_shared<FbxModelResource>(device, box, " ");
//		////AddFBXData(box_resource);
//
//		//// Ganfaul
//		//const char* ganfaul = "./Data/Assets/Bin/ganfaul.bin";
//		//std::shared_ptr<FbxModelResource> ganfaul_resource = std::make_shared<FbxModelResource>(device, ganfaul, "./Data/Assets/FBX/ganfaul/character.fbm/");
//		//AddFBXData(ganfaul_resource);
//
//		//// Robot
//		//const char* robo_filename = "./Data/Assets/Bin/Robo_L.bin";
//		//std::shared_ptr<FbxModelResource> robo_resource = std::make_shared<FbxModelResource>(device, robo_filename, "./Data/Assets/FBX/Robot/");
//		//AddFBXData(robo_resource);
//
//		// Robot
//		const char* robo2_filename = "./Data/Assets/Bin/Robot.bin";
//		std::shared_ptr<FbxModelResource> robo2_resource = std::make_shared<FbxModelResource>(device, robo2_filename, "./Data/Assets/FBX/Robot/");
//		AddFBXData(robo2_resource);
//
//		//// Enemies
//		//// DeepSeaLizard
//		//const char* deep_sea_lizard_filename = "./Data/Assets/Bin/DeepSeaLizard.bin";
//		//std::shared_ptr<FbxModelResource> deep_sea_lizard_resource = std::make_shared<FbxModelResource>(device, deep_sea_lizard_filename, "./Data/Assets/FBX/Enemies/E_00_DeepSeaLizard");
//		//AddFBXData(deep_sea_lizard_resource);
//
//		//// ForestDemon
//		//const char* forest_demon_filename = "./Data/Assets/Bin/ForestDemon.bin";
//		//std::shared_ptr<FbxModelResource> forest_demon_resource = std::make_shared<FbxModelResource>(device, forest_demon_filename, "./Data/Assets/FBX/Enemies/E_02ForestDemon");
//		//AddFBXData(forest_demon_resource);
//
//		//// Lich
//		//const char* lich_filename = "./Data/Assets/Bin/Lich.bin";
//		//std::shared_ptr<FbxModelResource> lich_resource = std::make_shared<FbxModelResource>(device, lich_filename, "./Data/Assets/FBX/Enemies/E_05_Lich");
//		//AddFBXData(lich_resource);
//
//		//const char* slime_filename = "./Data/Assets/Bin/Slime.bin";
//		//std::shared_ptr<FbxModelResource> slime_resource = std::make_shared<FbxModelResource>(device, slime_filename, "./Data/Assets/FBX/Enemies/E_11_Slime");
//		//AddFBXData(slime_resource);
//
//		//// SkeletonWarrior
//		//const char* skeleton_wrrior_filename = "./Data/Assets/Bin/SkeletonWarrior.bin";
//		//std::shared_ptr<FbxModelResource>  skeleton_wrrior_resource = std::make_shared<FbxModelResource>(device, skeleton_wrrior_filename, "./Data/Assets/FBX/Enemies/E_09_SkeletonWarrior");
//		//AddFBXData(skeleton_wrrior_resource);
//
//		// Soldier
//		const char* soldier_filename = "./Data/Assets/Bin/Soldier.bin";
//		std::shared_ptr<FbxModelResource>  soldier_resource = std::make_shared<FbxModelResource>(device, soldier_filename, "./Data/Assets/FBX/Enemies/E_16_Soldier/Textures");
//		AddFBXData(soldier_resource);
//
//		// Stages
//		const char* stage_01_filename = "./Data/Assets/Bin/Stage_01.bin";
//		std::shared_ptr<FbxModelResource>  stage_01_resource = std::make_shared<FbxModelResource>(device, stage_01_filename, "./Data/Assets/FBX/Stages/Stage_01/Textures");
//		AddFBXData(stage_01_resource);
//
//		const char* stage_01_collisionn_filename = "./Data/Assets/Bin/Stage_01_Collision.bin";
//		std::shared_ptr<FbxModelResource>  stage_01_collision_resource = std::make_shared<FbxModelResource>(device, stage_01_collisionn_filename, "./Data/Assets/FBX/Stages/Stage_01/Stage_01_For_Collision.fbm");
//		AddFBXData(stage_01_collision_resource);
//
//		// Cube
//		const char* cube_filename  = "./Data/Assets/Bin/cube.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
//		std::shared_ptr<FbxModelResource> cube_resource = std::make_shared<FbxModelResource>(device, cube_filename, " ");
//		AddFBXData(cube_resource);
//
//		// Sphere
//		const char* sphere_filename = "./Data/Assets/Bin/sphere.bin";
//		std::shared_ptr<FbxModelResource> sphere_resource = std::make_shared<FbxModelResource>(device, sphere_filename, " ");
//		AddFBXData(sphere_resource);
//
//		// Capsule
//		const char* capsule_filename = "./Data/Assets/Bin/capsule.bin";
//		std::shared_ptr<FbxModelResource> capsule_resource = std::make_shared<FbxModelResource>(device, capsule_filename, " ");
//		AddFBXData(capsule_resource);
//	}
//
//	{// Textureì«çûÇ›
//		const char* no_image = "./Data/Assets/Textures/no_image_256.png";
//		AddTextureData(std::make_shared<TextureResource>(device, no_image));
//
//		const char* coffee  = "./Data/Assets/Textures/coffee_256.png";
//		std::shared_ptr<TextureResource> coffee_resource = std::make_shared<TextureResource>(device, coffee);
//		AddTextureData(coffee_resource);
//
//		const char* uv_checker = "./Data/Assets/Textures/uv_checker.png";
//		AddTextureData(std::make_shared<TextureResource>(device, uv_checker));
//	}
//}