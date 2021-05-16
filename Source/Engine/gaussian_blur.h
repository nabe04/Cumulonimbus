#pragma once

#include <memory>
#include "fullscreen_quad.h"


namespace shader{
	class PixelShader;
}

class FrameBuffer;

class GaussianBlur final : public FullscreenQuad
{
public:
	explicit GaussianBlur(ID3D11Device* device,const u_int width, const u_int height);
	~GaussianBlur() override = default;
	void GenerateGaussianBlur(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* const blur_texture);
	[[nodiscard]] FrameBuffer* GetFrameBuffer() const { return fb_gaussian_blur.get(); }

private:
	const char* gaussian_blur_vertical_ps_name	 = "./Shader/cso/gaussian_blur_vertical_ps.cso";
	const char* gaussian_blur_horizontal_ps_name = "./Shader/cso/gaussian_blur_horizontal_ps.cso";

	std::unique_ptr<FrameBuffer> fb_gaussian_blur{};
	std::unique_ptr<shader::PixelShader> ps_blur_vertical{};
	std::unique_ptr<shader::PixelShader> ps_blur_horizontal{};

	const u_int blur_texture_ps_slot = 0;
};