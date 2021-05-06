#pragma once

#include <array>

#include <d3d11.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

// Blend State Types
enum class BlendState
{
	None,
	Alpha,
	Add,
	Subtract,
	Replace,
	Multiply,
	Lighten,
	Darken,
	Screen,
	AlphaToCoverage,

	End
};

class Blend final
{
private:
	BlendState current_state = { BlendState::None };

	std::array<ComPtr<ID3D11BlendState>, static_cast<u_int>(BlendState::End)>	blend_states = { nullptr };

public:
	Blend(ID3D11Device* device);
	~Blend() = default;

	void Activate(ID3D11DeviceContext* immediate_context, BlendState state)
	{
		if (current_state == state)
			return;

		current_state = state;
		immediate_context->OMSetBlendState(blend_states[static_cast<u_int>(current_state)].Get(), nullptr, 0xffffffff);
	}
};