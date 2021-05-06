#pragma once
#include <d3d11.h>
#include <memory>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <assert.h>
#include <array>

class DxManager final
{
public:
	ComPtr<ID3D11Device>			device;
	ComPtr<ID3D11DeviceContext>		immediate_context;

	float elapsetTime;

private:
	ComPtr<IDXGISwapChain>			 swap_chain;
	ComPtr<IDXGIDevice>				 dxgi_device;

	ComPtr<ID3D11RenderTargetView>	 render_target_view;
	ComPtr<ID3D11Texture2D>			 depth_stencil_texture;
	ComPtr<ID3D11DepthStencilView>   depth_stencil_view;
	ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_VIEWPORT					 viewport = {};

	HWND hwnd;
	int screen_width;
	int screen_height;

private:
	DxManager(HWND hwnd) { this->hwnd = hwnd; };
	~DxManager() {};

	HRESULT CreateDevice(HWND hwnd,ID3D11Device** device, ID3D11DeviceContext** immediate_context);
	bool InitializeRenderTarget();
	bool CreateDepthStencil();

public:
	//-- Call it once in the 「Framework」 --//
	bool Initialize(HWND hwnd, ID3D11Device** device, ID3D11DeviceContext** immediate_context, int width, int height);
	void Release() {};

	//-- Call every frame of a 「Scene」--//
	void Clear(DWORD color = 0x00000000);
	void Flip(int n = 0);

	static DxManager& GetInstance(HWND hwnd = nullptr)
	{
		static DxManager instance(hwnd);
		return instance;
	}

	int GetScreenWidth()  { return screen_width; }
	int GetScreenHeight() { return screen_height; }

	ID3D11DepthStencilView*  GetDepthStencilView() { return depth_stencil_view.Get(); }
	ID3D11RenderTargetView*  GetRenderTargetView() { return render_target_view.Get(); }
	ID3D11ShaderResourceView* GetShaderResourceView() { return shader_resource_view.Get(); }

	void SetViewPort(int width, int height);
};
