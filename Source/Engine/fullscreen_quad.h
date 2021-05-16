#pragma once

#include <memory>

#include <d3d11.h>
#include <wrl.h>

#include "frame_buffer.h"

//'fullscreen_quad' dose not have pixel shader and sampler state. you have to make and set pixel shader and sampler state by yourself.
class FullscreenQuad
{
public:
	FullscreenQuad(ID3D11Device* device);
	virtual ~FullscreenQuad()					= default;
	FullscreenQuad(FullscreenQuad&)				= delete;
	FullscreenQuad& operator=(FullscreenQuad&)	= delete;

	std::unique_ptr<FrameBuffer> blend_buffer;

	[[noreturn]] void Blit(ID3D11DeviceContext* immediate_context,
		bool use_embedded_rasterizer_state = true, bool use_embedded_depth_stencil_state = true,
		bool use_embedded_pixel_shader = false, bool enable_msaa = false);

	[[noreturn]] void Convolution(ID3D11DeviceContext* immediate_context,
		bool use_embedded_rasterizer_state = true, bool use_embedded_depth_stencil_state = true,
		bool use_embedded_pixel_shader = false, bool enable_msaa = false);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embedded_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embedded_pixel_shaders[2];

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> embedded_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> embedded_depth_stencil_state;
};