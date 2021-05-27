#include "test_animation_character.h"

#include "fbx_model_component.h"
#include "input_manager.h"

#include "scene.h"
#include "input_manager.h"

/**************************************
*
*
*	FBXのアニメーションテスト用
*
*
***************************************/

AnimationCharacterComponent::AnimationCharacterComponent(Entity* entity)
	:ActorComponent{ entity }
{
	auto model = GetEntity()->GetComponent<FbxModelComponent>();

	model->SwitchAnimation(1, true);
}

void AnimationCharacterComponent::NewFrame(const float delta_time)
{

}

void AnimationCharacterComponent::Update(const float delta_time)
{
	auto model = GetEntity()->GetComponent<FbxModelComponent>();

	if (!model)
		return;

	//if (key_input::Trg(InputType::Key::KEY_0))
	//{
	//	model->SwitchAnimation(0, true,0.3f);
	//}
	//else if(key_input::Trg(InputType::Key::KEY_1))
	//{
	//	model->SwitchAnimation(1, true, 0.3f);
	//}
	//else if(key_input::Trg(InputType::Key::KEY_2))
	//{
	//	model->SwitchAnimation(2, true, 0.3f);
	//}
	//else if(key_input::Trg(InputType::Key::KEY_3))
	//{
	//	model->SwitchAnimation(3, true, 0.3f);
	//}
	//else if(key_input::Trg(InputType::Key::KEY_4))
	//{
	//	model->SwitchAnimation(4, true, 0.3f);
	//}
	//else if(key_input::Trg(InputType::Key::KEY_5))
	//{
	//	model->SwitchAnimation(5, true, 0.3f);
	//}

	//static float val_y = 1;
	//static float val_x = 1;
	//if (key_input::State(InputType::Key::KEY_RIGHT))
	//{
	//	if (val_y > 180)
	//	{
	//		val_y -= 360;
	//	}

	//	//val_y++;
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AdjustLocalRotation_Y(val_y);
	//}
	//if (key_input::State(InputType::Key::KEY_LEFT))
	//{
	//	if (val_y < -180)
	//	{
	//		val_y += 360;
	//	}

	//	//val_y--;
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AdjustLocalRotation_Y(-val_y);
	//}

	//auto pad = GetEntity()->GetScene()->GetPadLink();
	//if (pad->State(InputType::GamePad::Up_DPad,0))
	//{
	//	if (val_x > 180)
	//	{
	//		val_x -= 360;
	//	}

	//	//val_x++;
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AdjustLocalRotation_X(val_x);
	//}
	//if (key_input::State(InputType::Key::KEY_DOWN))
	//{
	//	if (val_x < -180)
	//	{
	//		val_x += 360;
	//	}

	//	//val_x--;
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AdjustLocalRotation_X(-val_x);
	//}
	//if (key_input::State(InputType::Key::KEY_L))
	//{
	//	if (val_x < -180)
	//	{
	//		val_x += 360;
	//	}

	//	//val_x--;
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AdjustLocalRotation_Z(val_x);
	//}
	//if (key_input::State(InputType::Key::KEY_K))
	//{
	//	if (val_x < -180)
	//	{
	//		val_x += 360;
	//	}

	//	//val_x--;
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->AdjustLocalRotation_Z(-val_x);
	//}

	//if (key_input::Trg(InputType::Key::KEY_O))
	//{
	//	GetEntity()->GetComponent<TransformComponent>()->GetTransform()->SetWorldRotation_X(90);
	//}

	model->UpdateAnimState(delta_time);
}

void AnimationCharacterComponent::RenderImGui()
{
	ActorComponent::RenderImGui();
}