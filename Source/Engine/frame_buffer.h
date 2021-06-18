	#pragma once

#include <d3d11.h>
#include <wrl.h>

#include <DirectXMath.h>

class FrameBuffer
{
private:
	unsigned int	number_of_stored_viewports = 0;
	D3D11_VIEWPORT	default_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> default_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> default_depth_stencil_view;

public:
	// TODO : メンバ変数のpublicを消す & ゲッタの用意
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> render_target_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depth_stencil_shader_resource_view;

	D3D11_VIEWPORT viewport;

public:
	FrameBuffer(ID3D11Device* device, int width, int height, bool enable_msaa = false, int subsamples = 1, DXGI_FORMAT render_target_texture2d_format = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depth_stencil_texture2d_format = DXGI_FORMAT_R24G8_TYPELESS,
		bool need_render_target_shader_resource_view = true, bool need_depth_stencil_shader_resource_view = true, bool generate_mips = false);
	FrameBuffer(ID3D11Device* device, ID3D11RenderTargetView* shared_render_target_view, DXGI_FORMAT depth_stencil_texture2d_format = DXGI_FORMAT_R24G8_TYPELESS, bool need_depth_stencil_shader_resource_view = true);
	FrameBuffer(ID3D11Device* device, ID3D11DepthStencilView* shared_depth_stencil_view, DXGI_FORMAT render_target_texture2d_format = DXGI_FORMAT_R8G8B8A8_UNORM, bool need_render_target_shader_resource_view = true);
	FrameBuffer(ID3D11Device* device, ID3D11RenderTargetView* shared_render_target_view, ID3D11DepthStencilView* shared_depth_stencil_view);

	virtual ~FrameBuffer()					= default;
	FrameBuffer(FrameBuffer&)				= delete;
	FrameBuffer& operator=(FrameBuffer&)	= delete;

	void Clear(
		ID3D11DeviceContext* immediate_context,
		float r = 0, float g = 0, float b = 0, float a = 1,
		unsigned int clear_flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		float depth = 1, unsigned char stencil = 0) const
	{
		float colour[4] = { r, g, b, a };
		if (render_target_view)
		{
			immediate_context->ClearRenderTargetView(render_target_view.Get(), colour);
		}
		if (depth_stencil_view)
		{
			immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), clear_flags, depth, stencil);
		}
	}

	// clear only "back_buffer"
	void ClearRenderTargetView(
		ID3D11DeviceContext* immediate_context,
		float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const
	{
		float color[4] = { r,g,b,a };
		immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	}

	// clear only "depth_stencil_view"
	void ClearDepthStencilView(
		ID3D11DeviceContext* immediate_context,
		unsigned int clear_flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		float depth = 1, unsigned char stencil = 0) const
	{
		immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), clear_flags, depth, stencil);
	}

	void Activate(ID3D11DeviceContext* const immediate_context)
	{
		number_of_stored_viewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		immediate_context->RSGetViewports(&number_of_stored_viewports, default_viewports);
		immediate_context->RSSetViewports(1, &viewport);

		immediate_context->OMGetRenderTargets(1, default_render_target_view.ReleaseAndGetAddressOf(), default_depth_stencil_view.ReleaseAndGetAddressOf());
		immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
	}

	// activate only "back_buffer"
	void ActivateRenderTargetView(ID3D11DeviceContext* const immediate_context)
	{
		number_of_stored_viewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		immediate_context->RSGetViewports(&number_of_stored_viewports, default_viewports);
		immediate_context->RSSetViewports(1, &viewport);

		immediate_context->OMGetRenderTargets(1, default_render_target_view.ReleaseAndGetAddressOf(), default_depth_stencil_view.ReleaseAndGetAddressOf());
		immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), 0);
	}

	// activate only "depth_stencil_view"
	void ActivateDepthStencilView(ID3D11DeviceContext* const immediate_context)
	{
		number_of_stored_viewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		immediate_context->RSGetViewports(&number_of_stored_viewports, default_viewports);
		immediate_context->RSSetViewports(1, &viewport);

		immediate_context->OMGetRenderTargets(1, default_render_target_view.ReleaseAndGetAddressOf(), default_depth_stencil_view.ReleaseAndGetAddressOf());
		ID3D11RenderTargetView* null_render_target_view = nullptr;
		immediate_context->OMSetRenderTargets(1, &null_render_target_view, depth_stencil_view.Get());
	}

	void Deactivate(ID3D11DeviceContext* const immediate_context)
	{
		immediate_context->RSSetViewports(number_of_stored_viewports, default_viewports);
		immediate_context->OMSetRenderTargets(1, default_render_target_view.GetAddressOf(), default_depth_stencil_view.Get());
	}

	[[nodiscard]] ID3D11RenderTargetView*    GetRTV() const		  { return render_target_view.Get(); }
	[[nodiscard]] ID3D11RenderTargetView**   GetRTV_Address()	  { return render_target_view.GetAddressOf(); }
	[[nodiscard]] ID3D11DepthStencilView*    GetDSV() const		  { return depth_stencil_view.Get(); }
	[[nodiscard]] ID3D11DepthStencilView**   GetDSV_Address()	  { return depth_stencil_view.GetAddressOf(); }
	[[nodiscard]] ID3D11ShaderResourceView** GetRenderTargetSRV() { return render_target_shader_resource_view.GetAddressOf(); }
	[[nodiscard]] ID3D11ShaderResourceView** GetDepthStencilSRV() { return depth_stencil_shader_resource_view.GetAddressOf(); }
	
};