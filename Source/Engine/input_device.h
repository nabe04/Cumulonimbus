#pragma once
//#include <dinput.h>
//#include <Xinput.h>
//
//#include <memory>
//#include <assert.h>
//#include <array>
//
//#include <d3d11.h>
//
//#include <DirectXMath.h>
//#include <SimpleMath.h>
//
//#include "generic.h"
//
//#define INPUT_DEADZONE			( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.
//#define MAX_TRIGGER_VAL			255
//#define MAX_ANALOG_STICK_VAL	32768
//#define MAX_VIBRATION_VAL		65535
//
//namespace key_type
//{
//	// キーラベル
//	const int KEY_1			= DIK_1;
//	const int KEY_2			= DIK_2;
//	const int KEY_3			= DIK_3;
//	const int KEY_4			= DIK_4;
//	const int KEY_5			= DIK_5;
//	const int KEY_6			= DIK_6;
//	const int KEY_7			= DIK_7;
//	const int KEY_8			= DIK_8;
//	const int KEY_9			= DIK_9;
//	const int KEY_0			= DIK_0;
//	const int KEY_A			= DIK_A;
//	const int KEY_B			= DIK_B;
//	const int KEY_C			= DIK_C;
//	const int KEY_D			= DIK_D;
//	const int KEY_E			= DIK_E;
//	const int KEY_F			= DIK_F;
//	const int KEY_G			= DIK_G;
//	const int KEY_H			= DIK_H;
//	const int KEY_I			= DIK_I;
//	const int KEY_J			= DIK_J;
//	const int KEY_K			= DIK_K;
//	const int KEY_L			= DIK_L;
//	const int KEY_M			= DIK_M;
//	const int KEY_N			= DIK_N;
//	const int KEY_O			= DIK_O;
//	const int KEY_P			= DIK_P;
//	const int KEY_Q			= DIK_Q;
//	const int KEY_R			= DIK_R;
//	const int KEY_S			= DIK_S;
//	const int KEY_T			= DIK_T;
//	const int KEY_U			= DIK_U;
//	const int KEY_V			= DIK_V;
//	const int KEY_W			= DIK_W;
//	const int KEY_X			= DIK_X;
//	const int KEY_Y			= DIK_Y;
//	const int KEY_Z			= DIK_Z;
//	const int KEY_UP		= DIK_UP;
//	const int KEY_DOWN		= DIK_DOWN;
//	const int KEY_LEFT		= DIK_LEFT;
//	const int KEY_RIGHT		= DIK_RIGHT;
//	const int KEY_START		= DIK_RETURN;
//	const int KEY_SPACE		= DIK_SPACE;
//	const int KEY_BACK		= DIK_BACK;
//	const int KEY_SHIFT     = DIK_LSHIFT;
//	const int KEY_LEFT_ALT  = DIK_LMENU;
//	const int KEY_RIGHT_ALT = DIK_RMENU;
//	const int KEY_ESC		= DIK_ESCAPE;
//	const int KEY_F1		= DIK_F1;
//	const int KEY_F2		= DIK_F2;
//	const int KEY_F3		= DIK_F3;
//	const int KEY_F4		= DIK_F4;
//	const int KEY_F5		= DIK_F5;
//	const int KEY_F6		= DIK_F6;
//	const int KEY_F7		= DIK_F7;
//	const int KEY_F8		= DIK_F8;
//	const int KEY_F9		= DIK_F9;
//	const int KEY_F10		= DIK_F10;
//	const int KEY_11		= DIK_F11;
//	const int KEY_F12		= DIK_F12;
//}
//
//struct InputType
//{
//	enum class Key
//	{
//		KEY_1			= DIK_1,
//		KEY_2			= DIK_2,
//		KEY_3			= DIK_3,
//		KEY_4			= DIK_4,
//		KEY_5			= DIK_5,
//		KEY_6			= DIK_6,
//		KEY_7			= DIK_7,
//		KEY_8			= DIK_8,
//		KEY_9			= DIK_9,
//		KEY_0			= DIK_0,
//		KEY_A			= DIK_A,
//		KEY_B			= DIK_B,
//		KEY_C			= DIK_C,
//		KEY_D			= DIK_D,
//		KEY_E			= DIK_E,
//		KEY_F			= DIK_F,
//		KEY_G			= DIK_G,
//		KEY_H			= DIK_H,
//		KEY_I			= DIK_I,
//		KEY_J			= DIK_J,
//		KEY_K			= DIK_K,
//		KEY_L			= DIK_L,
//		KEY_M			= DIK_M,
//		KEY_N			= DIK_N,
//		KEY_O			= DIK_O,
//		KEY_P			= DIK_P,
//		KEY_Q			= DIK_Q,
//		KEY_R			= DIK_R,
//		KEY_S			= DIK_S,
//		KEY_T			= DIK_T,
//		KEY_U			= DIK_U,
//		KEY_V			= DIK_V,
//		KEY_W			= DIK_W,
//		KEY_X			= DIK_X,
//		KEY_Y			= DIK_Y,
//		KEY_Z			= DIK_Z,
//		KEY_UP			= DIK_UP,
//		KEY_DOWN		= DIK_DOWN,
//		KEY_LEFT		= DIK_LEFT,
//		KEY_RIGHT		= DIK_RIGHT,
//		KEY_START		= DIK_RETURN,
//		KEY_SPACE		= DIK_SPACE,
//		KEY_BACK		= DIK_BACK,
//		KEY_SHIFT		= DIK_LSHIFT,
//		KEY_LEFT_ALT	= DIK_LMENU,
//		KEY_RIGHT_ALT	= DIK_RMENU,
//		KEY_ESC			= DIK_ESCAPE,
//		KEY_F1			= DIK_F1,
//		KEY_F2			= DIK_F2,
//		KEY_F3			= DIK_F3,
//		KEY_F4			= DIK_F4,
//		KEY_F5			= DIK_F5,
//		KEY_F6			= DIK_F6,
//		KEY_F7			= DIK_F7,
//		KEY_F8			= DIK_F8,
//		KEY_F9			= DIK_F9,
//		KEY_F10			= DIK_F10,
//		KEY_F11			= DIK_F11,
//		KEY_F12			= DIK_F12,
//
//		None = -1,	// Not attach button
//	};
//
//	enum class Mouse
//	{
//		LEFT,
//		RIGHT,
//		WHEEL,
//
//		None = -1,	// Not attach button
//	};
//
//	enum class GamePad
//	{
//		Up_DPad			= XINPUT_GAMEPAD_DPAD_UP,
//		Down_DPad		= XINPUT_GAMEPAD_DPAD_DOWN,
//		Left_DPad		= XINPUT_GAMEPAD_DPAD_LEFT,
//		Right_DPad		= XINPUT_GAMEPAD_DPAD_RIGHT,
//		Start			= XINPUT_GAMEPAD_START,
//		Back			= XINPUT_GAMEPAD_BACK,
//		Left_Thumb		= XINPUT_GAMEPAD_LEFT_THUMB,
//		Right_Thumb		= XINPUT_GAMEPAD_RIGHT_THUMB,
//		Left_Shoulder	= XINPUT_GAMEPAD_LEFT_SHOULDER,
//		Right_Shoulder	= XINPUT_GAMEPAD_RIGHT_SHOULDER,
//		Pad_A			= XINPUT_GAMEPAD_A,
//		Pad_B			= XINPUT_GAMEPAD_B,
//		Pad_X			= XINPUT_GAMEPAD_X,
//		Pad_Y			= XINPUT_GAMEPAD_Y,
//	};
//
//};
//
//class KeyInput
//{
//private:
//	// DirectInput
//	HWND					m_hwnd;
//	LPDIRECTINPUT8			m_Dinput;
//	LPDIRECTINPUTDEVICE8	m_KeyDevice;
//	BYTE					m_KeyInput[256];
//	HRESULT					hr;
//
//	BYTE					m_OldKey[256];
//private:
//	KeyInput()
//	{
//		ZeroMemory(this, sizeof(KeyInput));
//		hr = InitDinput();
//		if (FAILED(hr))
//			assert(!"KeyInput :: initDinput error");
//	}
//	~KeyInput()
//	{
//		SAFE_RELEASE(m_Dinput);
//		SAFE_RELEASE(m_KeyDevice);
//	}
//
//public:
//	HRESULT  InitDinput();
//	void     KeyUpdate();
//	bool	 KeyState(int key);
//	bool	 KeyTrg(int keyType);
//
//	static KeyInput& GetInstance()
//	{
//		static KeyInput instance;
//		return instance;
//	}
//};
//
//
//class MouseInput
//{
//private:
//	// DirectInput
//	HWND							m_hwnd;
//	LPDIRECTINPUT8					m_Dinput;
//	LPDIRECTINPUTDEVICE8			m_MouseDevice;
//	static DIMOUSESTATE				m_CurrentMouseState;
//	static DIMOUSESTATE				m_PrevMouseState;
//	HRESULT							hr;
//	POINT							mousePos;
//	RECT							rect;
//	RECT							window_rect;
//	bool							is_debug  = false;
//	bool							is_show   = true;
//	bool							is_fixed  = false;
//	DirectX::SimpleMath::Vector2	fixed_position{};
//
//public:
//
//private:
//	MouseInput()
//	{
//		ZeroMemory(this, sizeof(MouseInput));
//		hr = InitDinput();
//		if (FAILED(hr))
//			assert(!"MouseInput :: initDinput error");
//	}
//	~MouseInput()
//	{
//		SAFE_RELEASE(m_Dinput);
//		SAFE_RELEASE(m_MouseDevice);
//	}
//
//public:
//	HRESULT InitDinput();
//	void	MouseUpdate();
//	bool	OnMouseDown(InputType::Mouse buttonType);
//	bool	OnMouseDownState(InputType::Mouse buttonType);
//	bool	OnMouseUp(InputType::Mouse buttonType);
//	int		GetCursorPosX();
//	int		GetCursorPosY();
//	int		GetMouseVelocityX();
//	int		GetMouseVelocityY();
//	void	IsDebug(bool flg)	{ is_debug = flg; }
//	bool	IsDebug()			{ return is_debug; }
//	void	IsShow(bool flg)	{ is_show = flg; }
//	bool	IsShow()			{ return is_show; }
//	void	IsFixed(bool flg)	{ is_fixed = flg; }
//	bool	IsFixed()			{ return is_fixed; }
//	void	FixedPosition(DirectX::SimpleMath::Vector2 pos)
//	{ fixed_position = pos; }
//	DirectX::SimpleMath::Vector2 FixedPosition()
//	{ return fixed_position; }
//
//
//	static MouseInput& GetInstance()
//	{
//		static MouseInput instance;
//		return instance;
//	}
//};
//
//class PadInput
//{
//public:
//	struct ControllerState
//	{
//		XINPUT_STATE	 xinput_state;
//		XINPUT_VIBRATION vibration;
//		bool			 is_connected;
//
//		WORD			current_state;
//		WORD			prev_state;
//	};
//
//	enum class AnalogStickType : short
//	{
//		Left_Trigger,
//		Right_Trigger,
//		Left_X_Thumb,
//		Left_Y_Thumb,
//		Right_X_Thumb,
//		Right_Y_Thumb,
//	};
//
//	enum class PadState
//	{
//		Up,
//		Hold,
//		Press,
//		Release,
//	};
//
//private:
//	std::array<ControllerState, 4> arr_controller;
//
//	bool is_deadzone = true;
//
//private:
//	PadInput()
//	{
//		Initialize();
//	}
//
//	~PadInput()
//	{
//		Reset();
//	}
//
//	bool IsConnected(u_int index)
//	{
//		DWORD result;
//
//		// Simply get the state of the controller from XInput.
//		result = XInputGetState(index, &arr_controller.at(index).xinput_state);
//
//		if (result == ERROR_SUCCESS)
//		{
//			arr_controller.at(index).is_connected = true;
//			return true;
//		}
//
//		arr_controller.at(index).is_connected = false;
//		return false;
//	}
//
//	// スティックの入力を方向パッドフラグに変換
//	WORD ThumbToDPad(SHORT sThumbX, SHORT sThumbY, SHORT sDeadZone)
//	{
//		WORD w_buttons = 0;
//
//		if (sThumbY >= sDeadZone)
//		{
//			w_buttons |= XINPUT_GAMEPAD_DPAD_UP;
//		}
//		else if (sThumbY <= -sDeadZone)
//		{
//			w_buttons |= XINPUT_GAMEPAD_DPAD_DOWN;
//		}
//
//		if (sThumbX <= -sDeadZone)
//		{
//			w_buttons |= XINPUT_GAMEPAD_DPAD_LEFT;
//		}
//		else if (sThumbX >= sDeadZone)
//		{
//			w_buttons |= XINPUT_GAMEPAD_DPAD_RIGHT;
//		}
//
//		return w_buttons;
//	}
//
//public:
//	static PadInput* GetInstance()
//	{
//		static PadInput instance;
//		return &instance;
//	}
//
//	void Initialize()
//	{
//		// Init state
//		ZeroMemory(arr_controller.data(), sizeof(ControllerState) * 4);
//	}
//
//	void Update()
//	{
//		DWORD result;
//		for (int i = 0; i < arr_controller.size(); ++i)
//		{
//			// Simply get the state of the controller from XInput.
//			result = XInputGetState(i, &arr_controller.at(i).xinput_state);
//
//			if (result == ERROR_SUCCESS)
//			{
//				arr_controller.at(i).is_connected = true;
//
//				// 左スティックからの入力を方向パッドに変換
//				arr_controller.at(i).xinput_state.Gamepad.wButtons |= ThumbToDPad(	arr_controller.at(i).xinput_state.Gamepad.sThumbLX,
//																					arr_controller.at(i).xinput_state.Gamepad.sThumbLY,
//																					XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
//
//				// コントローラーのボタンの状態(bit)取得
//				arr_controller.at(i).prev_state    = arr_controller.at(i).current_state;
//				arr_controller.at(i).current_state = arr_controller.at(i).xinput_state.Gamepad.wButtons;
//			}
//			else
//				arr_controller.at(i).is_connected = false;
//		}
//	}
//
//	void Reset()
//	{
//		ZeroMemory(arr_controller.data(), sizeof(ControllerState) * 4);
//		XINPUT_VIBRATION vib = {};
//		for (char i = 0; i < 4; i++)
//			XInputSetState(i, &vib);
//	}
//
//	bool IsInput(u_int controller_index, InputType::GamePad button_type, PadState state);
//
//	//return : アナログスティックの値を(0 ～ 1)の間で返す
//	float GetAnalogStickVal(u_int controller_index, AnalogStickType stick_type, bool is_dead_zone = true);
//
//	void SetVibration(u_int controller_index, float vibration_val, bool is_left = true);
//};
