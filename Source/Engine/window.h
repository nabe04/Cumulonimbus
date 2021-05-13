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

	[[noreturn]] void Create(u_int width, u_int height, u_int pos_x, u_int pos_y);

	[[nodiscard]] HWND  GetHWND() const { return hwnd; }
	[[nodiscard]] HINSTANCE HInstance() const { return instance; }
	[[noreturn]] void SetHWND(HWND hwnd) { this->hwnd = hwnd; }
	[[nodiscard]] u_int Width() const { return window_width; }
	[[nodiscard]] u_int Height() const { return window_height; }
	[[nodiscard]] RECT WindowRect();
	[[nodiscard]] RECT ClientRect();
	[[nodiscard]] bool IsWithinWindow(DirectX::XMINT2 pos);
	[[nodiscard]] const wchar_t* WindowClassName() const { return window_class_name; }
};