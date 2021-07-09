#include "imgui_manager.h"
#include "framework.h"

namespace imgui
{
	bool Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* immediate_context)
	{
		IMGUI_CHECKVERSION();
		ImGui::SetCurrentContext( ImGui::CreateContext());
		//ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		ImGui::StyleColorsDark();

		if (!(ImGui_ImplWin32_Init(hwnd))) return false;
		if (!(ImGui_ImplDX11_Init(device, immediate_context))) return false;

		return true;
	}

	void Update()
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Render()
	{
		// Rendering
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void Release()
	{
		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}