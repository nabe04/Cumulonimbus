#include "resource_manager.h"

#include "locator.h"
#include "fbx_model_resource.h"

void ResourceManager::Initialize(ID3D11Device* device)
{
	{// FBXモデル読込み
		// Bunny
		const char* bunny = "./Data/Assets/Bin/stanford_bunny.bin";
		std::shared_ptr<FbxModelResource> bunny_resource = std::make_shared<FbxModelResource>(device, bunny, " ");
		AddFBXData(bunny_resource);
		//const char* box = "./Data/Assets/Bin/box.bin";
		//std::shared_ptr<FbxModelResource> box_resource = std::make_shared<FbxModelResource>(device, box, " ");
		//AddFBXData(box_resource);

		// Ganfaul
		const char* ganfaul = "./Data/Assets/Bin/ganfaul.bin";
		std::shared_ptr<FbxModelResource> ganfaul_resource = std::make_shared<FbxModelResource>(device, ganfaul, "./Data/Assets/FBX/ganfaul/character.fbm/");
		AddFBXData(ganfaul_resource);

		// Robot
		const char* robo_filename = "./Data/Assets/Bin/Robo_L.bin";
		std::shared_ptr<FbxModelResource> robo_resource = std::make_shared<FbxModelResource>(device, robo_filename, "./Data/Assets/FBX/Robot/");
		AddFBXData(robo_resource);

		// Cube
		const char* cube_filename  = "./Data/Assets/Bin/cube.bin";  // "./Data/Assets/FBX/stage/stage.fbm/"
		std::shared_ptr<FbxModelResource> cube_resource = std::make_shared<FbxModelResource>(device, cube_filename, " ");
		AddFBXData(cube_resource);

	}

	{// Texture読込み
		const char* no_image = "./Data/Assets/Textures/no_image_256.png";
		AddTextureData(std::make_shared<TextureResource>(device, no_image));

		const char* coffee  = "./Data/Assets/Textures/coffee_256.png";
		std::shared_ptr<TextureResource> coffee_resource = std::make_shared<TextureResource>(device, coffee);
		AddTextureData(coffee_resource);

		const char* uv_checker = "./Data/Assets/Textures/uv_checker.png";
		AddTextureData(std::make_shared<TextureResource>(device, uv_checker));
	}
}