#pragma once

#include <array>
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>

#include "constant_buffer.h"
#include "sampler.h"
#include "fullscreen_quad.h"
#include "state_machine.h"

class Blend;
class FrameBuffer;
namespace shader
{
	class VertexShader;
	class PixelShader;
}

class ShadowMap : public FullscreenQuad
{
public:
	enum class ShadowMapSamplerStates
	{
		Linear_Border,

		End,
	};

	struct M_ShadowMap
	{
		DirectX::XMFLOAT4X4 light_view_matrix;
		DirectX::XMFLOAT4X4 light_prijection_matrix;
	};

	enum class RenderProcess
	{
		DepthExtraction,
		ShadowRendering,

		End
	};

private:
	std::unique_ptr<buffer::ConstantBuffer<M_ShadowMap>> constant_buffer;

	std::array<std::unique_ptr<Sampler>, static_cast<int>(ShadowMapSamplerStates::End)> samplers;
	std::unique_ptr<Blend> blend;

	std::unique_ptr<FrameBuffer> depth_extraction_fb;

	// Vertex shader
	std::unique_ptr<shader::VertexShader> depth_extraction_vs;
	std::unique_ptr<shader::VertexShader> shadow_map_vs;

	// Pixel shader
	std::unique_ptr<shader::PixelShader>  depth_extraction_ps;
	std::unique_ptr<shader::PixelShader>  shadow_map_ps;

	StateMachine<RenderProcess, void, ID3D11DeviceContext*, const M_ShadowMap&> begin_rendering_states;
	StateMachine<RenderProcess, void, ID3D11DeviceContext*>	end_rendering_states;

private:
	void ActivateDepthExtraction(ID3D11DeviceContext* immediate_context, const M_ShadowMap& cbuff_data);
	void DeactivateDepthExtraction(ID3D11DeviceContext* immediate_context);

	void ActivateShadowMap(ID3D11DeviceContext* immediate_context, const M_ShadowMap& cbuff_data);
	void DeactivateShadowMap(ID3D11DeviceContext* immediate_context);

public:
	ShadowMap(ID3D11Device* device, int width, int height);

	FrameBuffer* GetDepthExtractionFB() { return depth_extraction_fb.get(); }
	buffer::ConstantBuffer<M_ShadowMap>* GetConstantBuffer(){return constant_buffer.get(); }

	void Begin(ID3D11DeviceContext* immediate_context)
	{
		samplers.at(static_cast<int>(ShadowMapSamplerStates::Linear_Border))->Activate(immediate_context, 0, false);
		depth_extraction_fb->Clear(immediate_context, 1.f, 0.f, 1.f, 1.f);
	}
	void End(ID3D11DeviceContext* immediate_context)
	{
		ID3D11ShaderResourceView* null_shader_resource_view = nullptr;
		immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);
		samplers.at(static_cast<int>(ShadowMapSamplerStates::Linear_Border))->Deactivate(immediate_context);
	}

	StateMachine<RenderProcess, void, ID3D11DeviceContext*, const M_ShadowMap&>* GetBiginRenderingState() { return &begin_rendering_states; }
	StateMachine<RenderProcess, void, ID3D11DeviceContext*>* GetEndRenderingState() { return &end_rendering_states; }
};