#include "window.h"

//#include <Windows.h>
//#include <tchar.h>

#include "framework.h"

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Framework* f = reinterpret_cast<Framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	return f ? f->HandleMessage(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

void Window::Create(u_int width, u_int height, u_int pos_x, u_int pos_y)
{
	WNDCLASSEX wcex{};
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= fnWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= instance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _T("Cumlonimbus");
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);

	RECT rc = { 0, 0, static_cast<long>(width), static_cast<long>(height) };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hwnd = CreateWindow(_T("Cumlonimbus"), _T("Cumlonimbus"), WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);
	ShowWindow(hwnd, cmd_show);
	SetWindowPos(hwnd, HWND_TOP, pos_x, pos_y, rc.right, rc.bottom, SWP_SHOWWINDOW);

	window_class_name = wcex.lpszClassName;
	window_width	  = width;
	window_height	  = height;
}

RECT Window::GetThisWindowRect()
{
	GetWindowRect(hwnd, &window_rect);
	return window_rect;
}

RECT Window::GetThisClientRect()
{
	GetClientRect(hwnd, &client_rect);
	return client_rect;
}

bool Window::IsWithinWindow(DirectX::XMINT2 pos)
{
	RECT rect = GetThisClientRect();

	if (pos.x < rect.left)
		return false;
	if (pos.x > rect.right)
		return false;
	if (pos.y < rect.top)
		return false;
	if (pos.y > rect.bottom)
		return false;

	return true;
}