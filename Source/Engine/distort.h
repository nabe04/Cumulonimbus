#pragma once
#include "cbuffer_post_effect.h"
#include "constant_buffer.h"
#include "post_effect.h"

namespace cumulonimbus::post_effect
{
	class Distort final : public PostEffect
	{
	public:
		explicit Distort(ID3D11Device* device);
		~Distort() override = default;

		void RenderImGui(ecs::Registry* registry) override;
	private:
		std::unique_ptr<buffer::ConstantBuffer<DistortCB>> cb_distort{};

		void Begin(ID3D11DeviceContext* immediate_context) const override;
		void End(ID3D11DeviceContext* immediate_context) const override;
	};
} // cumulonimbus::post_effect