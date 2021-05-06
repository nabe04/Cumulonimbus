//#include "input_device.h"
//
//#include <GamePad.h>
//#include <Windows.h>
//
//
///////////////////////////////////////////////////////////////
////
////			KeyInput
////
///////////////////////////////////////////////////////////////
//
//////-- ダイレクトインプットの初期化 --////
//HRESULT KeyInput::InitDinput()
//{
//	// 「DirectInput」オブジェクトの作成
//	if (FAILED(DirectInput8Create(GetModuleHandle(NULL),
//		DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_Dinput, NULL)))
//	{
//		return E_FAIL;
//	}
//	// 「DirectInputデバイス」オブジェクトの作成
//	if (FAILED(m_Dinput->CreateDevice(GUID_SysKeyboard, &m_KeyDevice, NULL)))
//	{
//		return E_FAIL;
//	}
//	// デバイスをキーボードに設定
//	if (FAILED(m_KeyDevice->SetDataFormat(&c_dfDIKeyboard)))
//	{
//		return E_FAIL;
//	}
//	// 協調レベルの設定
//	HWND hwnd = GetDesktopWindow();	// ウィンドウハンドル取得
//	if (FAILED(m_KeyDevice->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
//	{
//		return E_FAIL;
//	}
//	// デバイスを「取得」する
//	hr = m_KeyDevice->Acquire();
//	if (FAILED(hr))
//	{
//		return E_FAIL;
//	}
//
//	return S_OK;
//}
//
//void KeyInput::KeyUpdate()
//{
//	hr = m_KeyDevice->Acquire();
//	if ((hr == DI_OK) || (hr == S_FALSE))
//	{
//		m_KeyDevice->GetDeviceState(sizeof(m_KeyInput), &m_KeyInput);
//	}
//}
//
//bool KeyInput::KeyState(int key)
//{
//	if ((hr == DI_OK) || (hr == S_FALSE))
//	{
//		if (m_KeyInput[key] & 0x80)
//			return true;
//	}
//
//	return false;
//}
//
//bool KeyInput::KeyTrg(int keyType)
//{
//	bool flg = false;
//
//	if ((hr == DI_OK) || (hr == S_FALSE))
//	{
//		if ((m_KeyInput[keyType] & 0x80) && !(m_OldKey[keyType] & 0x80))
//		{
//			flg = true;
//		}
//		m_OldKey[keyType] = m_KeyInput[keyType];
//	}
//
//	return flg;
//}
//
///////////////////////////////////////////////////////////////
////
////			MouseInput
////
///////////////////////////////////////////////////////////////
//
//DIMOUSESTATE MouseInput::m_CurrentMouseState;
//DIMOUSESTATE MouseInput::m_PrevMouseState;
//
//HRESULT MouseInput::InitDinput()
//{
//	// 「DirectInput」オブジェクトの作成
//	if (FAILED(DirectInput8Create(GetModuleHandle(NULL),
//		DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_Dinput, NULL)))
//	{
//		return E_FAIL;
//	}
//	// 「DirectInputデバイス」オブジェクトの作成
//	if (FAILED(m_Dinput->CreateDevice(GUID_SysMouse, &m_MouseDevice, NULL)))
//	{
//		return E_FAIL;
//	}
//	// デバイスをキーボードに設定
//	if (FAILED(m_MouseDevice->SetDataFormat(&c_dfDIMouse)))
//	{
//		return E_FAIL;
//	}
//	// 協調レベルの設定
//	HWND hwnd = GetForegroundWindow();	// ウィンドウハンドル取得
//	if (FAILED(m_MouseDevice->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
//	{
//		return E_FAIL;
//	}
//
//	// デバイスを「取得」する
//	hr = m_MouseDevice->Acquire();
//	if (FAILED(hr))
//	{
//		return E_FAIL;
//	}
//
//	// ポーリング開始
//	hr = m_MouseDevice->Poll();
//	if (FAILED(hr))
//	{
//		return E_FAIL;
//	}
//
//	is_debug = false;
//	is_show = true;
//	is_fixed = false;
//
//	return S_OK;
//}
//
//void MouseInput::MouseUpdate()
//{
//	// 更新前の最新マウス情報を保持する
//	m_PrevMouseState = m_CurrentMouseState;
//
//	// 最新のマウスの状態を更新
//	hr = m_MouseDevice->Acquire();
//	if ((hr == DI_OK) || (hr == S_FALSE))
//	{
//		m_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_CurrentMouseState);
//	}
//
//	// ウィンドウハンドル取得
//	HWND hwnd = GetActiveWindow();
//
//	ShowCursor(is_show);
//	// マウス座標取得
//	GetCursorPos(&mousePos);
//	//ScreenToClient(hwnd, &mousePos);
//	GetClientRect(hwnd, &rect);
//	GetWindowRect(hwnd, &window_rect);
//
//	if (is_debug)
//	{
//		if (mousePos.x > window_rect.right)
//		{
//			SetCursorPos(rect.left, mousePos.y);
//		}
//		else if (mousePos.x <= rect.left)
//		{
//			SetCursorPos(window_rect.right, mousePos.y);
//		}
//
//		if (mousePos.y > window_rect.bottom)
//		{
//			SetCursorPos(mousePos.x, rect.top);
//		}
//		else if (mousePos.y <= 0)
//		{
//			SetCursorPos(mousePos.x, rect.bottom);
//		}
//	}
//
//	if (is_fixed)
//		SetCursorPos(static_cast<int>(fixed_position.x), static_cast<int>(fixed_position.y));
//}
//
//// クリックした瞬間の判定
//bool MouseInput::OnMouseDown(InputType::Mouse buttonType)
//{
//	if (buttonType == InputType::Mouse::None)
//		return false;
//
//	// 一フレーム前のマウスのボタンがクリックされていない
//	// 最新のマウスのボタンがクリックされている
//	if (!(m_PrevMouseState.rgbButtons[static_cast<int>(buttonType)] & 0x80 )
//		&& (m_CurrentMouseState.rgbButtons[static_cast<int>(buttonType)] & (0x80)))
//	{
//		return true;
//	}
//
//	return false;
//}
//
//// クリックされている間
//bool MouseInput::OnMouseDownState(InputType::Mouse buttonType)
//{
//	if (buttonType == InputType::Mouse::None)
//		return false;
//
//
//	if (m_CurrentMouseState.rgbButtons[static_cast<int>(buttonType)] & (0x80))
//		return true;
//
//	return false;
//}
//
//// クリックをやめた瞬間の判定
//bool MouseInput::OnMouseUp(InputType::Mouse buttonType)
//{
//	if (buttonType == InputType::Mouse::None)
//		return false;
//
//	// 一フレーム前のマウスのボタンがクリックされている
//	// 最新のマウスのボタンがクリックされていない
//	if ((m_PrevMouseState.rgbButtons[static_cast<int>(buttonType)] & 0x80) &&
//		!(m_CurrentMouseState.rgbButtons[static_cast<int>(buttonType)] & 0x80))
//	{
//		return true;
//	}
//
//	return false;
//}
//
//// マウスカーソルのx座標を取得
//int MouseInput::GetCursorPosX()
//{
//	if (mousePos.x < 0)
//	{
//		//mousePos.x = 0;
//		mousePos.x = rect.right;
//	}
//	if (mousePos.x > rect.right)
//	{
//		//mousePos.x = rect.right;
//		mousePos.x = 0;
//	}
//	return mousePos.x;
//}
//
//// マウスカーソルのy座標を取得
//int MouseInput::GetCursorPosY()
//{
//	if (mousePos.y < 0)
//	{
//		//mousePos.y = 0;
//		mousePos.y = rect.bottom;
//	}
//	if (mousePos.y > rect.bottom)
//	{
//		//mousePos.y = rect.bottom;
//		mousePos.y = 0;
//	}
//	return mousePos.y;
//}
//
//
//// マウスカーソルが1フレームに移動した移動量(X)
//int MouseInput::GetMouseVelocityX()
//{
//	return m_CurrentMouseState.lX;
//}
//
//// マウスカーソルが1フレームに移動した移動量(Y)
//int MouseInput::GetMouseVelocityY()
//{
//	return m_CurrentMouseState.lY;
//}
//
///************************************************
//*
//*	Game Pad
//*
///************************************************/
//
//bool PadInput::IsInput(u_int controller_index, InputType::GamePad button_type, PadInput::PadState state)
//{
//	auto& controller = arr_controller.at(controller_index);
//
//	if (!controller.is_connected)
//		return false;
//
//
//	if (controller.current_state == XINPUT_GAMEPAD_DPAD_LEFT)
//	{
//		int a;
//		a = 0;
//	}
//
//	// 前の状態と判定したいボタンの状態が同じなら
//	if (controller.prev_state & static_cast<WORD>(button_type))
//	{
//		if (controller.current_state ^ static_cast<WORD>(button_type))
//		{
//			if (state == PadState::Release)	return true;
//		}
//
//		if      (state == PadState::Hold)	return true;
//		else						 	    return false;
//	}
//	// 今の状態と判定したいボタンの状態が同じなら
//	else if (controller.current_state & static_cast<WORD>(button_type))
//	{
//		if (state == PadState::Press)	return true;
//		else							return false;
//	}
//	else if (controller.current_state == controller.prev_state)
//	{
//		if (state == PadState::Up)	return true;
//		else						return false;
//	}
//	else if (controller.prev_state ^ static_cast<WORD>(button_type))
//	{
//		if (state == PadState::Release)	return true;
//	}
//
//	return false;
//}
//
//float PadInput::GetAnalogStickVal(u_int controller_index, AnalogStickType stick_type, bool is_dead_zone)
//{
//	auto& controller = arr_controller.at(controller_index);
//
//	if (!controller.is_connected)
//		return 0.0f;
//
//	if (is_dead_zone)
//	{
//		// Zero value if thumbsticks are within the dead zone
//		if ((controller.xinput_state.Gamepad.sThumbLX < INPUT_DEADZONE &&
//			controller.xinput_state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
//			(controller.xinput_state.Gamepad.sThumbLY < INPUT_DEADZONE &&
//				controller.xinput_state.Gamepad.sThumbLY > -INPUT_DEADZONE))
//		{
//			controller.xinput_state.Gamepad.sThumbLX = 0;
//			controller.xinput_state.Gamepad.sThumbLY = 0;
//		}
//
//		if ((controller.xinput_state.Gamepad.sThumbRX < INPUT_DEADZONE &&
//			controller.xinput_state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
//			(controller.xinput_state.Gamepad.sThumbRY < INPUT_DEADZONE &&
//				controller.xinput_state.Gamepad.sThumbRY > -INPUT_DEADZONE))
//		{
//			controller.xinput_state.Gamepad.sThumbRX = 0;
//			controller.xinput_state.Gamepad.sThumbRY = 0;
//		}
//	}
//
//	if		(stick_type == AnalogStickType::Left_Trigger)	return static_cast<float>(controller.xinput_state.Gamepad.bLeftTrigger) / MAX_TRIGGER_VAL;
//	else if (stick_type == AnalogStickType::Right_Trigger)	return static_cast<float>(controller.xinput_state.Gamepad.bRightTrigger) / MAX_TRIGGER_VAL;
//	else if (stick_type == AnalogStickType::Right_X_Thumb)	return static_cast<float>(controller.xinput_state.Gamepad.sThumbRX) / MAX_ANALOG_STICK_VAL;
//	else if (stick_type == AnalogStickType::Right_Y_Thumb)	return static_cast<float>(controller.xinput_state.Gamepad.sThumbRY) / MAX_ANALOG_STICK_VAL;
//	else if (stick_type == AnalogStickType::Left_X_Thumb)	return static_cast<float>(controller.xinput_state.Gamepad.sThumbLX) / MAX_ANALOG_STICK_VAL;
//	else if (stick_type == AnalogStickType::Left_Y_Thumb)	return static_cast<float>(controller.xinput_state.Gamepad.sThumbLY) / MAX_ANALOG_STICK_VAL;
//
//	return 0.0f;
//}
//
//void PadInput::SetVibration(u_int controller_index, float vibration_val, bool is_left)
//{
//	auto& controller = arr_controller.at(controller_index);
//
//	if (!controller.is_connected)
//		return;
//
//	if (is_left)	controller.vibration.wLeftMotorSpeed  = static_cast<WORD>(vibration_val * MAX_VIBRATION_VAL);
//	else			controller.vibration.wRightMotorSpeed = static_cast<WORD>(vibration_val * MAX_VIBRATION_VAL);
//
//	XInputSetState(controller_index, &controller.vibration);
//}
//
