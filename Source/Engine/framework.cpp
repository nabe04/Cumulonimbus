#include "framework.h"

#include <sstream>

#include <Keyboard.h>
#include <Mouse.h>

#include "window.h"
#include "scene.h"
#include "dx11_configurator.h"
#include "imgui_manager.h"
#include "input_manager.h"
#include "texture.h"
#include "gaussian_blur.h"


bool Framework::Initialize()
{
	dx11_configurator = std::make_shared<Dx11Configurator>(window->GetHWND());
	dx11_configurator->Initialize(window->GetHWND(), device.GetAddressOf(), immediate_context.GetAddressOf(), window->Width(), window->Height());
	Locator::Provide<Dx11Configurator>(dx11_configurator);

	Locator::Provide<Window>(window);
	input_system = std::make_shared<InputSystem>(window->GetHWND());
	input_system->Initialize(window->GetHWND());
	Locator::Provide<InputSystem>(input_system);

	// TextureManager Initialization
	TextureManager::GetInstance()->Initialize(device.Get());

#ifdef _DEBUG
	// Initialization of ImGui
	imgui::Initialize(window->GetHWND(), device.Get(), immediate_context.Get());
#endif

	//-- Setting of frame rate --//
	hr_timer.setFrameRate(frameRate);
	hr_timer.start();

	return true;
}

bool Framework::UnInitialize()
{
	//pad_input::Reset();
#ifdef _DEBUG
	imgui::Release();
#endif
	return true;
}

int Framework::Run()
{
	if (!Initialize())
	{
		return 0;
	}

	SceneManager scene_manager{};
	scene_manager.Execute(this);

	UnInitialize();

	return 0;
}

void Framework::DrawBegin()
{
	Locator::GetDx11Configurator()->Clear();
}

void Framework::DrawEnd()
{
#ifdef _DEBUG
	imgui::Render();
#endif

	// Screen flip
	Locator::GetDx11Configurator()->Flip(1);
}

LRESULT Framework::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		hr_timer.stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		hr_timer.start();
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

bool Framework::ProcessLoop()
{
	MSG msg{};

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(window->GetHWND()))
		{
			window->SetHWND(nullptr);
			UnregisterClass(window->WindowClassName(), window->HInstance());
			return false;
		}
	}

#ifdef _DEBUG
	imgui::Update();
#endif
	//InputSystem::Instance()->UpdateStates();
	Locator::GetInput()->UpdateStates();

	//key_input::KeyUpdate();
	//mouse_input::MouseUpdate();
	//pad_input::Update();

	if (Locator::GetInput()->Keyboard().GetState(Keycode::Esc) == ButtonState::Press)
	{
		return false;
	}

	while (!hr_timer.tick());

	CalculateFrameStates();

	return true;
}

void Framework::CalculateFrameStates()
{
	static int frames = 0;

	frames++;

	// Compute averages over one second period.
	if (hr_timer.GetComparisonTime() >= 1.0)
	{
		const float fps = static_cast<float>(frames);
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(window->GetHWND(), outs.str().c_str());

		hr_timer.ResetComparisonTime();
		frames = 0;
		hr_timer.start();
	}
}
