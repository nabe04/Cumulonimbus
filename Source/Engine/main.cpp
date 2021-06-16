#include <Windows.h>
#include <memory>
#include <assert.h>
#include "locator.h"
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

	Microsoft::WRL::ComPtr<ID3D11Debug> debugInterface;
	INT ret;
	{
		Framework f(window);
		SetWindowLongPtr(window->GetHWND(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));
		ret = f.Run();
		Locator::GetDx11Configurator()->device.As(&debugInterface);
		assert(debugInterface);
	}

	debugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);

	return ret;
}