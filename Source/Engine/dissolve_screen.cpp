#include "dissolve_screen.h"

namespace cumulonimbus::post_effect
{
	DissolveScreen::DissolveScreen(ID3D11Device* device)
		:PostEffect{ device }
	{
		cb_dissolve = std::make_unique<buffer::ConstantBuffer<DissolveScreenCB>>(device);
	}

	void DissolveScreen::RenderImGui(ecs::Registry* registry)
	{
	}

	void DissolveScreen::Begin(ID3D11DeviceContext* immediate_context) const
	{
	}

	void DissolveScreen::End(ID3D11DeviceContext* immediate_context) const
	{
	}
} // cumulonimbus::post_effect