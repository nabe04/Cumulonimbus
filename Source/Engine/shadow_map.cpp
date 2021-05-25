#include "shadow_map.h"

#include "blend.h"
#include "framework.h"
#include "shader.h"
#include "frame_buffer.h"

ShadowMap::ShadowMap(ID3D11Device* device, int width, int height)
	:FullscreenQuad{ device }
{
	// State
	samplers.at(static_cast<int>(ShadowMapSamplerStates::Linear_Border)) = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	blend = std::make_unique<Blend>(device);

	// FrameBuffer
	normal_shadow_depth_extraction_fb   = std::make_unique<FrameBuffer>(device, 4096, 4096, false, 1, DXGI_FORMAT_R32_FLOAT);
	variance_shadow_depth_extraction_fb = std::make_unique<FrameBuffer>(device, 4096, 4096, false, 1, DXGI_FORMAT_R32G32_FLOAT);

	// Shader & Constant buffer
	constant_buffer = std::make_unique<buffer::ConstantBuffer<M_ShadowMap>>(device);
	normal_shadow_depth_extraction_vs = std::make_unique<shader::VertexShader>(device, "./Shader/cso/normal_shadow_depth_extraction_vs.cso");
	normal_shadow_depth_extraction_ps = std::make_unique<shader::PixelShader>(device, "./Shader/cso/normal_shadow_depth_extraction_ps.cso");
	variance_shadow_depth_extraction_vs = std::make_unique<shader::VertexShader>(device, "./Shader/cso/variance_shadow_depth_extraction_vs.cso");
	variance_shadow_depth_extraction_ps = std::make_unique<shader::PixelShader>(device, "./Shader/cso/variance_shadow_depth_extraction_ps.cso");


	//-- Add States --//
	// Begin rendering
	begin_rendering_states.AddState(RenderProcess::NormalShadowDepthExtraction, [this](ID3D11DeviceContext* immediate_context, const M_ShadowMap& m_shadow) {ActivateNormalShadowDepthExtraction(immediate_context, m_shadow); });
	begin_rendering_states.AddState(RenderProcess::VarianceShadowDepthExtraction, [this](ID3D11DeviceContext* immediate_context, const M_ShadowMap& m_shadow) {ActivateVarianceShadowDepthExtraction(immediate_context, m_shadow); });

	// End rendering
	end_rendering_states.AddState(RenderProcess::NormalShadowDepthExtraction, [this](ID3D11DeviceContext* immediate_context) {DeactivateNormalShadowDepthExtraction(immediate_context); });
	end_rendering_states.AddState(RenderProcess::VarianceShadowDepthExtraction, [this](ID3D11DeviceContext* immediate_context) {DeactivateVarianceShadowDepthExtraction(immediate_context); });
}

void ShadowMap::ActivateNormalShadowDepthExtraction(ID3D11DeviceContext* immediate_context, const M_ShadowMap& m_data = {}) const
{
	//constant_buffer->data = cbuff_data;
	//constant_buffer->Activate(immediate_context, 6, true, false);

	// Extraction depth value
	normal_shadow_depth_extraction_vs->Activate(immediate_context);
	normal_shadow_depth_extraction_ps->Activate(immediate_context);
	normal_shadow_depth_extraction_fb->Activate(immediate_context);
}

void ShadowMap::DeactivateNormalShadowDepthExtraction(ID3D11DeviceContext* immediate_context) const
{
	normal_shadow_depth_extraction_ps->Deactivate(immediate_context);
	normal_shadow_depth_extraction_vs->Deactivate(immediate_context);
	normal_shadow_depth_extraction_fb->Deactivate(immediate_context);
}

void ShadowMap::ActivateVarianceShadowDepthExtraction(ID3D11DeviceContext* immediate_context, const M_ShadowMap m_data = {}) const
{
	variance_shadow_depth_extraction_vs->Activate(immediate_context);
	variance_shadow_depth_extraction_ps->Activate(immediate_context);
	variance_shadow_depth_extraction_fb->Activate(immediate_context);
}

void ShadowMap::DeactivateVarianceShadowDepthExtraction(ID3D11DeviceContext* immediate_context) const
{
	variance_shadow_depth_extraction_ps->Deactivate(immediate_context);
	variance_shadow_depth_extraction_vs->Deactivate(immediate_context);
	variance_shadow_depth_extraction_fb->Deactivate(immediate_context);
}
