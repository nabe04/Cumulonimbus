#pragma once
#include <Windows.h>
#include <wrl.h>
#include <memory>
#include <d3d11.h>

#include "high_resolution_timer.h"
#include "input_system.h"
#include "locator.h"

class Window;
class Dx11Device;

// Forward declare message handler from imgui_impl_win32.cpp
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class Framework final
{
public:
	explicit Framework(const std::shared_ptr<Window>& window);
	~Framework();

	[[nodiscard]]
	int Run();
	[[nodiscard]]
	bool ProcessLoop();
	void DrawBegin();
	void DrawEnd();

	[[nodiscard]]
	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	[[nodiscard]]
	auto* GetDevice()				const { return device.Get(); }
	[[nodiscard]]
	auto* GetDeviceContext()		const { return immediate_context.Get(); }
	[[nodiscard]]
	auto GetHighResolutionTimer()	const { return hr_timer; }
	[[nodiscard]]
	const auto& GetWindow()			const { return window; }
	[[nodiscard]]
	bool getDebugMode()				const { return debug_mode; }

private:
	Microsoft::WRL::ComPtr<ID3D11Device>		device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;

	std::shared_ptr<Window>		 window{};
	std::shared_ptr<InputSystem> input_system{};
	std::shared_ptr<Dx11Device>  dx11_configurator{};

	const float frame_fate = 60.0f;
	HighResolutionTimer hr_timer;

	bool debug_mode = false;

	bool Initialize();
	bool UnInitialize();
	void CalculateFrameStates();
};

