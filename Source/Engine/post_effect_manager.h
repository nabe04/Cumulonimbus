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
		explicit PostEffectManager()							= default;
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


		template<class Archive>
		void load(Archive&& archive, uint32_t version)
		{
			archive(
				CEREAL_NVP(distort),
				CEREAL_NVP(screen_filter)
			);
		}

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const
		{
			archive(
				CEREAL_NVP(distort),
				CEREAL_NVP(screen_filter)
			);
		}

		void Load(system::System& system);

		void RenderImGui(ecs::Registry* registry) const;

		ID3D11ShaderResourceView** Generate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** scene_srv_address) const;

		[[nodiscard]]
		post_effect::Distort& GetDistort() const
		{
			return *(distort.get());
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
		std::unique_ptr<post_effect::ScreenFilter>	 screen_filter{};
	};
} // cumulonimbus::system

CEREAL_CLASS_VERSION(cumulonimbus::system::PostEffectManager, 0)
