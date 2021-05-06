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
#include "dx_manager.h"
#include "shader_manager.h"
#include "model_render.h"
#include "rendering_buffer.h"

enum class ActorType
{
	Actor3D,
	Actor2D,

	End
};

class ActorComponent : public Component
{
protected:
	ActorType actor_type;

	//-- States of DirectX --//
	BlendState			 blend_state		 = BlendState::Alpha;
	RasterizeState		 rasterizer_state	 = RasterizeState::Cull_Back;
	RenderingSampleState sampler_state		 = RenderingSampleState::Linear_Border;
	DepthStencilState	 depth_stencil_state = DepthStencilState::DepthTest_True_Write_True;

	//-- State of Shader --//
	std::shared_ptr<shader::MeshShaderState>	shader_state   = nullptr;
	std::shared_ptr<shader::SpriteShaderState>	shader_state_2D = nullptr;

	std::unique_ptr<RenderingBufferBitset> rendering_buffer;

public:
	explicit ActorComponent(Entity* entity, ActorType actor_type = ActorType::Actor3D);
	~ActorComponent() = default;

	virtual void NewFrame(const float delta_time)override {};
	virtual void Update(const float delta_time)override {};
	virtual void RenderImGui() override;

	auto GetBlendState()		const  { return blend_state; }
	auto GetRasterizeState()	const  { return rasterizer_state; }
	auto GetDepthStencilState() const  { return depth_stencil_state; }
	auto GetSamplerState()		const  { return sampler_state; }
	auto GetShaderState()		const  { return shader_state; }
	auto GetShaderState2D()		const  { return shader_state_2D; }

	void SetBlendState(const BlendState& state)					{  blend_state = state; }
	void SetRasterizeState(const RasterizeState& state)			{ rasterizer_state = state; }
	void SetDepthStencilState(const DepthStencilState& state)	{ depth_stencil_state = state; }
	void SetSamplerState(const RenderingSampleState& state)		{ sampler_state = state; }
	void SetShaderState(shader::MeshShaderTypes type)			{ return shader_state->SetShaderState(type); }
	void SetShaderState2D(shader::SpriteShaderTypes type)		{ return shader_state_2D->SetShaderState(type); }

	ActorType GetActorType() { return actor_type; }

	RenderingBufferBitset* UsingBuffer() { return rendering_buffer.get(); }
};