#pragma once
#include <d3d11.h>
#include <wrl.h>

#include "graphics_mapping.h"

class TextureResource;

class Dx11Device final
{
public:
	Microsoft::WRL::ComPtr<ID3D11Device>			device{};
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediate_context{};

	float elapsed_time{};

	explicit Dx11Device(const HWND& hwnd) { this->hwnd = hwnd; }
	~Dx11Device()								= default;
	Dx11Device(Dx11Device&)						= delete;
	Dx11Device(Dx11Device&&)					= delete;
	Dx11Device(const Dx11Device&)				= delete;
	Dx11Device(const Dx11Device&&)				= delete;
	Dx11Device& operator=(Dx11Device&)			= delete;
	Dx11Device& operator=(Dx11Device&&)			= delete;
	Dx11Device& operator=(const Dx11Device&)	= delete;
	Dx11Device& operator=(const Dx11Device&&)	= delete;


	//-- Call it once in the 「Framework」 --//
	bool Initialize(HWND hwnd, ID3D11Device** device, ID3D11DeviceContext** immediate_context, int width, int height);

	//-- Call every frame of a 「Scene」--//
	void Clear(DWORD color = 0x00000000);
	void Flip(int n = 0);

	[[nodiscard]] int GetScreenWidth()  const { return screen_width; }
	[[nodiscard]] int GetScreenHeight() const { return screen_height; }

	[[nodiscard]] ID3D11DepthStencilView*   GetDepthStencilView()	const { return depth_stencil_view.Get(); }
	[[nodiscard]] ID3D11RenderTargetView*   GetBackBufferRTV()		const { return back_buffer.Get(); }
	[[nodiscard]] ID3D11ShaderResourceView* GetShaderResourceView() const { return shader_resource_view.Get(); }

	/*
	 * brief									: depth_stencil_viewの作成
	 * width									: dsvテクスチャの幅
	 * height									: dsvテクスチャの高さ
	 * need_depth_stencil_shader_resource_view	: 作成したdsvをテクスチャとして使用するか
	 */
	void CreateDepthStencilView(Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& dsv,
								Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& ds_srv,
								u_int width, u_int height,
								DXGI_FORMAT depth_stencil_texture_format = DXGI_FORMAT_R24G8_TYPELESS);

	void SetViewPort(int width, int height) const;
	void BindPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) const;
	void BindShaderResource(	cumulonimbus::mapping::graphics::ShaderStage state, ID3D11ShaderResourceView** srv, uint32_t slot) const;
	void BindShaderResource(	cumulonimbus::mapping::graphics::ShaderStage state, TextureResource* resource, uint32_t slot) const;
	void UnbindShaderResource(cumulonimbus::mapping::graphics::ShaderStage state, uint32_t slot) const;
	void BindPrimitiveTopology(cumulonimbus::mapping::graphics::PrimitiveTopology topology) const;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>			 swap_chain{};
	Microsoft::WRL::ComPtr<IDXGIDevice>				 dxgi_device{};

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	 back_buffer{};
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
