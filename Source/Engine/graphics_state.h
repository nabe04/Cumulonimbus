#pragma once
#include <wrl.h>

#include "blend.h"
#include "component_base.h"
#include "depth_stencil.h"
#include "enum_state_map.h"
#include "rasterizer.h"
#include "sampler.h"

namespace cumulonimbus::graphics
{
	class GraphicsState final
	{
	public:
		explicit GraphicsState() = default;
		~GraphicsState() = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(blend_state),
				CEREAL_NVP(rasterizer_state),
				CEREAL_NVP(depth_stencil_state),
				CEREAL_NVP(sampler_state)
			);
		}

		void RenderImGui();

		[[nodiscard]]
		const auto& GetBlendState()			const { return blend_state.GetCurrentState(); }
		[[nodiscard]]
		const auto& GetRasterizerState()	const { return rasterizer_state.GetCurrentState(); }
		[[nodiscard]]
		const auto& GetDepthStencilState()	const { return depth_stencil_state.GetCurrentState(); }
		[[nodiscard]]
		const auto& GetSamplerState()	    const { return sampler_state.GetCurrentState(); }

		void SetBlendState(const BlendState& state)					{ blend_state.SetState(state); }
		void SetRasterizerState(const RasterizeState& state)		{ rasterizer_state.SetState(state); }
		void SetDepthStencilState(const DepthStencilState& state)	{ depth_stencil_state.SetState(state); }
		void SetSamplerState(const RenderingSampleState& state)		{ sampler_state.SetState(state); }
	private:
		//-- States of DirectX --//
		EnumStateMap<BlendState>			blend_state{BlendState::Alpha};
		EnumStateMap<RasterizeState>		rasterizer_state{ RasterizeState::Cull_Back };
		EnumStateMap<DepthStencilState>		depth_stencil_state{ DepthStencilState::DepthTest_True_Write_True };
		EnumStateMap<RenderingSampleState>	sampler_state{ RenderingSampleState::Linear_Border };
	};
} // cumulonimbus::graphics

