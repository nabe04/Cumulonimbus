#pragma once
#include "cbuffer_post_effect.h"
#include "constant_buffer.h"
#include "post_effect.h"

namespace cumulonimbus::post_effect
{
	class ScreenFilter final : public PostEffect
	{
	public:
		explicit ScreenFilter(ID3D11Device* device);
		~ScreenFilter() override = default;

		void RenderImGui(ecs::Registry* registry) override;
	private:
		std::unique_ptr<buffer::ConstantBuffer<ScreenFilterCB>> cb_screen_filter{};

		void Begin(ID3D11DeviceContext* immediate_context) const override;
		void End(ID3D11DeviceContext* immediate_context) const override;

		bool is_gray_scale{ false };
	};
} // cumulonimbus::post_effect
