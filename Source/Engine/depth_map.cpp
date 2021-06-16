#include "depth_map.h"

#include "shader.h"
#include "shader_filename_mapping.h"
#include "gaussian_blur.h"

namespace cumulonimbus::renderer
{
	DepthMap::DepthMap(ID3D11Device* device, const u_int width, const u_int height)
	{
		depth_extraction_fb = std::make_unique<FrameBuffer>(device, width, height, false, DXGI_FORMAT_R32G32_FLOAT);
		
		gaussian_blur = std::make_unique<GaussianBlur>(device, width, height);

		depth_extraction_vs = std::make_unique<shader::VertexShader>(device, mapping::shader_filename::DepthExtraction_VS().c_str());
		depth_extraction_ps = std::make_unique<shader::PixelShader>(device , mapping::shader_filename::DepthExtraction_PS().c_str());
	}

	void DepthMap::Begin(ID3D11DeviceContext* const immediate_context) const
	{
		depth_extraction_fb->Clear(immediate_context);
	}

	void DepthMap::End(ID3D11DeviceContext* const immediate_context) const
	{
		// 深度値テクスチャのガウシアンブラー作成
		gaussian_blur->GenerateGaussianBlur(immediate_context, depth_extraction_fb->GetRenderTargetSRV());
	}

	void DepthMap::Activate(ID3D11DeviceContext* const immediate_context) const
	{
		depth_extraction_fb->Activate(immediate_context);
		depth_extraction_vs->Activate(immediate_context);
		depth_extraction_ps->Activate(immediate_context);
	}

	void DepthMap::Deactivate(ID3D11DeviceContext* const immediate_context) const
	{
		depth_extraction_ps->Deactivate(immediate_context);
		depth_extraction_vs->Deactivate(immediate_context);
		depth_extraction_fb->Deactivate(immediate_context);
	}
}
