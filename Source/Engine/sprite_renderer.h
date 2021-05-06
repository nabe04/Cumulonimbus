#pragma once

#include <array>

#include <d3d11.h>

#include "ecs.h"
#include "shader.h"
#include "shader_manager.h"
#include "constant_buffer.h"

class Blend;
class Rasterizer;
class DepthStencil;
class Sampler;

enum class SpriteSamplerState
{
	Linear_Border,

	End,
};

class SpriteRenderer final
{
private:
	//-- Constant buffer --//
	std::unique_ptr<buffer::ConstantBuffer<shader::CB_CoordinateTransformation>> cbuffer_transformation;
	std::unique_ptr<buffer::ConstantBuffer<shader::CB_Material>>				 cbuffer_material;
	std::unique_ptr<buffer::ConstantBuffer<shader::CB_Light>>					 cbuffer_light;

	//-- DirectX States --//
	std::unique_ptr<Blend>																blend = nullptr;
	std::unique_ptr<Rasterizer>															rasterizer = nullptr;
	std::unique_ptr<DepthStencil>														depth_stencil = nullptr;
	std::array<std::unique_ptr<Sampler>, static_cast<int>(SpriteSamplerState::End)>	    samplers;

	std::unique_ptr<shader::SpriteShaderManager> sprite_shader_manager;

private:
	void RenderSprite(ID3D11DeviceContext* immediate_context, const Entity* entity);
	void RenderAnim(ID3D11DeviceContext* immediate_context, const Entity* entity);
public:
	explicit SpriteRenderer(ID3D11Device* device);

	void Render(ID3D11DeviceContext* immediate_context, const Entity* entity);
};