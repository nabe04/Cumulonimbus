#pragma once
#include <Windows.h>
#include <wrl.h>
#include <map>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "high_resolution_timer.h"
#include "input_system.h"

class Window;

//const UINT g_screen_width  = 960;
//const UINT g_screen_height = 540;

// Forward declare message handler from imgui_impl_win32.cpp
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class Framework final
{
private:
	Microsoft::WRL::ComPtr<ID3D11Device>		device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;

	std::shared_ptr<Window>		 window;
	std::shared_ptr<InputSystem> input_system;

	const double frameRate = 60;
	HighResolutionTimer hr_timer;

	bool debugMode = false;

private:
	bool Initialize();
	bool UnInitialize();
	void CalculateFrameStates();

public:
	explicit Framework(const std::shared_ptr<Window>& w)
		:window{ w } {}
	~Framework() = default;

	int Run();
	bool ProcessLoop();
	void DrawBegin();
	void DrawEnd();

	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	[[nodiscard]] auto* GetDevice()const{ return device.Get(); }
	[[nodiscard]] auto* GetDeviceContext() const{ return immediate_context.Get(); }
	[[nodiscard]] auto GetHighResolutionTimer() const{ return hr_timer; }
	[[nodiscard]] const auto& GetWindow() const { return window; }
	[[nodiscard]] bool getDebugMode() const { return debugMode; }
};

