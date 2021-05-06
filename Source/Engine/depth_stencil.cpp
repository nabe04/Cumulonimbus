#include "depth_stencil.h"

#include <assert.h>

DepthStencil::DepthStencil(ID3D11Device* device)
{
	//-- Set up and create DepthStencilState --//
	D3D11_DEPTH_STENCIL_DESC	depth_stencil_desc = {};
	HRESULT						hr	= E_FAIL;

	{// Z test off
		depth_stencil_desc.DepthEnable = FALSE;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<u_int>(DepthStencilState::DepthTest_False)].GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateDepthStencilState error");
	}

	{// Z test on , Write on
		depth_stencil_desc = {};
		depth_stencil_desc.DepthEnable		= TRUE;
		depth_stencil_desc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc		= D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<u_int>(DepthStencilState::DepthTest_True_Write_True)].GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateDepthStencilState error");
	}

	{// Z test on , Write off
		depth_stencil_desc.DepthEnable		= TRUE;
		depth_stencil_desc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc		= D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<u_int>(DepthStencilState::DepthTest_True_Write_False)].GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateDepthStencilState error");
	}

	{
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<u_int>(DepthStencilState::Depth_First)].GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateDepthStencilState error");
	}
}

void DepthStencil::Activate(ID3D11DeviceContext* immediate_context, const DepthStencilState& state)
{
	// Set of DepthStencilState
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<u_int>(state)].Get(), 1);
}