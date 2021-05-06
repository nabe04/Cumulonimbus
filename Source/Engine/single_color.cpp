#include "single_color.h"

namespace shader
{
	SingleColor::SingleColor(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
		pixel_shader = std::make_unique<PixelShader>(device, ps_name);
	}

	void SingleColor::Activate(ID3D11DeviceContext* immediate_context)
	{
		material_data->Activate(immediate_context, 6, false);

		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void SingleColor::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		pixel_shader->Deactivate(immediate_context);
		vertex_shader->Deactivate(immediate_context);

		material_data->Deactivate(immediate_context);
	}
}