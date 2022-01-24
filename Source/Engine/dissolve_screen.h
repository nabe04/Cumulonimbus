#pragma once
#include "cbuffer_post_effect.h"
#include "constant_buffer.h"
#include "post_effect.h"

namespace cumulonimbus::post_effect
{
	class DissolveScreen final : public PostEffect
	{
	public:
		explicit DissolveScreen(ID3D11Device* device);
		~DissolveScreen() override = default;

		void RenderImGui(ecs::Registry * registry) override;

		[[nodiscard]]
		buffer::ConstantBuffer<DissolveScreenCB>& GetCBuffer() const
		{
			return *(cb_dissolve.get());
		}
	private:
		std::unique_ptr<buffer::ConstantBuffer<DissolveScreenCB>> cb_dissolve{};

		void Begin(ID3D11DeviceContext* immediate_context) const override;
		void End(ID3D11DeviceContext* immediate_context) const override;
	};
} // cumulonimbus::post_effect