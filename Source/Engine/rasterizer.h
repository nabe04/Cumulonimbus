#pragma once

#include <array>

#include <d3d11.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

// Rasterize State Types
enum class RasterizeState
{
	Cull_Back,
	Cull_Front,
	Cull_None,

	Cull_Back_CCW_True,
	Cull_Front_CCW_True,
	Cull_None_CCW_True,

	Wireframe,

	End,
};

class Rasterizer final
{
public:
	explicit Rasterizer(ID3D11Device* device);
	~Rasterizer() = default;

	void Activate(ID3D11DeviceContext* immediate_context, RasterizeState state)
	{
		if (current_state == state)
			return;

		current_state = state;
		immediate_context->RSSetState(rasterizerStates.at(static_cast<size_t>(state)).Get());
	}

private:
	static RasterizeState current_state;

	std::array<ComPtr<ID3D11RasterizerState>, static_cast<u_int>(RasterizeState::End)>		rasterizerStates = { nullptr };
};