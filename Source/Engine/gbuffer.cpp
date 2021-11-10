#include "gbuffer.h"

#include "locator.h"
#include "shader_filename_mapping.h"
#include "frame_buffer.h"
#include "vertex_shader.h"
#include "pixel_shader.h"
#include "texture_resource_mapping.h"

namespace cumulonimbus::graphics::buffer
{
	GBuffer::GBuffer()
	{
		// frame_bufferの作成
		ID3D11Device* device = locator::Locator::GetDx11Device()->device.Get();
		const float width    = locator::Locator::GetWindow()->Width();
		const float height   = locator::Locator::GetWindow()->Height();
		color_buffer		 = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R8G8B8A8_UNORM	   , DXGI_FORMAT_R24G8_TYPELESS, true, false);
		position_buffer      = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		normal_buffer	     = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);

		//  GBuffer用シェーダーの作成
		using namespace mapping::shader_filename;
		g_buffer_vs = std::make_unique<shader_system::VertexShader>(vs::GBuffer_VS().c_str());
		g_buffer_ps = std::make_unique<shader_system::PixelShader>(ps::GBuffer_PS().c_str());
		// GBufferのライティング用シェーダーの作成
		g_buff_lighting_ps = std::make_unique<shader_system::PixelShader>(ps::GBufferLighting_PS().c_str());

		// GBuffer用dsv,srvの作成
		locator::Locator::GetDx11Device()->CreateDepthStencilView(dsv_for_g_buffer, srv_for_g_buffer, width, height);
	}

	void GBuffer::Clear(float r, float g, float b, float a)
	{
		float clear_color[4] = { r,g,b,a };
		// shader_slot_bufferのクリアカラーは白で固定する
		const float clear_color_shader_slot[4] = { 0.f,0.f,0.f,0.f };
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(color_buffer->GetRTV()		, clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(position_buffer->GetRTV()	, clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(normal_buffer->GetRTV()		, clear_color);

		// GBuffer用depth_stencil_viewのっクリア処置
		locator::Locator::GetDx11Device()->immediate_context.Get()->ClearDepthStencilView(dsv_for_g_buffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}

	void GBuffer::BindShaderAndRTV()
	{
		BindShader();
		BindRTV();
	}

	void GBuffer::UnbindShaderAndRTV()
	{
		UnbindShader();
		UnbindRTV();
	}

	void GBuffer::BindGBuffLightingShader() const
	{
		g_buff_lighting_ps->BindPS();
	}

	void GBuffer::BindGBufferTextures() const
	{
		// albedoテクスチャ
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												color_buffer->GetRenderTargetSRV(),
												TexSlot_BaseColorMap);
		// normal_mapテクスチャ
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												normal_buffer->GetRenderTargetSRV(),
												TexSlot_NormalMap);
		// positionテクスチャ
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												position_buffer->GetRenderTargetSRV(),
												TexSlot_Position);
	}

	void GBuffer::UnbindGBufferTextures() const
	{
		// albedoテクスチャ
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_BaseColorMap);
		// normal_mapテクスチャ
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_NormalMap);
		// positionテクスチャ
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_Position);
		// shader_slotテクスチャ
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_ShaderSlot);
	}

	void GBuffer::BindColorTexture() const
	{
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												color_buffer->GetRenderTargetSRV(),
												TexSlot_BaseColorMap);
	}

	void GBuffer::BindNormalMapTexture() const
	{
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												normal_buffer->GetRenderTargetSRV(),
												TexSlot_NormalMap);
	}

	void GBuffer::BindPositionTexture() const
	{
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												position_buffer->GetRenderTargetSRV(),
												TexSlot_Position);
	}

	void GBuffer::UnbindColorTexture() const
	{
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_BaseColorMap);
	}

	void GBuffer::UnbindNormalMapTexture() const
	{
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_NormalMap);
	}

	void GBuffer::UnbindPositionTexture() const
	{
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_Position);
	}

	ID3D11ShaderResourceView** GBuffer::GetColorBufferSRV_Address() const
	{
		return color_buffer->GetRenderTargetSRV();
	}

	ID3D11ShaderResourceView** GBuffer::GetPositionBufferSRV_Address() const
	{
		return position_buffer->GetRenderTargetSRV();
	}

	ID3D11ShaderResourceView** GBuffer::GetNormalBufferSRV_Address() const
	{
		return normal_buffer->GetRenderTargetSRV();
	}

	void GBuffer::BindRTV()
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();

		immediate_context->OMGetRenderTargets(1, default_render_target_view.ReleaseAndGetAddressOf(), default_depth_stencil_view.ReleaseAndGetAddressOf());
		ID3D11RenderTargetView* rtv[num_rtv] =
		{
			color_buffer->GetRTV(),
			normal_buffer->GetRTV(),
			position_buffer->GetRTV(),
		};

		immediate_context->OMSetRenderTargets(num_rtv, rtv, dsv_for_g_buffer.Get());
	}

	void GBuffer::UnbindRTV()
	{
		ID3D11DeviceContext* immediate_context = locator::Locator::GetDx11Device()->immediate_context.Get();
		ID3D11RenderTargetView* null_rtv[num_rtv] = { nullptr ,nullptr,nullptr };

		immediate_context->OMSetRenderTargets(num_rtv, null_rtv, nullptr);
		immediate_context->OMSetRenderTargets(1, default_render_target_view.GetAddressOf(), default_depth_stencil_view.Get());
	}

	void GBuffer::BindShader() const
	{
		g_buffer_vs->BindVS();
		g_buffer_ps->BindPS();
	}

	void GBuffer::UnbindShader() const
	{
		g_buffer_vs->UnbindVS();
		g_buffer_ps->UnbindPS();
	}
} // cumulonimbus::graphics::buffer

