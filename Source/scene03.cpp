#include "scene.h"

#include <memory>

//#include "collision_component.h"
//#include "ecs.h"
//#include "component_list.h"
//#include "fbx_model_component.h"
//#include "geometric_primitive_component.h"
//#include "input_manager.h"
//#include "input_device.h"
//#include "obj_model_component.h"
//#include "sphere_collision_component.h"
//#include "scene_title.h"
//#include "scene_load.h"
//#include "transform_component.h"

#include "locator.h"

// For test
#include "test_animation_character.h"

#include "imgui.h"

using namespace shader;

void Scene03::InitializeScene()
{
	//-- View --//
	view->SetViewInfo(XMFLOAT3(.0f, 100.0f, -100.0f), XMFLOAT3(.0f, .0f, .0f), XMFLOAT3(.0f, 1.0f, .0f));
}

void Scene03::UpdateScene(const float delta_time)
{
	//if (Locator::GetInput()->Keyboard().GetState(Keycode::Enter) == ButtonState::Press)
	//{
	//	next_scene = std::make_shared<SceneLoad>(new SceneTitle);
	//}

	//if(pad_input::IsInput(0, PadInput::ButtonType::Left_DPad, PadInput::PadState::Press))
	//{
	//	int a = 0;
	//	a = 0;

	//	ImGui::Begin("Press");
	//	ImGui::End();
	//}
	//if (pad_input::IsInput(0, PadInput::ButtonType::Right_DPad, PadInput::PadState::Hold))
	//{
	//	int a = 0;
	//	a = 0;

	//	pad_input::SetVibration(0, 0.5f);

	//	ImGui::Begin("Hold");
	//	ImGui::End();
	//}
	//if (pad_input::IsInput(0, PadInput::ButtonType::Right_DPad, PadInput::PadState::Release))
	//{
	//	int a = 0;
	//	a = 0;

	//	pad_input::SetVibration(0, 0.0f);

	//	ImGui::Begin("Relase");
	//	ImGui::End();
	//}
	//if (pad_input::IsInput(0, PadInput::ButtonType::Left_DPad, PadInput::PadState::Up))
	//{
	//	int a = 0;
	//	a = 0;


	//	ImGui::Begin("Up");
	//	ImGui::End();
	//}

	//float val = pad_input::GetAnalogStickVal(0, PadInput::AnalogStickType::Left_Y_Thumb);

	//ImGui::Begin("Value");
	//ImGui::Text("Val %f", val);
	//ImGui::End();
}

void Scene03::UnInitializeScene()
{

}