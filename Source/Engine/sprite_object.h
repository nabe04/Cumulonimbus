#pragma once

#include <array>
#include <string_view>
#include <memory>
#include <string>

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"
#include "enum_state_map.h"
#include "blend.h"
#include "rasterizer.h"
#include "depth_stencil.h"
#include "shader_manager.h"
#include "sampler.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::component
{
	class SpriteObjectComponent final :public ComponentBase
	{
	public:
		explicit SpriteObjectComponent() = default;	// For cereal
		explicit SpriteObjectComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		~SpriteObjectComponent() override = default;

		void NewFrame(float dt) override {};
		void Update(float dt)   override {};
		void RenderImGui()      override {};

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		[[nodiscard]] const BlendState&					GetBlendState()			const { return blend_state.GetCurrentState(); }
		[[nodiscard]] const RasterizeState&				GetRasterizerState()	const { return rasterizer_state.GetCurrentState(); }
		[[nodiscard]] const SpriteSamplerState&			GetSamplerState()		const { return sampler_state.GetCurrentState(); }
		[[nodiscard]] const DepthStencilState&			GetDepthStencilState()	const { return depth_stencil_state.GetCurrentState(); }
		[[nodiscard]] const shader::SpriteShaderTypes&	GetShaderState()		const { return sprite_shader_state.GetShaderState(); }

		void SetBlendState(const BlendState& state)					{ blend_state = state; }
		void SetRasterizerState(const RasterizeState& state)		{ rasterizer_state = state; }
		void SetSamplerState(const SpriteSamplerState& state)		{ sampler_state = state; }
		void SetDepthStencilState(const DepthStencilState& state)	{ depth_stencil_state = state; }
		void SetShaderState(const shader::SpriteShaderTypes type)	{ sprite_shader_state.SetShaderState(type); }



		template <class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::base_class<ComponentBase>(this),
				CEREAL_NVP(blend_state),
				CEREAL_NVP(rasterizer_state),
				CEREAL_NVP(sampler_state),
				CEREAL_NVP(depth_stencil_state),
				CEREAL_NVP(sprite_shader_state)
			);
		}

	private:
		//-- States of DirectX --//
		EnumStateMap<BlendState>		 blend_state{ BlendState::Alpha };
		EnumStateMap<RasterizeState>	 rasterizer_state{ RasterizeState::Cull_Back };
		EnumStateMap<SpriteSamplerState> sampler_state{ SpriteSamplerState::Linear_Border };
		EnumStateMap<DepthStencilState>  depth_stencil_state{ DepthStencilState::DepthTest_False };

		shader::SpriteShaderState sprite_shader_state;
	};

	static_assert(std::is_move_constructible_v<SpriteObjectComponent>);
	static_assert(std::is_move_assignable_v<SpriteObjectComponent>);
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::SpriteObjectComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::SpriteObjectComponent)

