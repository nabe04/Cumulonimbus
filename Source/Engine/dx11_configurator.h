#pragma once
#include <d3d11.h>
#include <wrl.h>

#include "graphics_mapping.h"

class TextureResource;

class Dx11Configurator final
{
public:
	Microsoft::WRL::ComPtr<ID3D11Device>			device{};
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediate_context{};

	float elapsed_time{};


	explicit Dx11Configurator(const HWND& hwnd) { this->hwnd = hwnd; }
	~Dx11Configurator() = default;
	Dx11Configurator(Dx11Configurator&) = delete;
	Dx11Configurator(Dx11Configurator&&) = delete;
	Dx11Configurator(const Dx11Configurator&) = delete;
	Dx11Configurator(const Dx11Configurator&&) = delete;
	Dx11Configurator& operator=(Dx11Configurator&) = delete;
	Dx11Configurator& operator=(Dx11Configurator&&) = delete;
	Dx11Configurator& operator=(const Dx11Configurator&) = delete;
	Dx11Configurator& operator=(const Dx11Configurator&&) = delete;


	//-- Call it once in the 「Framework」 --//
	bool Initialize(HWND hwnd, ID3D11Device** device, ID3D11DeviceContext** immediate_context, int width, int height);

	//-- Call every frame of a 「Scene」--//
	void Clear(DWORD color = 0x00000000);
	void Flip(int n = 0);

	//static Dx11Configurator& GetInstance(HWND hwnd = nullptr)
	//{
	//	static Dx11Configurator instance(hwnd);
	//	return instance;
	//}

	[[nodiscard]] int GetScreenWidth() const  { return screen_width; }
	[[nodiscard]] int GetScreenHeight() const { return screen_height; }

	[[nodiscard]] ID3D11DepthStencilView*   GetDepthStencilView() const { return depth_stencil_view.Get(); }
	[[nodiscard]] ID3D11RenderTargetView*   GetRenderTargetView() const { return render_target_view.Get(); }
	[[nodiscard]] ID3D11ShaderResourceView* GetShaderResourceView() const { return shader_resource_view.Get(); }

	void SetViewPort(int width, int height) const;
	void BindPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) const;
	void BindShaderResource(cumulonimbus::mapping::graphics::ShaderStage state, ID3D11ShaderResourceView** srv, uint32_t slot) const;
	void BindShaderResource(cumulonimbus::mapping::graphics::ShaderStage state, TextureResource* resource, uint32_t slot) const;
	void BindPrimitiveTopology(cumulonimbus::mapping::graphics::PrimitiveTopology topology) const;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>			 swap_chain{};
	Microsoft::WRL::ComPtr<IDXGIDevice>				 dxgi_device{};

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	 render_target_view{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			 depth_stencil_texture{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   depth_stencil_view{};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view{};
	D3D11_VIEWPORT					 viewport{};

	HWND hwnd{};
	int screen_width{};
	int screen_height{};

	HRESULT CreateDevice(HWND hwnd, ID3D11Device** device, ID3D11DeviceContext** immediate_context);
	bool InitializeRenderTarget();
	bool CreateDepthStencil();
};
