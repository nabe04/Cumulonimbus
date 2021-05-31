#pragma once

#include <memory>

#include <DirectXMath.h>
using namespace DirectX;

#include <cereal/cereal.hpp>

#include "ecs.h"
#include "enum_state_map.h"
#include "blend.h"
#include "rasterizer.h"
#include "depth_stencil.h"
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
public:
	explicit ActorComponent(Entity* entity, ActorType actor_type = ActorType::Actor3D);
	explicit ActorComponent()  = default; // For cereal
	~ActorComponent() override = default;

	void NewFrame(const float delta_time)override {};
	void Update(const float delta_time)override {};
	void RenderImGui() override;

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path_and_name) override;

	[[nodiscard]] const BlendState& GetBlendState()					const  { return blend_state.GetCurrentState(); }
	[[nodiscard]] const RasterizeState& GetRasterizeState()			const  { return rasterizer_state.GetCurrentState(); }
	[[nodiscard]] const DepthStencilState& GetDepthStencilState()		const  { return depth_stencil_state.GetCurrentState(); }
	[[nodiscard]] const RenderingSampleState& GetSamplerState()		const  { return sampler_state.GetCurrentState(); }
	[[nodiscard]] const shader::MeshShaderState& GetShaderState()		const  { return shader_state; }
	[[nodiscard]] const shader::SpriteShaderState& GetShaderState2D()	const  { return shader_state_2d; }

	void SetBlendState(const BlendState& state)					{  blend_state.SetState(state); }
	void SetRasterizeState(const RasterizeState& state)			{ rasterizer_state.SetState(state); }
	void SetDepthStencilState(const DepthStencilState& state)	{ depth_stencil_state.SetState(state); }
	void SetSamplerState(const RenderingSampleState& state)		{ sampler_state.SetState(state); }
	void SetShaderState(shader::MeshShaderTypes type) 			{ return shader_state.SetShaderState(type); }
	void SetShaderState2D(shader::SpriteShaderTypes type) 		{ return shader_state_2d.SetShaderState(type); }

	[[nodiscard]] const ActorType& GetActorType() const { return actor_type.GetCurrentState(); }

	void SetUsingBuffer(const RenderingBuffer buffer) { rendering_buffer.Set(buffer); }

	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			cereal::make_nvp("actor type"			, actor_type),

			cereal::make_nvp("blend_state"			, blend_state),
			cereal::make_nvp("rasterizer_state"		, rasterizer_state),
			cereal::make_nvp("sampler_state"		, sampler_state),
			cereal::make_nvp("depth_stencil_state"	, depth_stencil_state),

			cereal::make_nvp("shader_state"			, shader_state),
			cereal::make_nvp("shader_state_2d"		, shader_state_2d),

			cereal::make_nvp("rendering_buffer"		, rendering_buffer)
		);
	}

protected:
	EnumStateMap<ActorType> actor_type{ActorType::Actor3D};

	//-- States of DirectX --//
	EnumStateMap<BlendState>			blend_state{ BlendState::Alpha };
	EnumStateMap<RasterizeState>		rasterizer_state{ RasterizeState::Cull_Back };
	EnumStateMap<RenderingSampleState>	sampler_state{ RenderingSampleState::Linear_Border };
	EnumStateMap<DepthStencilState>		depth_stencil_state{ DepthStencilState::DepthTest_True_Write_True };

	//-- State of Shader --//
	shader::MeshShaderState   shader_state{};
	shader::SpriteShaderState shader_state_2d{};

	RenderingBufferBitset rendering_buffer;
};

CEREAL_REGISTER_TYPE(ActorComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ActorComponent)