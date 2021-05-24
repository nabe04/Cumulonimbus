#pragma once

#include <bitset>
#include <wrl.h>
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "ecs.h"
#include "blend.h"
#include "rasterizer.h"
#include "sampler.h"
#include "depth_stencil.h"
#include "dx11_configurator.h"
#include "shader_manager.h"
#include "model_render.h"
#include "rendering_buffer.h"

class MeshObject :public Component
{
protected:
	//-- States of DirectX --//
	BlendState			 blend_state			= BlendState::Alpha;
	RasterizeState		 rasterizer_state		= RasterizeState::Cull_Back;
	RenderingSampleState sampler_state			= RenderingSampleState::Linear_Border;
	DepthStencilState	 depth_stencil_state	= DepthStencilState::DepthTest_True_Write_True;

	std::shared_ptr<shader::MeshShaderState>	shader_state = nullptr;
	std::unique_ptr<RenderingBufferBitset>		rendering_buffer;

public:
	explicit MeshObject(Entity* entity);
	~MeshObject() = default;

	virtual void NewFrame(const float delta_time)override {};
	virtual void Update(const float delta_time)override {};
	virtual void RenderImGui() override;

	[[nodiscard]] auto GetBlendState()		    const { return blend_state; }
	[[nodiscard]] auto GetRasterizeState()	    const { return rasterizer_state; }
	[[nodiscard]] auto GetDepthStencilState()   const { return depth_stencil_state; }
	[[nodiscard]] auto GetSamplerState()		const { return sampler_state; }
	[[nodiscard]] auto GetShaderState()		    const { return shader_state; }

	void SetBlendState(const BlendState& state) { blend_state = state; }
	void SetRasterizeState(const RasterizeState& state) { rasterizer_state = state; }
	void SetDepthStencilState(const DepthStencilState& state) { depth_stencil_state = state; }
	void SetSamplerState(const RenderingSampleState& state) { sampler_state = state; }
	void SetShaderState(shader::MeshShaderTypes type) {  shader_state->SetShaderState(type); }

	RenderingBufferBitset* UsingBuffer() { return rendering_buffer.get(); }
};