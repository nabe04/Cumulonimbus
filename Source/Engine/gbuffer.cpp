#include "gbuffer.h"

#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus::graphics::buffer
{
	GBuffer::GBuffer()
	{
		// frame_buffer�̍쐬
		ID3D11Device* device = locator::Locator::GetDx11Device()->device.Get();
		const float width    = locator::Locator::GetWindow()->Width();
		const float height   = locator::Locator::GetWindow()->Height();
		albedo_buffer	     = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R8G8B8A8_UNORM	, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		position_buffer      = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		normal_buffer	     = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);

		// shader�̍쐬
		using namespace mapping::shader_filename;
		gbuffer_vs = std::make_unique<shader_system::VertexShader>(vs::GBuffer_VS().c_str());
		gbuffer_ps = std::make_unique<shader_system::PixelShader>(ps::GBuffer_PS().c_str());
	}

	void GBuffer::Clear(float r, float g, float b, float a)
	{
		is_used_gbuffer = false;

		float clear_color[4] = { r,g,b,a };
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(albedo_buffer->GetRTV()  , clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(position_buffer->GetRTV(), clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(normal_buffer->GetRTV()  , clear_color);
	}

	void GBuffer::BindShaderAndRTV(ID3D11DepthStencilView* depth_stencil_view)
	{
		is_used_gbuffer = true;

		BindShader();
		BindRTV(depth_stencil_view);
	}

	void GBuffer::UnbindShaderAndRTV()
	{
		UnbindShader();
		UnbindRTV();
	}

	void GBuffer::BindRTV(ID3D11DepthStencilView* depth_stencil_view)
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();

		immediate_context->OMGetRenderTargets(1, default_render_target_view.ReleaseAndGetAddressOf(), default_depth_stencil_view.ReleaseAndGetAddressOf());
		ID3D11RenderTargetView* rtv[num_rtv] =
		{
			albedo_buffer->GetRTV(),
			normal_buffer->GetRTV(),
			position_buffer->GetRTV()
		};

		immediate_context->OMSetRenderTargets(num_rtv, rtv, depth_stencil_view);
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

