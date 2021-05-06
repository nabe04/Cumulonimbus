#pragma once

#include <array>
#include <memory>

#include "constant_buffer.h"
#include "sampler.h"
#include "fullscreen_quad.h"

class Blend;
class FremeBuffer;
namespace shader
{
	class PixelShader;
}

class SorbelFilter : public FullscreenQuad
{
public:
	enum class SorbelFilterSamplerState
	{
		Linear_Wrap,

		End,
	};

	enum class SorbelFilterDirection
	{
		Horizontal,
		Vertical,

		End,
	};

	struct CB_SorbelFilter
	{
		float ajust_filter_carnel = 1;	// フィルターの調整値
		float padding[3] = {};
	};

private:
	std::array<std::unique_ptr<Sampler>, static_cast<int>(SorbelFilterSamplerState::End)> samplers;
	std::unique_ptr<Blend> blend;

	std::unique_ptr<FrameBuffer> monochrome;
	std::unique_ptr<FrameBuffer> outline_extraction;
	std::unique_ptr<FrameBuffer> sorbel_filter_convolution;

	std::unique_ptr<shader::PixelShader> monochrome_ps;
	std::unique_ptr<shader::PixelShader> outline_extraction_ps;
	std::unique_ptr<shader::PixelShader> sorbel_filter_convolution_ps;

	std::unique_ptr<buffer::ConstantBuffer<CB_SorbelFilter>> cb_sorbel_filter;

	const char* monochrome_ps_filename				= "./Shader/cso/monochrome_ps.cso";
	const char* outline_extraction_ps_filename		= "./Shader/cso/outline_extraction_ps.cso";
	const char* sorbel_filter_convolution_filename	= "./Shader/cso/sorbel_filter_convolution_ps.cso";

public:
	SorbelFilter(ID3D11Device* device, unsigned int width, unsigned int height);
	~SorbelFilter()							= default;
	SorbelFilter(SorbelFilter&)				= delete;
	SorbelFilter& operator= (SorbelFilter)	= delete;

	void Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* shader_resource_veiw_texture);
};