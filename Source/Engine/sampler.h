#pragma once

#include <d3d11.h>
#include <wrl.h>

#include <array>
#include <assert.h>
#include <map>

enum  RenderingSampleState
{
	Linear_Border,
	Point_Wrap,
	Linear_Wrap,
	Anistropic,

	End,
};
class Sampler final
{
public:
	explicit Sampler(ID3D11Device* device, D3D11_FILTER filter = D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_MODE address_mode = D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_FUNC comparison_func = D3D11_COMPARISON_NEVER, float r = 1, float g = 1, float b = 1, float a = 1)
	{
		//4096 unique sampler state objects can be created on a device at a time.
			//If an application attempts to create a sampler - state interface with the same state as an existing interface,
			//the same interface will be returned and the total number of unique sampler state objects will stay the same.
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
		HRESULT hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateSamplerState error");
	}
	~Sampler() = default;

	void Activate(ID3D11DeviceContext* immediate_context,unsigned int slot, bool set_in_vs = false)
	{
		using_slot = slot;
		immediate_context->PSGetSamplers(using_slot, 1, default_sampler_states[0].ReleaseAndGetAddressOf());
		immediate_context->VSSetSamplers(using_slot, 1, sampler_state.GetAddressOf());

		immediate_context->PSSetSamplers(using_slot, 1, sampler_state.GetAddressOf());
		if (set_in_vs)
		{
			immediate_context->VSGetSamplers(using_slot, 1, default_sampler_states[1].ReleaseAndGetAddressOf());
		}
	}

	void Deactivate(ID3D11DeviceContext* immediate_context)
	{
		immediate_context->PSSetSamplers(using_slot, 1, default_sampler_states[0].GetAddressOf());
		immediate_context->VSSetSamplers(using_slot, 1, default_sampler_states[1].GetAddressOf());
	}

private:
	unsigned int using_slot = 0;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
	std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, 2> default_sampler_states;
};