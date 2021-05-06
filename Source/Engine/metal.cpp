#include "metal.h"

namespace shader
{
	Metal::Metal(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
		pixel_shader = std::make_unique<PixelShader>(device, ps_name);
	}

	void Metal::Activate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);

		material_data->Activate(immediate_context, 6);
	}

	void Metal::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		material_data->Deactivate(immediate_context);

		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);
	}
}