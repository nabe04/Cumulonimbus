#include "blend.h"

#include <assert.h>

Blend::Blend(ID3D11Device* device)
{
	D3D11_BLEND_DESC bd{};

	for (int state = 0; state < static_cast<u_int>(BlendState::End); state++)
	{
		switch (state)
		{
			case static_cast<u_int>(BlendState::None) :
				bd										 = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = FALSE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_ADD;
				//-- Settings for Alpha
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			case static_cast<u_int>(BlendState::Alpha) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend		     = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_ADD;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			case static_cast<u_int>(BlendState::Add) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_ADD;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			case static_cast<u_int>(BlendState::Subtract) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_REV_SUBTRACT;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			case static_cast<u_int>(BlendState::Replace) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_ADD;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break
				;
			case static_cast<u_int>(BlendState::Multiply) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_DEST_COLOR;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_ZERO;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_ADD;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_DEST_ALPHA;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			case static_cast<u_int>(BlendState::Lighten) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_MAX;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_MAX;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			case static_cast<u_int>(BlendState::Darken) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_MIN;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_MIN;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			case static_cast<u_int>(BlendState::Screen) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = FALSE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_INV_SRC_COLOR;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_ADD;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;

			case static_cast<u_int>(BlendState::AlphaToCoverage) :
				bd = {};
				bd.IndependentBlendEnable				 = FALSE;
				bd.AlphaToCoverageEnable				 = TRUE;
				bd.RenderTarget[0].BlendEnable			 = TRUE;
				//-- Settings for RGB --//
				bd.RenderTarget[0].SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[0].DestBlend			 = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOp				 = D3D11_BLEND_OP_ADD;
				//-- Settings for Alpha --//
				bd.RenderTarget[0].SrcBlendAlpha		 = D3D11_BLEND_ONE;
				bd.RenderTarget[0].DestBlendAlpha		 = D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[0].BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
				bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			break;
		}
		HRESULT hr = device->CreateBlendState(&bd, blend_states[state].GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateBlendState error");
	}
}
