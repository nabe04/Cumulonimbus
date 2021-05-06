#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Windows.h>
#include <list>

namespace Input
{
	static const char PadMax = 4;

	enum class PadButton :unsigned short
	{
		Up = 1 << 0,
		Down = 1 << 1,
		Left = 1 << 2,
		Right = 1 << 3,
		Start = 1 << 4,
		Back = 1 << 5,
		LeftThumb = 1 << 6,
		RightThumb = 1 << 7,
		LeftShoulder = 1 << 8,
		RightShoulder = 1 << 9,
		A = 1 << 12,
		B = 1 << 13,
		X = 1 << 14,
		Y = 1 << 15
	};
	PadButton operator|(PadButton, PadButton);

	enum class Direction :char
	{
		Left,
		Right,
		Up,
		Down
	};

	enum class MouseButton :char
	{
		Left,
		Right,
		Wheel
	};

	struct KeyAndPadLinkData
	{
		char playerNo = 0;
		char vKey = 0;
		PadButton button = {};
	};
	typedef std::list<KeyAndPadLinkData> KeyAndPadLinkDataList;

	void Initialize(float padStickTilt, float padTriggerDepth, const KeyAndPadLinkDataList* linkList = nullptr);
	void Update();
	bool MouseUpdate(HWND hwnd, UINT msg, WPARAM wparam);

	class _GamePad;
	_GamePad* GamePad(char playerNo);

	class _Keyboard;
	_Keyboard* Keyboard();

	class _Mouse;
	_Mouse* Mouse();

	class _GamePad
	{
		class _Button
		{
			WORD current = 0;
			WORD last = 0;

		public:
			void Update(char padNo, WORD data);
			bool State(PadButton button) { return current & (short)button; }
			bool Trigger(PadButton button) { return (~last) & current & (short)button; }
		};
		_Button button;

		class _Stick
		{
			float tiltX = 0;
			float tiltY = 0;
			bool lastStateX = false;
			bool lastStateY = false;

		public:
			void Update(short dataX, short dataY);
			bool State(Direction direction);
			bool Trigger(Direction direction);
			float TiltX() { return tiltX; }
			float TiltY() { return tiltY; }
		};
		_Stick stickL;
		_Stick stickR;

		class _Trigger
		{
			float depth = 0.0f;
			bool lastState = false;

		public:
			void Update(char data);
			bool State();
			bool Trigger();
			float Depth() { return depth; }
		};
		_Trigger triggerL;
		_Trigger triggerR;

		class _ButtonAndStickL
		{
			friend 	class _GamePad;
			_Button* button = nullptr;
			_Stick* stick = nullptr;
			bool lastStateLeft = false;
			bool lastStateRight = false;
			bool lastStateUp = false;
			bool lastStateDown = false;

		public:
			void Update(_Button* button, _Stick* stick);
			bool State(Direction direction);
			bool Trigger(Direction direction);
		};
		_ButtonAndStickL buttonAndStickL;

	public:
		void Update(char playerNo);

		_Button* Button() { return &button; }
		_Stick* StickL() { return &stickL; }
		_Stick* StickR() { return &stickR; }
		_Trigger* TriggerL() { return &triggerL; }
		_Trigger* TriggerR() { return &triggerR; }
		_ButtonAndStickL* ButtonAndStickL() { return &buttonAndStickL; }
	};

	class _Keyboard
	{
		BYTE key[256] = {};
		BYTE lastKey[256] = {};

	public:
		void Update();
		bool State(char vKey) { return key[vKey] & 0x80; }
		bool Trigger(char vKey) { return (~lastKey[vKey]) & key[vKey] & 0x80; }
	};

	class _Mouse
	{
		float curPosX = 0.0f;
		float curPosY = 0.0f;
		int screenTexX = 0;
		int screenTexY = 0;
		float curMovementX = 0.0f;
		float curMovementY = 0.0f;

		int wheelNotch = 0;
		int wheelFraction = 0;

		class _Button
		{
			char button = 0;
			char lastButton = 0;

		public:
			void Update(BYTE data);
			bool State() { return button & 0x80; }
			bool Trigger() { return (~lastButton) & button & 0x80; }
		};
		_Button button[3];

	public:
		bool Update(HWND hwnd, UINT msg, WPARAM wparam);
		_Button* Button(MouseButton button_) { return &button[(char)button_]; }
		float GetCursorPosX() { return curPosX; }
		float GetCursorPosY() { return curPosY; }
		float GetCursorMovementX() { return curMovementX; }
		float GetCursorMovementY() { return curMovementY; }
		void SetCursorPos(float x, float y);
		int WheelRotation() { return wheelNotch; }
	};
}

#endif // !INPUT_MANAGER_H

