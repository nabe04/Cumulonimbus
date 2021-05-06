#include "standard_sprite.h"

#include <array>

namespace shader
{
	StandardSprite::StandardSprite(ID3D11Device* device)
		: Shader{ device }
	{
		// Setting the InputLayout
		std::array<D3D11_INPUT_ELEMENT_DESC, 3 > layout =
		{
		 {
			{ "POSITION",	0 , DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0 , DXGI_FORMAT_R32G32_FLOAT ,		0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",		0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		 }
		};

		vertex_shader = std::make_unique<VertexShader>(device, vs_name, layout.data(), layout.size());
		pixel_shader  = std::make_unique<PixelShader>(device, ps_name);
	}

	void StandardSprite::Activate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void StandardSprite::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);
	}
}