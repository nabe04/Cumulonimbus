#pragma once

#include <d3d11.h>
#include <wrl.h>

#include <array>

enum class DepthStencilState
{
	DepthTest_False,
	DepthTest_True_Write_True,
	DepthTest_True_Write_False,
	Depth_First,

	End,
};

class DepthStencil final
{
private:
	DepthStencilState current_state = DepthStencilState::End;

	std::array<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>, static_cast<u_int>(DepthStencilState::End)>	depth_stencil_states = {};

public:
	explicit DepthStencil() = default;
	explicit DepthStencil(ID3D11Device* device);
	~DepthStencil() = default;

	void Activate(ID3D11DeviceContext* immediate_context, const DepthStencilState& state);
};