#include "gbuffer.h"

#include "locator.h"
#include "shader_filename_mapping.h"
#include "frame_buffer.h"
#include "vertex_shader.h"
#include "pixel_shader.h"

namespace cumulonimbus::graphics::buffer
{
	GBuffer::GBuffer()
	{
		// frame_buffer�̍쐬
		ID3D11Device* device = locator::Locator::GetDx11Device()->device.Get();
		const float width    = locator::Locator::GetWindow()->Width();
		const float height   = locator::Locator::GetWindow()->Height();
		albedo_buffer	     = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB	   , DXGI_FORMAT_R24G8_TYPELESS, true, false);
		position_buffer      = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		normal_buffer	     = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS, true, false);
		shader_slot_buffer	 = std::make_unique<FrameBuffer>(device, width, height, false, 1, DXGI_FORMAT_R16_FLOAT		   , DXGI_FORMAT_R24G8_TYPELESS, true, false);

		//  GBuffer�p�V�F�[�_�[�̍쐬
		using namespace mapping::shader_filename;
		g_buffer_vs = std::make_unique<shader_system::VertexShader>(vs::GBuffer_VS().c_str());
		g_buffer_ps = std::make_unique<shader_system::PixelShader>(ps::GBuffer_PS().c_str());
		// GBuffer�̃��C�e�B���O�p�V�F�[�_�[�̍쐬
		g_buff_lighting_ps = std::make_unique<shader_system::PixelShader>(ps::GBufferLighting_PS().c_str());

		// GBuffer�pdsv,srv�̍쐬
		locator::Locator::GetDx11Device()->CreateDepthStencilView(dsv_for_g_buffer, srv_for_g_buffer, width, height);
	}

	void GBuffer::Clear(float r, float g, float b, float a)
	{
		float clear_color[4] = { r,g,b,a };
		// shader_slot_buffer�̃N���A�J���[�͔��ŌŒ肷��
		const float clear_color_shader_slot[4] = { 0.f,0.f,0.f,0.f };
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(albedo_buffer->GetRTV()		, clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(position_buffer->GetRTV()	, clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(normal_buffer->GetRTV()		, clear_color);
		locator::Locator::GetDx11Device()->immediate_context->ClearRenderTargetView(shader_slot_buffer->GetRTV(), clear_color_shader_slot);

		// GBuffer�pdepth_stencil_view�̂��N���A���u
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
		// albedo�e�N�X�`��
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												albedo_buffer->GetRenderTargetSRV(),
												TexSlot_BaseColorMap);
		// normal_map�e�N�X�`��
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												normal_buffer->GetRenderTargetSRV(),
												TexSlot_NormalMap);
		// position�e�N�X�`��
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												position_buffer->GetRenderTargetSRV(),
												TexSlot_Position);

		// shader_slot�e�N�X�`��
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												shader_slot_buffer->GetRenderTargetSRV(),
												TexSlot_ShaderSlot);
	}

	void GBuffer::UnbindGBufferTextures() const
	{
		// albedo�e�N�X�`��
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_BaseColorMap);
		// normal_map�e�N�X�`��
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_NormalMap);
		// position�e�N�X�`��
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_Position);
		// shader_slot�e�N�X�`��
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_ShaderSlot);
	}

	void GBuffer::BindAlbedoTexture() const
	{
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												albedo_buffer->GetRenderTargetSRV(),
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

	void GBuffer::BindShaderSlotTexture() const
	{
		locator::Locator::GetDx11Device()->BindShaderResource(
												mapping::graphics::ShaderStage::PS,
												shader_slot_buffer->GetRenderTargetSRV(),
												TexSlot_ShaderSlot);
	}

	void GBuffer::UnbindAlbedoTexture() const
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

	void GBuffer::UnbindShaderSlotTexture() const
	{
		locator::Locator::GetDx11Device()->UnbindShaderResource(
												mapping::graphics::ShaderStage::PS,
												TexSlot_ShaderSlot);
	}

	ID3D11ShaderResourceView** GBuffer::GetAlbedoBufferSRV_Address() const
	{
		return albedo_buffer->GetRenderTargetSRV();
	}

	ID3D11ShaderResourceView** GBuffer::GetPositionBufferSRV_Address() const
	{
		return position_buffer->GetRenderTargetSRV();
	}

	ID3D11ShaderResourceView** GBuffer::GetNormalBufferSRV_Address() const
	{
		return normal_buffer->GetRenderTargetSRV();
	}

	ID3D11ShaderResourceView** GBuffer::GetShaderSlotBufferSRV_Address() const
	{
		return shader_slot_buffer->GetRenderTargetSRV();
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

		immediate_context->OMSetRenderTargets(num_rtv, rtv, dsv_for_g_buffer.Get());
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
		g_buffer_vs->BindVS();
		g_buffer_ps->BindPS();
	}

	void GBuffer::UnbindShader() const
	{
		g_buffer_vs->UnbindVS();
		g_buffer_ps->UnbindPS();
	}
} // cumulonimbus::graphics::buffer

