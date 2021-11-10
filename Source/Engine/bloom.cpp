#include "bloom.h"

#include "blend.h"
#include "frame_buffer.h"
#include "shader.h"

Bloom::Bloom(ID3D11Device* device, unsigned int width, unsigned int height)
	:FullscreenQuad{ device }
{
	constant_buffer = std::make_unique<cumulonimbus::buffer::ConstantBuffer<M_Bloom>>(device);

	glow_extraction = std::make_unique<FrameBuffer>(device, width, height, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);

	gaussian_blur[0][0] = std::make_unique<FrameBuffer>(device, width >> 0, height >> 0, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[0][1] = std::make_unique<FrameBuffer>(device, width >> 0, height >> 0, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[1][0] = std::make_unique< FrameBuffer>(device, width >> 1, height >> 1, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[1][1] = std::make_unique<FrameBuffer>(device, width >> 1, height >> 1, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[2][0] = std::make_unique<FrameBuffer>(device, width >> 2, height >> 2, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[2][1] = std::make_unique<FrameBuffer>(device, width >> 2, height >> 2, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[3][0] = std::make_unique<FrameBuffer>(device, width >> 3, height >> 3, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[3][1] = std::make_unique<FrameBuffer>(device, width >> 3, height >> 3, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[4][0] = std::make_unique<FrameBuffer>(device, width >> 4, height >> 4, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);
	gaussian_blur[4][1] = std::make_unique<FrameBuffer>(device, width >> 4, height >> 4, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);

	glow_extraction_ps			 = std::make_unique<shader::PixelShader>(device, "./Shader/cso/glow_extraction_ps.cso");
	gaussian_blur_horizontal_ps  = std::make_unique<shader::PixelShader>(device, "./Shader/cso/gaussian_blur_horizontal_ps.cso");
	gaussian_blur_vertical_ps	 = std::make_unique<shader::PixelShader>(device, "./Shader/cso/gaussian_blur_vertical_ps.cso");
	gaussian_blur_convolution_ps = std::make_unique<shader::PixelShader>(device, "./Shader/cso/gaussian_blur_convolution_ps.cso");

	samplers[BloomSamplerStates::Linear_Border] = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR,	D3D11_TEXTURE_ADDRESS_BORDER,	D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplers[BloomSamplerStates::Point_Wrap]	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_POINT,		D3D11_TEXTURE_ADDRESS_WRAP,		D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplers[BloomSamplerStates::Linear_Wrap]	= std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR,	D3D11_TEXTURE_ADDRESS_WRAP,		D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	samplers[BloomSamplerStates::Anistropic]	= std::make_unique<Sampler>(device, D3D11_FILTER_ANISOTROPIC,			D3D11_TEXTURE_ADDRESS_WRAP,		D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);

	blend = std::make_unique<Blend>(device);
}

void Bloom::ActivateGlowExtraction(ID3D11DeviceContext* immediate_context)
{
	samplers[BloomSamplerStates::Point_Wrap]->Activate(immediate_context, 0, false);
	samplers[BloomSamplerStates::Linear_Wrap]->Activate(immediate_context, 1, false);
	samplers[BloomSamplerStates::Anistropic]->Activate(immediate_context, 2, false);

	constant_buffer->Activate(immediate_context, 6, false, true);

	// Extracting bright color
	glow_extraction->Clear(immediate_context);
	glow_extraction->Activate(immediate_context);
	glow_extraction_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);

	ID3D11ShaderResourceView* null_shader_resource_view = 0;
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);
	glow_extraction_ps->Deactivate(immediate_context);
	glow_extraction->Deactivate(immediate_context);

	samplers[BloomSamplerStates::Point_Wrap]->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Linear_Wrap]->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Anistropic]->Deactivate(immediate_context);

	////
	////Gaussian blur
	////Efficient Gaussian blur with linear sampling
	////http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	samplers[BloomSamplerStates::Linear_Border]->Activate(immediate_context, 0, false);

	immediate_context->PSSetShaderResources(0, 1, glow_extraction->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[0][0]->Clear(immediate_context);
	gaussian_blur[0][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[0][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[0][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[0][1]->Clear(immediate_context);
	gaussian_blur[0][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[0][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[0][1]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[1][0]->Clear(immediate_context);
	gaussian_blur[1][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[1][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[1][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[1][1]->Clear(immediate_context);
	gaussian_blur[1][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[1][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[1][1]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[2][0]->Clear(immediate_context);
	gaussian_blur[2][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[2][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[2][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[2][1]->Clear(immediate_context);
	gaussian_blur[2][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[2][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[2][1]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[3][0]->Clear(immediate_context);
	gaussian_blur[3][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[3][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[3][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[3][1]->Clear(immediate_context);
	gaussian_blur[3][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[3][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[3][1]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[4][0]->Clear(immediate_context);
	gaussian_blur[4][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[4][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[4][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[4][1]->Clear(immediate_context);
	gaussian_blur[4][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[4][1]->Deactivate(immediate_context);

	samplers[BloomSamplerStates::Linear_Border]->Deactivate(immediate_context);
}

void Bloom::DeactivateGlowExtraction(ID3D11DeviceContext* immediate_context)
{
	immediate_context->PSSetShaderResources(0, 1, glow_extraction->render_target_shader_resource_view.GetAddressOf());
	FullscreenQuad::Blit(immediate_context);

	glow_extraction_ps->Deactivate(immediate_context);
	glow_extraction->Deactivate(immediate_context);
	ID3D11ShaderResourceView* null_shader_resource_view = 0;
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

	samplers[BloomSamplerStates::Point_Wrap]->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Linear_Wrap]->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Anistropic]->Deactivate(immediate_context);
}



void Bloom::Clear(ID3D11DeviceContext* immediate_context)
{
	glow_extraction->Clear(immediate_context);
}

void Bloom::Genarate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* hdr_texture)
{
	samplers[BloomSamplerStates::Point_Wrap]->Activate(immediate_context, 0, false);
	samplers[BloomSamplerStates::Linear_Wrap]->Activate(immediate_context, 1, false);
	samplers[BloomSamplerStates::Anistropic]->Activate(immediate_context, 2, false);

	constant_buffer->Activate(immediate_context, 6, false, true);

	// Extracting bright color
	glow_extraction->Clear(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &hdr_texture);
	glow_extraction->Activate(immediate_context);
	glow_extraction_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);

	ID3D11ShaderResourceView* null_shader_resource_view = 0;
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);
	glow_extraction_ps->Deactivate(immediate_context);
	glow_extraction->Deactivate(immediate_context);

	samplers[BloomSamplerStates::Point_Wrap]->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Linear_Wrap]->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Anistropic]->Deactivate(immediate_context);

	////
	////Gaussian blur
	////Efficient Gaussian blur with linear sampling
	////http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	samplers[BloomSamplerStates::Linear_Border]->Activate(immediate_context, 0, false);

	immediate_context->PSSetShaderResources(0, 1, glow_extraction->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[0][0]->Clear(immediate_context);
	gaussian_blur[0][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[0][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[0][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[0][1]->Clear(immediate_context);
	gaussian_blur[0][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[0][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[0][1]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[1][0]->Clear(immediate_context);
	gaussian_blur[1][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[1][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[1][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[1][1]->Clear(immediate_context);
	gaussian_blur[1][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[1][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[1][1]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[2][0]->Clear(immediate_context);
	gaussian_blur[2][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[2][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[2][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[2][1]->Clear(immediate_context);
	gaussian_blur[2][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[2][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[2][1]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[3][0]->Clear(immediate_context);
	gaussian_blur[3][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[3][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[3][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[3][1]->Clear(immediate_context);
	gaussian_blur[3][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[3][1]->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[3][1]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[4][0]->Clear(immediate_context);
	gaussian_blur[4][0]->Activate(immediate_context);
	gaussian_blur_horizontal_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_horizontal_ps->Deactivate(immediate_context);
	gaussian_blur[4][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, gaussian_blur[4][0]->render_target_shader_resource_view.GetAddressOf());
	gaussian_blur[4][1]->Clear(immediate_context);
	gaussian_blur[4][1]->Activate(immediate_context);
	gaussian_blur_vertical_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	gaussian_blur_vertical_ps->Deactivate(immediate_context);
	gaussian_blur[4][1]->Deactivate(immediate_context);

	samplers[BloomSamplerStates::Linear_Border]->Deactivate(immediate_context);
}

void Bloom::Blit(ID3D11DeviceContext* immediate_context)
{
	samplers[BloomSamplerStates::Point_Wrap]->Activate(immediate_context,0);
	samplers[BloomSamplerStates::Linear_Wrap]->Activate(immediate_context,1);
	samplers[BloomSamplerStates::Anistropic]->Activate(immediate_context,2);
	blend->Activate(immediate_context, BlendState::Add);
	constant_buffer->Activate(immediate_context, 6, false, true);

	ID3D11ShaderResourceView* shader_resource_views[5] =
	{
		gaussian_blur[0][1]->render_target_shader_resource_view.Get(),
		gaussian_blur[1][1]->render_target_shader_resource_view.Get(),
		gaussian_blur[2][1]->render_target_shader_resource_view.Get(),
		gaussian_blur[3][1]->render_target_shader_resource_view.Get(),
		gaussian_blur[4][1]->render_target_shader_resource_view.Get()
	};
	immediate_context->PSSetShaderResources(0, 5, shader_resource_views);
	gaussian_blur_convolution_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);	// BackBuffer
	gaussian_blur_convolution_ps->Deactivate(immediate_context);
	ID3D11ShaderResourceView* null_shader_resource_views[5] = { 0 };
	immediate_context->PSSetShaderResources(0, 5, null_shader_resource_views);

	constant_buffer->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Point_Wrap]->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Linear_Wrap]->Deactivate(immediate_context);
	samplers[BloomSamplerStates::Anistropic]->Deactivate(immediate_context);
}