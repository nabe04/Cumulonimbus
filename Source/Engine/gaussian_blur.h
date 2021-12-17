#pragma once

#include <memory>
#include <d3d11.h>

#include "fullscreen_quad.h"
#include "vertex_shader.h"
#include "pixel_shader.h"
#include "shader.h"

class GaussianBlur final : public FullscreenQuad
{
public:
	explicit GaussianBlur(ID3D11Device* device, u_int width, u_int height , DXGI_FORMAT format = DXGI_FORMAT_R32G32_FLOAT);
	~GaussianBlur() override = default;

	void ClearRTV(ID3D11DeviceContext* immediate_context) const
	{
		fb_gaussian_blur_horizontal->Clear(immediate_context);
		fb_gaussian_blur_vertical->Clear(immediate_context);
	}
	void GenerateGaussianBlur(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** const blur_texture);

	[[nodiscard]] FrameBuffer* GetFrameBuffer() const
	{
		// fb_gaussian_blur_vertical‚ð•Ô‚µ‚Ä‚¢‚é‚Ì‚Í
		// horizontal‚Åblur‚ð‚©‚¯‚½‚à‚Ì‚Évertical‚ð‚©‚¯‚Ä‚¢‚é‚©‚ç
		return fb_gaussian_blur_vertical.get();
	}
	[[nodiscard]] ID3D11ShaderResourceView* GetSRV() const { return fb_gaussian_blur_vertical->render_target_shader_resource_view.Get(); }

private:
	const char* gaussian_blur_vertical_ps_name	 = "./Shader/cso/gaussian_blur_vertical_ps.cso";
	const char* gaussian_blur_horizontal_ps_name = "./Shader/cso/gaussian_blur_horizontal_ps.cso";

	std::unique_ptr<FrameBuffer> fb_gaussian_blur_vertical{};
	std::unique_ptr<FrameBuffer> fb_gaussian_blur_horizontal{};
	std::unique_ptr<cumulonimbus::shader_system::PixelShader> ps_blur_vertical{};
	std::unique_ptr<cumulonimbus::shader_system::PixelShader> ps_blur_horizontal{};

	const u_int blur_texture_ps_slot = 0;
};