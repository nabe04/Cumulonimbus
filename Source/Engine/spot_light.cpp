#include "spot_light.h"

#include <d3d11.h>

//namespace shader
//{
//	SpotLight::SpotLight(ID3D11Device* device)
//		:Shader{ device }
//	{
//		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
//		pixel_shader = std::make_unique<PixelShader>(device, ps_name);
//
//		//D3D11_BUFFER_DESC bd{};
//		//bd.Usage			= D3D11_USAGE_DEFAULT;
//		//bd.ByteWidth		= sizeof(spot_light::CB_SpotLight);
//		//bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
//		//bd.CPUAccessFlags	= 0;
//
//		//HRESULT hr = device->CreateBuffer(&bd, NULL, c_buffer.GetAddressOf());
//		//if (FAILED(hr))
//		//	assert(!"CreateBuffer (Constant Buffer of SpotShader) error");
//	}
//
//	void SpotLight::Activate(ID3D11DeviceContext* immediate_context)
//	{
//		material_data->Activate(immediate_context, 6);
//
//		vertex_shader->Activate(immediate_context);
//		pixel_shader->Activate(immediate_context);
//	}
//
//	void SpotLight::Deactivate(ID3D11DeviceContext* immediate_context)
//	{
//		vertex_shader->Deactivate(immediate_context);
//		pixel_shader->Deactivate(immediate_context);
//
//		material_data->Deactivate(immediate_context);
//	}
//
//}