#include "gbuffer.h"

#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus::graphics::buffer
{
	GBuffer::GBuffer()
	{
		// frame_bufferの作成
		ID3D11Device* device = locator::Locator::GetDx11Device()->device.Get();
		const float width    = locator::Locator::GetWindow()->Width();
		const float height   = locator::Locator::GetWindow()->Height();
		albedo_buffer	     = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R8G8B8A8_UNORM	   , DXGI_FORMAT_R24G8_TYPELESS, true, false);
		position_buffer      = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		normal_buffer	     = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		shader_slot_buffer	 = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16_FLOAT			, DXGI_FORMAT_R24G8_TYPELESS, true, false);

		// shaderの作成
		using namespace mapping::shader_filename;
		gbuffer_vs = std::make_unique<shader_system::VertexShader>(vs::GBuffer_VS().c_str());
		gbuffer_ps = std::make_unique<shader_system::PixelShader>(ps::GBuffer_PS().c_str());

		// GBuffer用dsv,srvの作成
		locator::Locator::GetDx11Device()->CreateDepthStencilView(dsv_for_gbuffer, srv_for_gbuffer, width, height);
	}

	void GBuffer::Clear(float r, float g, float b, float a)
	{
		is_used_gbuffer = false;

		float clear_color[4] = { r,g,b,a };
		// shader_slot_bufferのクリアカラーは白で固定する
		const float clear_color_shader_slot[4] = { 0.f,0.f,0.f,0.f };
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(albedo_buffer->GetRTV()		, clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(position_buffer->GetRTV()	, clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(normal_buffer->GetRTV()		, clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(shader_slot_buffer->GetRTV(), clear_color_shader_slot);

		// GBuffer用depth_stencil_viewのっクリア処置
		locator::Locator::GetDx11Device()->immediate_context.Get()->ClearDepthStencilView(dsv_for_gbuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}

	void GBuffer::BindShaderAndRTV()
	{
		is_used_gbuffer = true;

		BindShader();
		BindRTV();
	}

	void GBuffer::UnbindShaderAndRTV()
	{
		UnbindShader();
		UnbindRTV();
	}

	void GBuffer::BindRTV()
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();

		immediate_context->OMGetRenderTargets(1, default_render_target_view.ReleaseAndGetAddressOf(), default_depth_stencil_view.ReleaseAndGetAddressOf());
		ID3D11RenderTargetView* rtv[num_rtv] =
		{
			albedo_buffer->GetRTV(),
			normal_buffer->GetRTV(),
			position_buffer->GetRTV(),
			shader_slot_buffer->GetRTV()
		};

		immediate_context->OMSetRenderTargets(num_rtv, rtv, dsv_for_gbuffer.Get());
	}

	void GBuffer::UnbindRTV()
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();
		ID3D11RenderTargetView* null_rtv[num_rtv] = { nullptr };

		immediate_context->OMSetRenderTargets(num_rtv, null_rtv, nullptr);
		immediate_context->OMSetRenderTargets(1, default_render_target_view.GetAddressOf(), default_depth_stencil_view.Get());
	}

	void GBuffer::BindShader() const
	{
		gbuffer_vs->BindVS();
		gbuffer_ps->BindPS();
	}

	void GBuffer::UnbindShader() const
	{
		gbuffer_vs->UnbindVS();
		gbuffer_ps->UnbindPS();
	}
} // cumulonimbus::graphics::buffer

