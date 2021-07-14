#include <iostream>
#include <fstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "model_data.h"
#include "build_fbx.h"

int main()
{
	// Ganfaul
	//BulidFBX build_fbx("../../Data/Assets/FBX/ganfaul/character.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/walk.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/run.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/attack.fbx");
	//build_fbx.Export("../../Data/Assets/Bin/ganfaul.bin");

	//BulidFBX bunny_fbx("../../Data/Assets/FBX/stanford_bunny.fbx");
	//bunny_fbx.Export("../../Data/Assets/Bin/stanford_bunny.bin");

	//BulidFBX build_fbx("../../Data/Assets/FBX/Building/rpgpp_lt_building_01.fbx");
	//build_fbx.Export("../../Data/Assets/Bin/rpgpp_lt_building_01.bin");

	//-- House --//
	//BulidFBX build_fbx("../../Data/Assets/FBX/stones/stone_3/stone_3.fbx");
	//build_fbx.Export("../../Data/Assets/Bin/stones/stone_3.bin");

	//-- RPG Character --//
	//BulidFBX build_fbx("../../Data/Assets/FBX/RPG-Character/RPG-Character.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Idle.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Run-Forward.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Attack-L1.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Roll-Backward.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Stunned.FBX");
	//build_fbx.Export("../../Data/TestData/RPG-Character/RPG-Character.bin");

	//-- Robot --//
	//BulidFBX robot_fbx("../../Data/Assets/FBX/Robot/Robo_L.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Idle_ver_B.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Walk_ver_B_Front.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Walk_ver_B_Back.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Attack_3Combo_1.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Attack_3Combo_2.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Attack_3Combo_3.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Run_ver_B.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Dash_Attack_ver_B.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Damage_Front_Big_ver_A.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Damage_Die.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Damage_Die_Loop.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Revenge_Guard_Start.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Revenge_Guard_Loop.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Revenge_Guard_End.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Evade_Front.fbx");
	//robot_fbx.AddAnimation("../../Data/Assets/FBX/Robot/Animations/Evade_Back.fbx");

	//robot_fbx.Export("../../Data/Assets/Bin/Robo_L.bin");

	////-- Cube --//
	//BulidFBX cube("../../Data/Assets/FBX/Geometric_Primitive/cube_maya.fbx");
	//cube.Export("../../Data/Assets/Bin/cube.bin");

	////-- Sphere --//
	//BulidFBX sphere("../../Data/Assets/FBX/Geometric_Primitive/sphere_maya.fbx");
	//sphere.Export("../../Data/Assets/Bin/sphere.bin");

	//-- Capsule --//
	BulidFBX capsule("../../Data/Assets/FBX/Geometric_Primitive/capsule_maya.fbx");
	capsule.Export("../../Data/Assets/Bin/capsule.bin");

	//getchar();
	return 0;
}