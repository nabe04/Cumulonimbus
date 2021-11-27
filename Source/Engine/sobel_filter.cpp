#include "sobel_filter.h"

#include "blend.h"
#include "frame_buffer.h"
#include "shader.h"

SorbelFilter::SorbelFilter(ID3D11Device* device, unsigned int width, unsigned int height)
	:FullscreenQuad{ device }
{
	//-- Frame buffer --//
	monochrome = std::make_unique<FrameBuffer>(device, width, height, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*neblited_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);

	outline_extraction = std::make_unique<FrameBuffer>(
			device, width, height, false/*enable_msaa*/, 1/*subsamples*/,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
			true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
			false/*generate_mips*/);

	sorbel_filter_convolution = std::make_unique<FrameBuffer>(
		device, width, height, false/*enable_msaa*/, 1/*subsamples*/,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN,
		true/*need_render_target_shader_resource_view*/, false/*need_depth_stencil_shader_resource_view*/,
		false/*generate_mips*/);


	//-- Pixel Shader --//
	monochrome_ps				 = std::make_unique<shader::PixelShader>(device, monochrome_ps_filename);
	outline_extraction_ps		 = std::make_unique<shader::PixelShader>(device, outline_extraction_ps_filename);
	sorbel_filter_convolution_ps = std::make_unique<shader::PixelShader>(device, sorbel_filter_convolution_filename);

	//-- Constatnt buffer --//
	cb_sorbel_filter = std::make_unique<cumulonimbus::buffer::ConstantBuffer<CB_SorbelFilter>>(device);

	//-- Sampler state--//
	samplers.at(static_cast<int>(SorbelFilterSamplerState::Linear_Wrap)) = std::make_unique<Sampler>(
		device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);

	//-- Blend state --//
	blend = std::make_unique<Blend>(device);
}

void SorbelFilter::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* shader_resource_veiw_texture)
{
	immediate_context->PSSetShaderResources(0, 1, &shader_resource_veiw_texture);
	monochrome->Clear(immediate_context);
	monochrome->Activate(immediate_context);
	monochrome_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);

	ID3D11ShaderResourceView* null_shader_resource_view = 0;
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);
	monochrome_ps->Deactivate(immediate_context);
	monochrome->Deactivate(immediate_context);

	cb_sorbel_filter->Activate(immediate_context, 6);
	immediate_context->PSSetShaderResources(0, 1, monochrome->render_target_shader_resource_view.GetAddressOf());
	outline_extraction->Clear(immediate_context);
	outline_extraction->Activate(immediate_context);
	outline_extraction_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	outline_extraction_ps->Deactivate(immediate_context);
	outline_extraction->Deactivate(immediate_context);
	cb_sorbel_filter->Deactivate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, monochrome->render_target_shader_resource_view.GetAddressOf());
	immediate_context->PSSetShaderResources(1, 1, outline_extraction->render_target_shader_resource_view.GetAddressOf());
	sorbel_filter_convolution->Clear(immediate_context);
	sorbel_filter_convolution->Activate(immediate_context);
	sorbel_filter_convolution_ps->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	sorbel_filter_convolution_ps->Deactivate(immediate_context);
	sorbel_filter_convolution->Deactivate(immediate_context);

	blend->Activate(immediate_context, BlendState::Subtract);
	immediate_context->PSSetShaderResources(0, 1, sorbel_filter_convolution->render_target_shader_resource_view.GetAddressOf());
	FullscreenQuad::Blit(immediate_context, true, true, true);

	ID3D11ShaderResourceView* null_shader_resource = nullptr;
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource);
}
