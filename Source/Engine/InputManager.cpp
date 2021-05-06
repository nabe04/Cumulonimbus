#include "InputManager.h"

#pragma comment(lib,"Xinput")
#include <Xinput.h>
#include <assert.h>

namespace Input
{
	struct LinkData
	{
		char vKey = 0;
		PadButton button = {};
	};

	struct
	{
		bool initializedFlg = false;

		float stickTilt = 0.0f;
		float triggerDepth = 0.0f;
		std::list<LinkData> linkDataList[PadMax];

		_GamePad gamepad[PadMax];
		_Keyboard keyboard;
		_Mouse mouse;
	}manager;

	_GamePad* GamePad(char playerNo)
	{
		assert(playerNo >= 0 && playerNo < PadMax && "パッド番号が範囲外です。");
		return &manager.gamepad[playerNo];
	}

	_Keyboard* Keyboard()
	{
		return &manager.keyboard;
	}

	_Mouse* Mouse()
	{
		return &manager.mouse;
	}

	PadButton operator|(PadButton a, PadButton b)
	{
		return (PadButton)((short)a & (short)b);
	}

	void Initialize(float padStickTilt, float padTriggerDepth, const KeyAndPadLinkDataList* linkList)
	{
		if (manager.initializedFlg)return;
		manager.initializedFlg = true;

		manager.stickTilt = padStickTilt;
		manager.triggerDepth = padTriggerDepth;

		if (linkList) {
			for (auto& it : *linkList) {
				if (it.playerNo < 0 || it.playerNo >= PadMax)continue;
				manager.linkDataList[it.playerNo].push_back({ it.vKey,it.button });
			}
		}
	}

	void Update()
	{
		manager.keyboard.Update();
		for (char i = 0; i < PadMax; i++) {
			manager.gamepad[i].Update(i);
		}
	}

	void _GamePad::Update(char padNo)
	{
		assert(padNo >= 0 && padNo < PadMax && "パッド番号が範囲外です。");
		buttonAndStickL.Update(&button, &stickL);

		XINPUT_STATE xinput_state = {};
		XInputGetState(padNo, &xinput_state);
		XINPUT_GAMEPAD& pad = xinput_state.Gamepad;
		button.Update(padNo, pad.wButtons);
		stickL.Update(pad.sThumbLX, pad.sThumbLY);
		stickR.Update(pad.sThumbRX, pad.sThumbRY);
		triggerL.Update(pad.bLeftTrigger);
		triggerR.Update(pad.bRightTrigger);
	}

	void _GamePad::_Button::Update(char padNo,WORD data)
	{
		for (auto& it : manager.linkDataList[padNo]) {
			if (manager.keyboard.State(it.vKey))
				data |= (WORD)it.button;
		}
		last = current;
		current = data;
	}

	void _GamePad::_Stick::Update(short dataX, short dataY)
	{
		lastStateX = fabsf(tiltX) > manager.stickTilt;
		lastStateY = fabsf(tiltY) > manager.stickTilt;

		tiltX = (float)dataX / (dataX > 0 ? 32767 : 32768);
		tiltY = (float)dataY / (dataY > 0 ? 32767 : 32768);
	}

	bool _GamePad::_Stick::State(Direction direction)
	{
		switch (direction) {
		case Direction::Left:
			return tiltX < -manager.stickTilt;
		case Direction::Right:
			return tiltX > manager.stickTilt;
		case Direction::Up:
			return tiltY > manager.stickTilt;
		case Direction::Down:
			return tiltY < -manager.stickTilt;
		}
		return false;
	}

	bool _GamePad::_Stick::Trigger(Direction direction)
	{
		switch (direction) {
		case Direction::Left:
			if (lastStateX)return false;
			return tiltX < -manager.stickTilt;

		case Direction::Right:
			if (lastStateX)return false;
			return tiltX > manager.stickTilt;

		case Direction::Up:
			if (lastStateY)return false;
			return tiltY > manager.stickTilt;

		case Direction::Down:
			if (lastStateY)return false;
			return tiltY < -manager.stickTilt;
		}
		return false;
	}

	void _GamePad::_Trigger::Update(char data)
	{
		lastState = depth > manager.triggerDepth;
		depth = (float)depth / 256;
	}

	bool _GamePad::_Trigger::State()
	{
		return depth > manager.triggerDepth;
	}

	bool _GamePad::_Trigger::Trigger()
	{
		if (lastState)return false;
		return depth > manager.triggerDepth;
	}

	void _GamePad::_ButtonAndStickL::Update(_Button* button, _Stick* stick)
	{
		this->button = button;
		this->stick = stick;

		lastStateLeft = button->State(PadButton::Left) || stick->State(Direction::Left);
		lastStateRight = button->State(PadButton::Right) || stick->State(Direction::Right);
		lastStateUp = button->State(PadButton::Up) || stick->State(Direction::Up);
		lastStateDown = button->State(PadButton::Down) || stick->State(Direction::Down);
	}

	bool _GamePad::_ButtonAndStickL::State(Direction direction)
	{
		switch (direction) {
		case Direction::Left:
			return button->State(PadButton::Left) || stick->State(Direction::Left);

		case Direction::Right:
			return button->State(PadButton::Right) || stick->State(Direction::Right);

		case Direction::Up:
			return button->State(PadButton::Up) || stick->State(Direction::Up);

		case Direction::Down:
			return button->State(PadButton::Down) || stick->State(Direction::Down);
		}
		return false;
	}
	bool _GamePad::_ButtonAndStickL::Trigger(Direction direction)
	{
		switch (direction) {
		case Direction::Left:
			if (lastStateLeft)
				return false;
			return button->Trigger(PadButton::Left) || stick->Trigger(Direction::Left);

		case Direction::Right:
			if (lastStateRight)
				return false;
			return button->Trigger(PadButton::Right) || stick->Trigger(Direction::Right);

		case Direction::Up:
			if (lastStateUp)return false;
			return button->Trigger(PadButton::Up) || stick->Trigger(Direction::Up);

		case Direction::Down:
			if (lastStateDown)return false;
			return button->Trigger(PadButton::Down) || stick->Trigger(Direction::Down);
		}
		return false;
	}

	void _Keyboard::Update()
	{
		for (int i = 0; i < 256; i++)lastKey[i] = key[i];
		GetKeyboardState(key);
	}

	bool MouseUpdate(HWND hwnd, UINT msg, WPARAM wparam)
	{
		return manager.mouse.Update(hwnd, msg, wparam);
	}

	void _Mouse::SetCursorPos(float x, float y)
	{
		::SetCursorPos((int)x + screenTexX, (int)y + screenTexY);
	}

	void _Mouse::_Button::Update(BYTE data)
	{
		lastButton = button;
		button = data;
	}

	bool _Mouse::Update(HWND hwnd, UINT msg, WPARAM wparam)
	{
		switch (msg) {
		case WM_MOUSEMOVE: {
			float lastPosX = curPosX;
			float lastPosY = curPosY;

			POINT curPos = {};
			ScreenToClient(hwnd, &curPos);
			screenTexX = -curPos.x;
			screenTexY = -curPos.y;

			GetCursorPos(&curPos);
			curPosX = (float)(curPos.x - screenTexX);
			curPosY = (float)(curPos.y - screenTexY);

			curMovementX = curPosX - lastPosX;
			curMovementY = curPosY - lastPosY;

			return true;
		}
		case WM_MOUSEWHEEL: {
			wheelFraction += GET_WHEEL_DELTA_WPARAM(wparam);
			wheelNotch = wheelFraction / WHEEL_DELTA;
			wheelFraction %= WHEEL_DELTA;

			return true;
		}
		case WM_LBUTTONDOWN: {
			button[(char)MouseButton::Left].Update(0x80);
			return true;
		}
		case WM_LBUTTONUP: {
			button[(char)MouseButton::Left].Update(0x00);
			return true;
		}
		case WM_RBUTTONDOWN: {
			button[(char)MouseButton::Right].Update(0x80);
			return true;
		}
		case WM_RBUTTONUP: {
			button[(char)MouseButton::Right].Update(0x00);
			return true;
		}
		case WM_MBUTTONDOWN: {
			button[(char)MouseButton::Wheel].Update(0x80);
			return true;
		}
		case WM_MBUTTONUP: {
			button[(char)MouseButton::Wheel].Update(0x00);
			return true;
		}
		default:break;
		}

		return false;
	}
}