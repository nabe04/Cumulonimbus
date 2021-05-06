#include "rasterizer.h"

#include <assert.h>

RasterizeState Rasterizer::current_state = { RasterizeState::End };

//----------------------------------------
//	Create RasterizerState
//----------------------------------------
Rasterizer::Rasterizer(ID3D11Device* device)
{
	D3D11_RASTERIZER_DESC rd;
	for (int state = 0; state < static_cast<u_int>(RasterizeState::End); state++)
	{
		switch (state)
		{
			case static_cast<u_int>(RasterizeState::Cull_Back) :
				rd							= {};
				rd.FillMode					= D3D11_FILL_SOLID;
				rd.CullMode					= D3D11_CULL_BACK;
				rd.FrontCounterClockwise	= FALSE;
				rd.DepthBias				= 0;
				rd.DepthBiasClamp			= 0;
				rd.SlopeScaledDepthBias		= 0;
				rd.DepthClipEnable			= TRUE;
				rd.ScissorEnable			= FALSE;
				rd.MultisampleEnable		= TRUE;
				rd.AntialiasedLineEnable	= FALSE;
			break;
			case static_cast<u_int>(RasterizeState::Cull_Front) :
				rd							= {};
				rd.FillMode					= D3D11_FILL_SOLID;
				rd.CullMode					= D3D11_CULL_FRONT;
				rd.FrontCounterClockwise	= FALSE;
				rd.DepthBias				= 0;
				rd.DepthBiasClamp			= 0;
				rd.SlopeScaledDepthBias		= 0;
				rd.DepthClipEnable			= TRUE;
				rd.ScissorEnable			= FALSE;
				rd.MultisampleEnable		= TRUE;
				rd.AntialiasedLineEnable	= FALSE;
			break;
			case static_cast<u_int>(RasterizeState::Cull_None) :
				rd							= {};
				rd.FillMode					= D3D11_FILL_SOLID;
				rd.CullMode					= D3D11_CULL_NONE;
				rd.FrontCounterClockwise	= FALSE;
				rd.DepthBias				= 0;
				rd.DepthBiasClamp			= 0;
				rd.SlopeScaledDepthBias		= 0;
				rd.DepthClipEnable			= TRUE;
				rd.ScissorEnable			= FALSE;
				rd.MultisampleEnable		= TRUE;
				rd.AntialiasedLineEnable	= FALSE;
			break; case static_cast<u_int>(RasterizeState::Cull_Back_CCW_True) :
				rd = {};
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_BACK;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = FALSE;
			break;
			case static_cast<u_int>(RasterizeState::Cull_Front_CCW_True) :
				rd = {};
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_FRONT;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = FALSE;
			break;
			case static_cast<u_int>(RasterizeState::Cull_None_CCW_True) :
				rd = {};
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_NONE;
				rd.FrontCounterClockwise = TRUE;
				rd.DepthBias = 0;
				rd.DepthBiasClamp = 0;
				rd.SlopeScaledDepthBias = 0;
				rd.DepthClipEnable = TRUE;
				rd.ScissorEnable = FALSE;
				rd.MultisampleEnable = TRUE;
				rd.AntialiasedLineEnable = FALSE;
			break;
			case static_cast<u_int>(RasterizeState::Wireframe) :
				rd							= {};
				rd.FillMode					= D3D11_FILL_WIREFRAME;
				rd.CullMode					= D3D11_CULL_NONE;
				rd.FrontCounterClockwise	= FALSE;
				rd.DepthBias				= 0;
				rd.DepthBiasClamp			= 0;
				rd.SlopeScaledDepthBias		= 0;
				rd.DepthClipEnable			= TRUE;
				rd.ScissorEnable			= FALSE;
				rd.MultisampleEnable		= TRUE;
				rd.AntialiasedLineEnable	= FALSE;
			break;
		}
		HRESULT hr = device->CreateRasterizerState(&rd, rasterizerStates[state].GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateRasterizerState errror");
	}
}
