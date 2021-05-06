#pragma once

#include <Windows.h>
#include <tchar.h>

#include <DirectXMath.h>

class Window final
{
private:
	HWND hwnd{};
	const wchar_t* window_class_name{};
	u_int window_width{};
	u_int window_height{};

	HINSTANCE instance{};
	HINSTANCE prev_instance{};
	LPWSTR	  cmd_line{};
	INT		  cmd_show{};

	RECT window_rect{};
	RECT client_rect{};

public:
	Window(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
		:instance{ instance }
		,prev_instance{ prev_instance }
		,cmd_line{ cmd_line }
		,cmd_show{ cmd_show } {}
	~Window() = default;

	void Create(u_int width, u_int height, u_int pos_x, u_int pos_y);

	HWND  GetHWND() { return hwnd; }
	HINSTANCE HInstance() { return instance; }
	[[noreturn]] void SetHWND(HWND hwnd) { this->hwnd = hwnd; }
	u_int Width() { return window_width; }
	u_int Height() { return window_height; }
	RECT WindowRect();
	RECT ClientRect();
	[[nodiscard]] bool IsWithinWindow(DirectX::XMINT2 pos);
	const wchar_t* WindowClassName() { return window_class_name; }
};