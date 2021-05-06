#include "sampler.h"

#include <float.h>
#include <assert.h>

//Sampler::Sampler(ID3D11Device* device)
//{
//	D3D11_SAMPLER_DESC sampler_desc = {};
//	{// Any
//		sampler_desc.Filter				= D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
//		sampler_desc.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
//		sampler_desc.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
//		sampler_desc.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
//		sampler_desc.MipLODBias			= 0.0f;
//		sampler_desc.MaxAnisotropy		= 16;
//		sampler_desc.ComparisonFunc		= D3D11_COMPARISON_ALWAYS;
//		sampler_desc.BorderColor[0]		= 1.0f;
//		sampler_desc.BorderColor[1]		= 1.0f;
//		sampler_desc.BorderColor[2]		= 1.0f;
//		sampler_desc.BorderColor[3]		= 1.0f;
//		sampler_desc.MinLOD				= -FLT_MAX;
//		sampler_desc.MaxLOD				= FLT_MAX;
//	}
//
//	HRESULT hr = device->CreateSamplerState(&sampler_desc, sampler_states[static_cast<u_int>(SamplerState::Any)].GetAddressOf());
//	if (FAILED(hr))
//		assert(!"CreateSamplerState error");
//}

//void Sampler::Activate(ID3D11DeviceContext* immediate_context, const SamplerState& state)
//{
//	if (state == current_state)
//		return;
//
//	current_state = state;
//
//	immediate_context->PSSetSamplers(0, 1, sampler_states.at(static_cast<u_int>(current_state)).GetAddressOf());
//}