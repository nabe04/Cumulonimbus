#include "gaussian_blur.h"

#include "frame_buffer.h"
#include "shader.h"

GaussianBlur::GaussianBlur(ID3D11Device* device, const u_int width, const u_int height)
	:FullscreenQuad{device}
{
	fb_gaussian_blur   = std::make_unique<FrameBuffer>(device, width, height);
	ps_blur_vertical   = std::make_unique<shader::PixelShader>(device, gaussian_blur_vertical_ps_name);
	ps_blur_horizontal = std::make_unique<shader::PixelShader>(device, gaussian_blur_horizontal_ps_name);
}


void GaussianBlur::GenerateGaussianBlur(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* const blur_texture)
{
	immediate_context->PSSetShaderResources(blur_texture_ps_slot, 1, &blur_texture);

	// FrameBuffer‚Ì Clear & Set
	fb_gaussian_blur->Clear(immediate_context);
	fb_gaussian_blur->Activate(immediate_context);

	ps_blur_horizontal->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	ps_blur_horizontal->Deactivate(immediate_context);

	ps_blur_vertical->Activate(immediate_context);
	FullscreenQuad::Blit(immediate_context);
	ps_blur_vertical->Deactivate(immediate_context);

	fb_gaussian_blur->Deactivate(immediate_context);
}
