#include <Windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>

#include "window.h"
#include "framework.h"

//LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	Framework* f = reinterpret_cast<Framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
//	return f ? f->HandleMessage(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
//}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
#if defined(DEBUG) | defined(_DEBUG)
	// ÉÅÉÇÉäÉäÅ[ÉOåüèo
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	std::shared_ptr<Window> window = std::make_shared<Window>(instance, prev_instance, cmd_line, cmd_show);
	window->Create(960, 540, 270, 50);

	Framework f(window);
	SetWindowLongPtr(window->GetHWND(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));
	return f.Run();
}