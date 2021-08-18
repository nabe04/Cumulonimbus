#include <iostream>
#include <fstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include "model_data.h"
#include "build_fbx.h"

int main()
{
	//// Ganfaul
	//BulidFBX build_fbx("../../Data/Assets/FBX/ganfaul/character.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/walk.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/run.fbx");
	//build_fbx.AddAnimation("../../Data/Assets/FBX/ganfaul/attack.fbx");
	//build_fbx.Export("../../Data/Assets/Bin/ganfaul.bin");

	// Stanford bunny
	//BulidFBX bunny_fbx("../../Data/Assets/FBX/stanford_bunny.fbx");
	//bunny_fbx.Export("../../Data/Assets/Bin/stanford_bunny.bin");

	//-- RPG Character --//
	//BulidFBX build_fbx("../../Data/Assets/FBX/RPG-Character/RPG-Character.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Idle.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Run-Forward.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Attack-L1.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Roll-Backward.FBX");
	//build_fbx.AddAnimation("../../Data/TestData/RPG-Character/Unarmed/RPG-Character@Unarmed-Stunned.FBX");
	//build_fbx.Export("../../Data/TestData/RPG-Character/RPG-Character.bin");

	////-- Robot --//
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

	//////-- Cube --//
	//BulidFBX cube("../../Data/Assets/FBX/Geometric_Primitive/cube_maya.fbx");
	//cube.Export("../../Data/Assets/Bin/cube.bin");

	//////-- Sphere --//
	//BulidFBX sphere("../../Data/Assets/FBX/Geometric_Primitive/sphere_maya.fbx");
	//sphere.Export("../../Data/Assets/Bin/sphere.bin");

	////-- Capsule --//
	//BulidFBX capsule("../../Data/Assets/FBX/Geometric_Primitive/capsule_maya.fbx");
	//capsule.Export("../../Data/Assets/Bin/capsule.bin");

	// Robo
	//BulidFBX robot2_fbx("../../Data/Assets/FBX/Robo/Robo_L.fbx");
	//robot2_fbx.Export("../../Data/Assets/Bin/Robo.bin");

	// E_00_DeepSeaLizard
	//BulidFBX e_00("../../Data/Assets/FBX/Enemies/E_00_DeepSeaLizard/DeepSeaLizard.fbx");
	//e_00.Export("../../Data/Assets/Bin/DeepSeaLizard.bin");

	//// E_01_DemonWarrior
	//BulidFBX e_01("../../Data/Assets/FBX/Enemies/E_01_DemonWarrior/DemonWarrior.fbx");
	//e_01.Export("../../Data/Assets/Bin/DemonWarrior.bin");

	//// E_02ForestDemon
	//BulidFBX e_02("../../Data/Assets/FBX/Enemies/E_02ForestDemon/ForestDemon.fbx");
	//e_02.Export("../../Data/Assets/Bin/ForestDemon.bin");

	// E_03_Hellguard
	//BulidFBX e_03("../../Data/Assets/FBX/Enemies/E_03_Hellguard/Hellguard.fbx");
	//e_03.Export("../../Data/Assets/Bin/Hellguard.bin");

	//// E_04_Imp
	//BulidFBX e_04("../../Data/Assets/FBX/Enemies/E_04_Imp/Imp.fbx");
	//e_04.Export("../../Data/Assets/Bin/Imp.bin");

	//// E_05_Lich
	//BulidFBX e_05("../../Data/Assets/FBX/Enemies/E_05_Lich/Lich.fbx");
	//e_05.Export("../../Data/Assets/Bin/Lich.bin");

	//// E_06_Mushroom
	//BulidFBX e_06("../../Data/Assets/FBX/Enemies/E_06_Mushroom/Mushroom.fbx");
	//e_06.Export("../../Data/Assets/Bin/Mushroom.bin");

	//// E_07_RockGolem
	//BulidFBX e_07("../../Data/Assets/FBX/Enemies/E_07_RockGolem/RockGolem.fbx");
	//e_07.Export("../../Data/Assets/Bin/RockGolem.fbx");

	//// E_08_SkeletonArcher
	//BulidFBX e_08("../../Data/Assets/FBX/Enemies/E_08_SkeletonArcher/SkeletonArcher.fbx");
	//e_08.Export("../../Data/Assets/Bin/SkeletonArcher.bin");

	//// E_09_SkeletonWarrior
	//BulidFBX e_09("../../Data/Assets/FBX/Enemies/E_09_SkeletonWarrior/SkeletonWarrior.fbx");
	//e_09.Export("../../Data/Assets/Bin/SkeletonWarrior.bin");

	//// E_10_SkeletonWizard
	//BulidFBX e_10("../../Data/Assets/FBX/Enemies/E_10_SkeletonWizard/SkeletonWizard.fbx");
	//e_10.Export("../../Data/Assets/Bin/SkeletonWizard.bin");

	//// E_11_Slime
	//BulidFBX e_11("../../Data/Assets/FBX/Enemies/E_11_Slime/Slime.fbx");
	//e_11.Export("../../Data/Assets/Bin/Slime.bin");

	//// E_12_SlimeMan
	//BulidFBX e_12("../../Data/Assets/FBX/Enemies/E_12_SlimeMan/SlimeMan.fbx");
	//e_12.Export("../../Data/Assets/Bin/SlimeMan.bin");

	//// E_13_Vampire
	//BulidFBX e_13("../../Data/Assets/FBX/Enemies/E_13_Vampire/Vampire.fbx");
	//e_13.Export("../../Data/Assets/Bin/Vampire.bin");

	//// E_14_Zombie
	//BulidFBX e_14("../../Data/Assets/FBX/Enemies/E_14_Zombie/Zombie.fbx");
	//e_14.Export("../../Data/Assets/Bin/Zombie.bin");

	// E_15_ZombieHound
	//BulidFBX e_15("../../Data/Assets/FBX/Enemies/E_15_ZombieHound/ZombieHound.fbx");
	//e_15.Export("../../Data/Assets/Bin/ZombieHound.bin");


	// Stage_01
	BulidFBX stage_01("../../Data/Assets/FBX/Stages/Stage_01/Stage_01_3.fbx");
	stage_01.Export("../../Data/Assets/Bin/Stage_01.bin");

	return 0;
}