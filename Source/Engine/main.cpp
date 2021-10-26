#include <cassert>
#include <memory>
#include <tchar.h>
#include <Windows.h>

#include "arithmetic.h"
#include "framework.h"
#include "locator.h"
#include "window.h"

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	SetProcessDPIAware();
#if defined(DEBUG) | defined(_DEBUG)
	// ÉÅÉÇÉäÉäÅ[ÉOåüèo
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	std::shared_ptr<Window> window = std::make_shared<Window>(instance, prev_instance, cmd_line, cmd_show);
	const DirectX::SimpleMath::Vector2 window_size = arithmetic::CalcWindowSize(Window::aspect_ratio ,
																				GetSystemMetrics(SM_CXSCREEN),
																				GetSystemMetrics(SM_CYSCREEN));
	window->Create(window_size.x, window_size.y, 0, 0);

	Microsoft::WRL::ComPtr<ID3D11Debug> debugInterface;
	INT ret;
	{
		Framework f(window);
		SetWindowLongPtr(window->GetHWND(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));
		ret = f.Run();
		cumulonimbus::locator::Locator::GetDx11Device()->device.As(&debugInterface);
		assert(debugInterface);
	}

	debugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);

	return ret;
}