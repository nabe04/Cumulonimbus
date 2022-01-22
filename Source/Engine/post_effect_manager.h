#pragma once
#include "distort.h"
#include "frame_buffer.h"
#include "lens_distortion.h"
#include "screen_filter.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace system
	{
		class System;
	} // system
} // cumulonimbus::ecs

namespace cumulonimbus::system
{
	class PostEffectManager final
	{
	public:
		explicit PostEffectManager(system::System& system, ID3D11Device* device);
		~PostEffectManager()									= default;
		PostEffectManager(const PostEffectManager&)				= delete;
		PostEffectManager(PostEffectManager&)					= delete;
		PostEffectManager(const PostEffectManager&&)			= delete;
		PostEffectManager(PostEffectManager&&)					= delete;
		PostEffectManager operator=(const PostEffectManager&)	= delete;
		PostEffectManager operator=(PostEffectManager&)			= delete;
		PostEffectManager operator=(const PostEffectManager&&)  = delete;
		PostEffectManager operator=(PostEffectManager&&)		= delete;

		void RenderImGui(ecs::Registry* registry) const;

		ID3D11ShaderResourceView** Generate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** scene_srv_address) const;

		[[nodiscard]]
		post_effect::Distort& GetDistort() const
		{
			return *(distort.get());
		}

		[[nodiscard]]
		post_effect::LensDistortion& GetLensDistortion() const
		{
			return *(lens_distortion.get());
		}

		[[nodiscard]]
		post_effect::ScreenFilter& GetScreenFilter() const
		{
			return *(screen_filter.get());
		}
	private:
		std::unique_ptr<FullscreenQuad>				fullscreen_quad{};
		std::unique_ptr<FrameBuffer>				frame_buffer{};
		
		std::unique_ptr<post_effect::Distort>		 distort{};
		std::unique_ptr<post_effect::LensDistortion> lens_distortion{};
		std::unique_ptr<post_effect::ScreenFilter>	 screen_filter{};
	};
} // cumulonimbus::system
