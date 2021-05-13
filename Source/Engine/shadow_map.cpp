#include "shadow_map.h"

#include "blend.h"
#include "framework.h"
#include "shader.h"

ShadowMap::ShadowMap(ID3D11Device* device, int width, int height)
	:FullscreenQuad{ device }
{
	// State
	samplers.at(static_cast<int>(ShadowMapSamplerStates::Linear_Border)) = std::make_unique<Sampler>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_ALWAYS, 0.0f, 0.0f, 0.0f, 1.0f);
	blend = std::make_unique<Blend>(device);

	// FrameBuffer
	depth_extraction_fb = std::make_unique<FrameBuffer>(device, width, height,false,1, DXGI_FORMAT_R32G32_FLOAT);

	// Shader & Constant buffer
	constant_buffer = std::make_unique<buffer::ConstantBuffer<M_ShadowMap>>(device);
	depth_extraction_vs = std::make_unique<shader::VertexShader>(device, "./Shader/cso/depth_extraction_vs.cso");
	depth_extraction_ps = std::make_unique<shader::PixelShader>(device, "./Shader/cso/depth_extraction_ps.cso");
	shadow_map_vs = std::make_unique<shader::VertexShader>(device, "./Shader/cso/shadow_map_vs.cso");
	shadow_map_ps = std::make_unique<shader::PixelShader>(device, "./Shader/cso/shadow_map_ps.cso");


	//-- Add States --//
	// Begin rendering
	begin_rendering_states.AddState(RenderProcess::DepthExtraction, [this](ID3D11DeviceContext* immediate_context, const M_ShadowMap& cbuffer) {ActivateDepthExtraction(immediate_context, cbuffer); });
	begin_rendering_states.AddState(RenderProcess::ShadowRendering, [this](ID3D11DeviceContext* immediate_context, const M_ShadowMap& cbuffer) {ActivateShadowMap(immediate_context, cbuffer); });

	// End rendering
	end_rendering_states.AddState(RenderProcess::DepthExtraction, [this](ID3D11DeviceContext* immediate_context) {DeactivateDepthExtraction(immediate_context); });
	end_rendering_states.AddState(RenderProcess::ShadowRendering, [this](ID3D11DeviceContext* immediate_context) {DeactivateShadowMap(immediate_context); });
}

void ShadowMap::ActivateDepthExtraction(ID3D11DeviceContext* immediate_context, const M_ShadowMap& cbuff_data = {})
{
	//constant_buffer->data = cbuff_data;
	//constant_buffer->Activate(immediate_context, 6, true, false);

	// Extraction depth value
	depth_extraction_fb->Activate(immediate_context);
	depth_extraction_vs->Activate(immediate_context);
	depth_extraction_ps->Activate(immediate_context);
}

void ShadowMap::DeactivateDepthExtraction(ID3D11DeviceContext* immediate_context)
{
	depth_extraction_ps->Deactivate(immediate_context);
	depth_extraction_vs->Deactivate(immediate_context);
	depth_extraction_fb->Deactivate(immediate_context);
}

void ShadowMap::ActivateShadowMap(ID3D11DeviceContext* immediate_context, const M_ShadowMap& cbuff_data = {})
{
	shadow_map_vs->Activate(immediate_context);
	shadow_map_ps->Activate(immediate_context);

	immediate_context->PSSetShaderResources(0, 1, depth_extraction_fb->render_target_shader_resource_view.GetAddressOf());
}

void ShadowMap::DeactivateShadowMap(ID3D11DeviceContext* immediate_context)
{
	shadow_map_ps->Deactivate(immediate_context);
	shadow_map_vs->Deactivate(immediate_context);
}