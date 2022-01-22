#include "sampler.h"

Sampler::Sampler(
	ID3D11Device* device, const D3D11_FILTER filter,
	const D3D11_TEXTURE_ADDRESS_MODE address_mode,
	const D3D11_COMPARISON_FUNC comparison_func,
	const float r, const float g, const float b, const float a)
{
	// custom_sampler_state�̍쐬
	CreateSamplerState(
		custom_sampler_state.GetAddressOf(),
		device, filter, address_mode,
		comparison_func, r, g, b, a);
	//default_sampler_states�̍쐬
	CreateSamplerState(
		default_sampler_states.at(static_cast<u_int>(SamplerState::Linear_Border)).GetAddressOf(),
		device, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_COMPARISON_ALWAYS,
		0.0f, 0.0f, 0.0f, 1.0f);
	CreateSamplerState(
		default_sampler_states.at(static_cast<u_int>(SamplerState::Point_Wrap)).GetAddressOf(),
		device, D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS,
		0.0f, 0.0f, 0.0f, 1.0f);
	CreateSamplerState(
		default_sampler_states.at(static_cast<u_int>(SamplerState::Linear_Wrap)).GetAddressOf(),
		device, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS,
		0.0f, 0.0f, 0.0f, 1.0f);
	CreateSamplerState(
		default_sampler_states.at(static_cast<u_int>(SamplerState::Anistropic)).GetAddressOf(),
		device, D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS,
		0.0f, 0.0f, 0.0f, 1.0f);
	CreateSamplerState(
		default_sampler_states.at(static_cast<u_int>(SamplerState::Mirror)).GetAddressOf(),
		device, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_COMPARISON_ALWAYS,
		0.0f, 0.0f, 0.0f, 1.0f);
}

void Sampler::Activate(ID3D11DeviceContext* immediate_context, unsigned slot, bool set_in_vs)
{
	using_slot = slot;
	immediate_context->PSGetSamplers(using_slot, 1, save_sampler_states[0].ReleaseAndGetAddressOf());
	immediate_context->PSSetSamplers(using_slot, 1, custom_sampler_state.GetAddressOf());
	if (set_in_vs)
	{
		immediate_context->VSGetSamplers(using_slot, 1, save_sampler_states[1].ReleaseAndGetAddressOf());
		immediate_context->VSSetSamplers(using_slot, 1, custom_sampler_state.GetAddressOf());

	}
}

void Sampler::Activate(ID3D11DeviceContext* immediate_context, SamplerState state, unsigned slot, bool set_in_vs)
{
	using_slot = slot;
	immediate_context->PSGetSamplers(using_slot, 1, save_sampler_states[0].ReleaseAndGetAddressOf());
	immediate_context->PSSetSamplers(using_slot, 1, default_sampler_states.at(static_cast<u_int>(state)).GetAddressOf());
	if (set_in_vs)
	{
		immediate_context->VSGetSamplers(using_slot, 1, save_sampler_states[1].ReleaseAndGetAddressOf());
		immediate_context->VSSetSamplers(using_slot, 1, default_sampler_states.at(static_cast<u_int>(state)).GetAddressOf());

	}
}


void Sampler::Deactivate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->PSSetSamplers(using_slot, 1, save_sampler_states[0].GetAddressOf());
	immediate_context->VSSetSamplers(using_slot, 1, save_sampler_states[1].GetAddressOf());
}

void Sampler::CreateSamplerState(
	ID3D11SamplerState** sampler,
	ID3D11Device* device, const D3D11_FILTER filter,
	const D3D11_TEXTURE_ADDRESS_MODE address_mode,
	const D3D11_COMPARISON_FUNC comparison_func,
	const float r, const float g, const float b, const float a)
{
	// custom_sampler_state�̍쐬
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = filter;
	sampler_desc.AddressU = address_mode;
	sampler_desc.AddressV = address_mode;
	sampler_desc.AddressW = address_mode;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = comparison_func;
	float border_colour[4] = { r, g, b, a };
	memcpy(sampler_desc.BorderColor, border_colour, sizeof(float) * 4);
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = device->CreateSamplerState(&sampler_desc, sampler);
	if (FAILED(hr))
		assert(!"CreateSamplerState error");
}



