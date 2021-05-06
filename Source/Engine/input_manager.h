#pragma once
//
//#include <map>
//#include <tuple>
//#include <utility>
//
//#include "input_device.h"
//
//namespace key_input
//{
//	void KeyUpdate();
//	bool State(InputType::Key key);
//	bool Trg(InputType::Key keyType);
//}
//
//namespace mouse_input
//{
//	void MouseUpdate();
//	bool OnMouseDown(InputType::Mouse buttonType);
//	bool OnMouseDownState(InputType::Mouse buttonType);
//	bool OnMouseUp(InputType::Mouse buttonType);
//	int  GetCursorPosX();
//	int  GetCursorPosY();
//	int  GetMouseVelocityX();
//	int  GetMouseVelocityY();
//}
//
//namespace pad_input
//{
//	void Initialize();
//	void Update();
//	void Reset();
//	bool IsInput(u_int controller_index, InputType::GamePad button_type, PadInput::PadState state);
//	float GetAnalogStickVal(u_int controller_index, PadInput::AnalogStickType stick_type, bool is_dead_zone = true);
//	void SetVibration(u_int controller_index, float vibration_val, bool is_left = true);
//}
//
//namespace pad_link
//{// GamePadにキーボードとマウスの入力を連動
//	struct PadLink
//	{
//		//PadLink();
//		std::multimap<InputType::GamePad, std::tuple<InputType::Key,InputType::Mouse>> pad_map;
//
//		// GamePadの登録
//		void Register(InputType::GamePad pad_type, InputType::Key key_type);
//		void Register(InputType::GamePad pad_type, InputType::Mouse mouse_type);
//
//		bool Trg(InputType::GamePad pad_type, int controller_index);
//		bool State(InputType::GamePad pad_type, int controller_index);
//	};
//}
//
