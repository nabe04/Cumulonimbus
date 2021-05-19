#include <iostream>
#include <fstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "model_data.h"
#include "build_fbx.h"

int main()
{
	//BulidFBX build_fbx("../../Data/Assets/FBX/ganfaul/character.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/walk.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/run.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/attack.fbx");
	//build_fbx.Export("../../Data/Assets/Bin/ganfaul.bin");

	//BulidFBX build_fbx("../../Data/Assets/FBX/Building/rpgpp_lt_building_01.fbx");
	//build_fbx.Export("../../Data/Assets/Bin/rpgpp_lt_building_01.bin");

	BulidFBX build_fbx("../../Data/Assets/FBX/house/house.fbx");
	build_fbx.Export("../../Data/Assets/Bin/house.bin");

	//-- RPG Character --//
	//BulidFBX build_fbx("../../Data/TestData/RPG-Character/RPG-Character.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Idle.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Run-Forward.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Attack-L1.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Roll-Backward.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Stunned.FBX");
	//build_fbx.Export("../../Data/TestData/RPG-Character/RPG-Character.bin");


	//getchar();
	return 0;
}