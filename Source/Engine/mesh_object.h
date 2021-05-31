#pragma once

#include <wrl.h>
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>

#include "ecs.h"
#include "enum_state_map.h"
#include "blend.h"
#include "rasterizer.h"
#include "depth_stencil.h"
#include "shader_manager.h"
#include "model_render.h"
#include "rendering_buffer.h"

class MeshObject :public Component
{
protected:
	//-- States of DirectX --//
	EnumStateMap<BlendState>			blend_state{};
	EnumStateMap<RasterizeState>		rasterizer_state{};
	EnumStateMap<RenderingSampleState>	sampler_state{};
	EnumStateMap<DepthStencilState>		depth_stencil_state{};

	shader::MeshShaderState	shader_state;
	RenderingBufferBitset	rendering_buffer;

public:
	explicit MeshObject(Entity* entity);
	explicit MeshObject()  = default; // For cereal
	~MeshObject() override = default;

	void NewFrame(const float delta_time)override {};
	void Update(const float delta_time)override {};
	void RenderImGui() override;

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path_and_name) override;

	[[nodiscard]] auto GetBlendState()		    const { return blend_state.GetCurrentState(); }
	[[nodiscard]] auto GetRasterizerState()	    const { return rasterizer_state.GetCurrentState(); }
	[[nodiscard]] auto GetDepthStencilState()   const { return depth_stencil_state.GetCurrentState(); }
	[[nodiscard]] auto GetSamplerState()		const { return sampler_state.GetCurrentState(); }
	[[nodiscard]] auto GetShaderState()		    const { return shader_state; }

	void SetBlendState(const BlendState& state)					{ blend_state.SetState(state); }
	void SetRasterizerState(const RasterizeState& state)		{ rasterizer_state.SetState(state); }
	void SetDepthStencilState(const DepthStencilState& state)	{ depth_stencil_state.SetState(state); }
	void SetSamplerState(const RenderingSampleState& state)		{ sampler_state.SetState(state); }
	void SetShaderState(shader::MeshShaderTypes type)			{  shader_state.SetShaderState(type); }

	[[nodiscard]] RenderingBufferBitset* UsingBuffer() { return &rendering_buffer; }

	template<class Archive>
	void serialize(Archive&& archive)
	{
		archive(
			cereal::make_nvp("component name"		,component_name),
			cereal::make_nvp("blend state"			,blend_state),
			cereal::make_nvp("rasterizer state"		,rasterizer_state),
			cereal::make_nvp("sampler state"		,sampler_state),
			cereal::make_nvp("depth stencil_state"	,depth_stencil_state),

			cereal::make_nvp("shader state"			,shader_state),
			cereal::make_nvp("rendering_buffer"		,rendering_buffer)
		);
	}
};

CEREAL_REGISTER_TYPE(MeshObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, MeshObject)