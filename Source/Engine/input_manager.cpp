//#include "input_manager.h"
//
//#include <utility>
//#include <typeinfo>
//
//
///////////////////////////////////////////////////////////////
////
////			KeyInput
////
///////////////////////////////////////////////////////////////
//namespace key_input
//{
//	void KeyUpdate()
//	{
//		KeyInput::GetInstance().KeyUpdate();
//	}
//
//	bool State(InputType::Key keyType)
//	{
//		return KeyInput::GetInstance().KeyState(static_cast<int>(keyType));
//	}
//
//	bool Trg(InputType::Key keyType)
//	{
//		return KeyInput::GetInstance().KeyTrg(static_cast<int>(keyType));
//	}
//}
//
///////////////////////////////////////////////////////////////
////
////			MouseInput
////
///////////////////////////////////////////////////////////////
//
//
//namespace mouse_input
//{
//	// �X�V
//	void MouseUpdate()
//	{
//		MouseInput::GetInstance().MouseUpdate();
//	}
//
//	// �N���b�N���ꂽ�u��
//	bool OnMouseDown(InputType::Mouse buttonType)
//	{
//		return MouseInput::GetInstance().OnMouseDown(buttonType);
//	}
//
//	// �N���b�N����Ă����
//	bool OnMouseDownState(InputType::Mouse buttonType)
//	{
//		return MouseInput::GetInstance().OnMouseDownState(buttonType);
//	}
//
//	// �N���b�N�������ꂽ�u��
//	bool OnMouseUp(InputType::Mouse buttonType)
//	{
//		return MouseInput::GetInstance().OnMouseUp(buttonType);
//	}
//
//	// �J�[�\�����W�擾(X)
//	int GetCursorPosX()
//	{
//		return MouseInput::GetInstance().GetCursorPosX();
//	}
//
//	// �J�[�\�����W�擾(Y)
//	int GetCursorPosY()
//	{
//		return MouseInput::GetInstance().GetCursorPosY();
//	}
//
//	// 1�t���[���̃J�[�\���ړ���(X)
//	int GetMouseVelocityX()
//	{
//		return MouseInput::GetInstance().GetMouseVelocityX();
//	}
//
//	// 1�t���[���̃J�[�\���ړ���(Y)
//	int GetMouseVelocityY()
//	{
//		return MouseInput::GetInstance().GetMouseVelocityY();
//	}
//}
//
//namespace pad_input
//{
//	void Initialize()
//	{
//		PadInput::GetInstance()->Initialize();
//	}
//
//	void Update()
//	{
//		PadInput::GetInstance()->Update();
//	}
//
//	void Reset()
//	{
//		PadInput::GetInstance()->Reset();
//	}
//
//	bool IsInput(u_int controller_index, InputType::GamePad button_type, PadInput::PadState state)
//	{
//		return PadInput::GetInstance()->IsInput(controller_index, button_type, state);
//	}
//
//	//return : �A�i���O�X�e�B�b�N�̒l��(0 �` 1)�̊ԂŕԂ�
//	float GetAnalogStickVal(u_int controller_index, PadInput::AnalogStickType stick_type, bool is_dead_zone)
//	{
//		return PadInput::GetInstance()->GetAnalogStickVal(controller_index, stick_type, is_dead_zone);
//	}
//
//	void SetVibration(u_int controller_index, float vibration_val, bool is_left)
//	{
//		PadInput::GetInstance()->SetVibration(controller_index, vibration_val, is_left);
//	}
//}
//
//namespace pad_link
//{
//	void PadLink::Register(InputType::GamePad pad_type, InputType::Key key_type)
//	{
//		pad_map.insert(std::make_pair(pad_type, std::make_tuple(key_type, InputType::Mouse::None)));
//	}
//
//	void PadLink::Register(InputType::GamePad pad_type, InputType::Mouse mouse_type)
//	{
//		pad_map.insert(std::make_pair(pad_type, std::make_tuple(InputType::Key::None, mouse_type)));
//	}
//
//
//	bool PadLink::Trg(InputType::GamePad pad_type, int controller_index)
//	{
//		bool flg = false;
//
//		if (!pad_map.contains(pad_type))
//			assert(!"Not found registered gamepad");
//		if (pad_input::IsInput(controller_index, pad_type, PadInput::PadState::Press))
//			return true;
//
//		auto it = pad_map.find(pad_type);
//		if (it == pad_map.end())
//			return false;
//
//		for (int i = 0; i < pad_map.count(pad_type); ++i)
//		{
//
//			if(typeid(std::get<0>(it->second)) == typeid(InputType::Key))
//			{// Input test of Keyboard
//				auto key_type = std::get<0>(it->second);
//
//				bool is_test = true;
//				if (key_type == InputType::Key::None)
//				{
//					is_test = false;
//				}
//
//				if (is_test)
//				{
//					if (key_input::Trg(key_type))
//						return true;
//				}
//			}
//
//			if (typeid(std::get<1>(it->second)) == typeid(InputType::Mouse))
//			{// Input test of Mouse
//				auto mouse_type = std::get<1>(it->second);
//
//				bool is_test = true;
//				if (std::get<1>(pad_map.find(pad_type)->second) == InputType::Mouse::None)
//				{
//					is_test = false;
//				}
//
//				if (is_test)
//				{
//					if (mouse_input::OnMouseDown(mouse_type))
//						return true;
//				}
//			}
//
//			++it;
//		}
//
//		return flg;
//	}
//
//	bool PadLink::State(InputType::GamePad pad_type, int controller_index)
//	{
//		bool flg = false;
//
//		if (!pad_map.contains(pad_type))
//			assert(!"Not found registered gamepad");
//		if (pad_input::IsInput(controller_index, pad_type, PadInput::PadState::Hold))
//			return true;
//
//		auto it = pad_map.find(pad_type);
//		if (it == pad_map.end())
//			return false;
//
//		for (int i = 0; i < pad_map.count(pad_type); ++i)
//		{
//			{// Input test of Keyboard
//				auto key_type = std::get<0>(it->second);
//				if (key_type == InputType::Key::None)
//					flg = false;
//
//				if (key_input::State(key_type))
//					return true;
//			}
//
//			{// Input test of Mouse
//				auto mouse_type = std::get<1>(it->second);
//				if (std::get<1>(pad_map.find(pad_type)->second) == InputType::Mouse::None)
//					flg = false;
//
//				if (mouse_input::OnMouseDownState(mouse_type))
//					return true;
//			}
//
//			++it;
//		}
//
//		return flg;
//	}
//}
