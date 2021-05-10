#include "scene_game.h"

#include <memory>

#include "fbx_model_component.h"
#include "input_manager.h"
#include "input_device.h"
#include "scene_title.h"


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