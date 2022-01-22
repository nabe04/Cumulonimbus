#pragma once

#include <array>
#include <memory>

#include "constant_buffer.h"
#include "sampler.h"
#include "fullscreen_quad.h"

class Blend;
class FrameBuffer;
namespace shader
{
	class PixelShader;
}

class Bloom final :public FullscreenQuad
{
public:
	enum BloomSamplerStates
	{
		Linear_Border,
		Point_Wrap,
		Linear_Wrap,
		Anistropic,

		End,
	};

	struct M_Bloom
	{
		float glow_extraction_threshold  = 0.010f;
		float blur_convolution_intensity = 0.500f;
		float options[2]{};

#ifdef __cplusplus
		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(glow_extraction_threshold),
				CEREAL_NVP(blur_convolution_intensity),
				CEREAL_NVP(options)
			);
		}

#endif // __cplusplus
	};

	std::unique_ptr<cumulonimbus::buffer::ConstantBuffer<M_Bloom>> constant_buffer;

private:
	std::array<std::unique_ptr<Sampler>, BloomSamplerStates::End> samplers;
	std::unique_ptr<Blend>	blend;

	std::unique_ptr<FrameBuffer> glow_extraction;
	std::unique_ptr<FrameBuffer> gaussian_blur[5][2];

	std::unique_ptr<shader::PixelShader> glow_extraction_ps;
	std::unique_ptr<shader::PixelShader> gaussian_blur_horizontal_ps;
	std::unique_ptr<shader::PixelShader> gaussian_blur_vertical_ps;
	std::unique_ptr<shader::PixelShader> gaussian_blur_convolution_ps;

public:
	Bloom(ID3D11Device* device, unsigned int width, unsigned int height);
	~Bloom() override			= default;
	Bloom(Bloom&)				= delete;
	Bloom& operator =(Bloom)	= delete;

	// 輝度抽出用フレームバッファ実行 & 終了
	void ActivateGlowExtraction(ID3D11DeviceContext* immediate_context);
	void DeactivateGlowExtraction(ID3D11DeviceContext* immediate_context);

	void Clear(ID3D11DeviceContext* immediate_context);

	void Genarate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* hdr_texture);
	void Blit(ID3D11DeviceContext* immediate_context);
};