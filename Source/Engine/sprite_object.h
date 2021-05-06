#pragma once

#include <array>
#include <string_view>
#include <memory>
#include <string>

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>

#include "ecs.h"
#include "blend.h"
#include "rasterizer.h"
#include "depth_stencil.h"
#include "shader_manager.h"
#include "sprite_renderer.h"

class SpriteObject final :public Component
{
private:
	//-- States of DirectX --//
	BlendState			 blend_state			= BlendState::Alpha;
	RasterizeState		 rasterizer_state		= RasterizeState::Cull_Back;
	SpriteSamplerState   sampler_state			= SpriteSamplerState::Linear_Border;
	DepthStencilState	 depth_stencil_state	= DepthStencilState::DepthTest_False;

	std::unique_ptr<shader::SpriteShaderState> sprite_shader_state;

public:
	SpriteObject() = default;
	SpriteObject(Entity* entity);

	const BlendState& GetBlendState() { return blend_state; }
	const RasterizeState& GetRasterizerState() { return rasterizer_state; }
	const SpriteSamplerState& GetSamplerState() { return sampler_state; }
	const DepthStencilState& GetDepthStencilState() { return depth_stencil_state; }
	const shader::SpriteShaderTypes& GetShaderState() { return sprite_shader_state->GetShaderState(); }

	void SetBlendState(const BlendState& state)					{ blend_state = state; }
	void SetRasterizerState(const RasterizeState& state)		{ rasterizer_state = state; }
	void SetSamplerState(const SpriteSamplerState& state)		{ sampler_state = state; }
	void SetDepthStencilState(const DepthStencilState& state)	{ depth_stencil_state = state; }
	void SetShaderState(const shader::SpriteShaderTypes type) { sprite_shader_state->SetShaderState(type); }

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path_and_name) override;
	
	template <class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			//cereal::base_class<Component>(this)
			CEREAL_NVP(component_name),
			CEREAL_NVP(blend_state),
			CEREAL_NVP(rasterizer_state),
			CEREAL_NVP(sampler_state),
			CEREAL_NVP(depth_stencil_state),
			CEREAL_NVP(sprite_shader_state)
		);
	}
};
