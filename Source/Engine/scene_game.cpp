#include "scene_game.h"

#include <iostream>

// Test
#include <GamePad.h>
#include <memory>

#include "collision_component.h"
#include "ecs.h"
#include "fbx_model_component.h"
#include "geometric_primitive_component.h"
#include "input_manager.h"
#include "input_device.h"
#include "obj_model_component.h"
#include "my_effekseer.h"
#include "sphere_collision_component.h"
#include "string_helper.h"
#include "scene_title.h"
#include "scene_load.h"
#include "transform_component.h"
#include "camera_operation.h"

#include "imgui.h"

// Á‚·
#include "test_effekseer.h"

using namespace shader;

void SceneGame::InitializeScene()
{
	current_scene = SceneType::Game;

	//-- View --//
	view->SetCameraPos(XMFLOAT3(.0f, 1000.0f, -1000.0f), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));
}

void SceneGame::UpdateScene(const float delta_time)
{
	//if (PadInput::GetInstance()->IsInput(0,InputType::GamePad::Start,PadInput::PadState::Press)
	//	|| KeyInput::GetInstance().KeyTrg(static_cast<int>(InputType::Key::KEY_BACK)))
	//{
	//	is_paused = !is_paused;

	//	GetSoundResourceManager()->GetSystemSound(System_SE::MenuCursor)->Play(false);
	//}

	const SimpleMath::Vector3 angle{ -43.902f,-29.268f,1.00f };

	SimpleMath::Vector3 vec = { view->GetCameraFront().x,view->GetCameraFront().y,view->GetCameraFront().z };
	SimpleMath::Matrix mat;
	mat = mat.CreateFromYawPitchRoll(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z));

	vec = XMVector3TransformCoord(vec, mat);

	light->SetLightDir({ vec.x ,vec.y,vec.z,0 });
}

void SceneGame::UnInitializeScene()
{
	sound_resource->GetBGMSound(BGM::Game)->Stop();
}