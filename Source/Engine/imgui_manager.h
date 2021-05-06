#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#define USE_IMGUI

namespace imgui
{
	bool Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* immediate_context);
	void Update();
	void Render();
	void Release();
}