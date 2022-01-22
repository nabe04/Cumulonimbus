#pragma once
#include "post_effect.h"
#include "fullscreen_quad.h"
#include "frame_buffer.h"
#include "pixel_shader.h"

namespace cumulonimbus::post_effect
{
	class LensDistortion final : public  PostEffect
	{
	public:
		explicit LensDistortion(ID3D11Device* device);
		~LensDistortion() override = default;

		void RenderImGui(ecs::Registry* registry) override;
	};
} // cumulonimbus::post_effect